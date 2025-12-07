/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file 

    Hardware test to verify basic watchdog functionality on STM32F4 target.
    
    This test verifies:
    1. The watchdog can be enabled
    2. One event thread (supervisor) can be monitored
    3. One raw thread can be monitored
    4. Threads can have different health-check intervals
    5. System runs for at least 5x the hardware watchdog timeout period
       without triggering a reset (demonstrating proper watchdog kicking)

*/

#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Thread.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Watchdog/Supervisor.h"
#include "Kit/System/Watchdog/WatchedEventThread.h"
#include "Kit/System/Watchdog/WatchedRawThread.h"
#include "Kit/System/EventLoop.h"
#include "Kit/EventQueue/Server.h"
#include <cstdint>

#define SECT_ "_hw_wdogenable"

///
using namespace Kit::System;
using namespace Kit::System::Watchdog;
using namespace Kit::EventQueue;

//------------------------------------------------------------------------------
// Test Configuration
//------------------------------------------------------------------------------

/** Hardware watchdog timeout is 1000ms (from HAL implementation).
    We want to run for at least 5x this period to demonstrate proper operation.
*/
#define HW_WATCHDOG_TIMEOUT_MS     1000
#define TEST_DURATION_MS           ( HW_WATCHDOG_TIMEOUT_MS * 5 )

/** Supervisor thread watchdog timeout.
    This should be less than the hardware watchdog timeout to allow time
    for the supervisor to kick the hardware watchdog.
*/
#define SUPERVISOR_WDOG_TIMEOUT_MS 800

/** Supervisor thread health check interval.
    This is how often the supervisor checks its own health.
    Must be less than SUPERVISOR_WDOG_TIMEOUT_MS.
*/
#define SUPERVISOR_HEALTH_CHECK_MS 200

/** Raw thread watchdog timeout.
    This is how long the raw thread can go without checking in.
*/
#define RAW_THREAD_WDOG_TIMEOUT_MS 600

//------------------------------------------------------------------------------
// Raw Thread Implementation
//------------------------------------------------------------------------------

/// Counter for raw thread iterations
static volatile uint32_t rawThreadCounter_ = 0;

/// Runnable for the raw thread
class RawThreadRunnable : public IRunnable
{
protected:
    /// Watchdog monitor for this raw thread
    RawThread m_wdog;

public:
    /// Constructor
    RawThreadRunnable()
        : m_wdog( RAW_THREAD_WDOG_TIMEOUT_MS )
    {
    }

public:
    /// Thread entry point
    void entry() noexcept override
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread starting" );

        // Start watchdog monitoring for this thread
        KIT_SYSTEM_WATCHDOG_START_RAWTHREAD( m_wdog );

        // Run for the test duration
        uint32_t iterations = 0;
        while ( rawThreadCounter_ < TEST_DURATION_MS )
        {
            // Do some work
            iterations++;
            Bsp_toggle_debug2();

            // Kick the watchdog to indicate we're alive
            KIT_SYSTEM_WATCHDOG_KICK_RAWTHREAD( m_wdog );

            // Wait a bit (less than our watchdog timeout)
            HAL_Delay( 100 );

            rawThreadCounter_ += 100;
        }

        KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread completed %lu iterations", iterations );

        // Stop watchdog monitoring
        KIT_SYSTEM_WATCHDOG_STOP_RAWTHREAD( m_wdog );
    }
};

//------------------------------------------------------------------------------
// Event Thread (Supervisor) Implementation
//------------------------------------------------------------------------------

/// Counter for supervisor iterations
static volatile uint32_t supervisorCounter_ = 0;

//------------------------------------------------------------------------------
// Main Application
//------------------------------------------------------------------------------

int main( void )
{
    // Initialize the board (HAL, clocks, GPIOs, UART)
    Bsp_initialize();

    // Initialize the KIT system
    Kit::System::initialize();

    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "_hw_wdogenable" );
    //    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eINFO );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eVERBOSE );
    KIT_SYSTEM_TRACE_MSG( SECT_, "KIT System initialized" );

    KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Watchdog Enable Test Starting" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "HW Watchdog Timeout: %u ms", HW_WATCHDOG_TIMEOUT_MS );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Test Duration: %u ms (5x HW timeout)", TEST_DURATION_MS );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor Wdog Timeout: %u ms", SUPERVISOR_WDOG_TIMEOUT_MS );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor Health Check: %u ms", SUPERVISOR_HEALTH_CHECK_MS );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Raw Thread Wdog Timeout: %u ms", RAW_THREAD_WDOG_TIMEOUT_MS );
    KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );

    // Check if this is a watchdog reset
    if ( __HAL_RCC_GET_FLAG( RCC_FLAG_IWDGRST ) != RESET )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "*** UNEXPECTED WATCHDOG RESET DETECTED ***" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Test FAILED - System was reset by watchdog" );
        Bsp_turn_on_debug1();
        __HAL_RCC_CLEAR_RESET_FLAGS();

        // Blink LED rapidly to indicate failure
        while ( 1 )
        {
            Bsp_toggle_debug1();
            HAL_Delay( 100 );
        }
    }

    // Configure the supervisor thread (event thread)
    WatchedEventThread supervisorWdog( SUPERVISOR_WDOG_TIMEOUT_MS,
                                       SUPERVISOR_HEALTH_CHECK_MS,
                                       true );  // This is the supervisor thread

    // Create the event loop for the supervisor thread
    Server eventLoop( OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
                      nullptr,
                      &supervisorWdog );

    // Create and start the supervisor thread
    auto* supervisorThread = Thread::create( eventLoop, "SUPERVISOR" );
    if ( !supervisorThread )
    {
        FatalError::logf( Shutdown::eFAILURE, "Failed to create supervisor thread" );
    }

    // Enable the hardware watchdog
    KIT_SYSTEM_TRACE_MSG( SECT_, "Enabling hardware watchdog..." );
    if ( !Supervisor::enableWdog() )
    {
        FatalError::logf( Shutdown::eFAILURE, "Failed to enable hardware watchdog" );
    }
    KIT_SYSTEM_TRACE_MSG( SECT_, "Hardware watchdog enabled successfully" );

    // Create and start the raw thread
    RawThreadRunnable rawRunnable;
    auto* rawThread = Thread::create( rawRunnable, "RAW_THREAD" );
    if ( !rawThread )
    {
        FatalError::logf( Shutdown::eFAILURE, "Failed to create raw thread" );
    }

    // Monitor progress
    KIT_SYSTEM_TRACE_MSG( SECT_, "Test running..." );

    while ( rawThreadCounter_ < TEST_DURATION_MS )
    {
        // Toggle LED to show we're alive
        Bsp_toggle_debug1();
        HAL_Delay( 500 );
        supervisorCounter_ += 500;
    }

    // Test completed successfully
    KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Test PASSED" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Ran for %u ms without watchdog reset", TEST_DURATION_MS );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor iterations: %lu", supervisorCounter_ / 500 );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread iterations: %lu", rawThreadCounter_ / 100 );
    KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );

    // Stop the threads gracefully
    eventLoop.pleaseStop();
    HAL_Delay( 100 );

    // Keep LED on to indicate success
    Bsp_turn_on_debug1();
    Bsp_turn_on_debug2();

    // Infinite loop - test is complete
    while ( 1 )
    {
        HAL_Delay( 1000 );
    }

    return 0;
}
