/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file 

    Hardware test to demonstrate raw thread failure detection.
    
    This test verifies that when a raw thread gets stuck in a busy loop
    and exceeds its thread-specific watchdog timeout, the supervisor
    detects this failure and trips the MCU watchdog, causing a system reset.
    
    Expected behavior:
    - First run: System starts normally, raw thread gets stuck after delay
    - Supervisor detects raw thread timeout and trips the watchdog
    - Watchdog resets the MCU
    - Second run: Detects watchdog reset flag and indicates test passed

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

#define SECT_ "_hw_rawthread"

///
using namespace Kit::System;
using namespace Kit::System::Watchdog;
using namespace Kit::EventQueue;

//------------------------------------------------------------------------------
// Test Configuration
//------------------------------------------------------------------------------

/** Hardware watchdog timeout is 1000ms (from HAL implementation).
*/
#define HW_WATCHDOG_TIMEOUT_MS     1000

/** Supervisor thread watchdog timeout.
    Must be less than HW watchdog timeout.
*/
#define SUPERVISOR_WDOG_TIMEOUT_MS 800

/** Supervisor thread health check interval.
    Must be less than SUPERVISOR_WDOG_TIMEOUT_MS.
*/
#define SUPERVISOR_HEALTH_CHECK_MS 200

/** Raw thread watchdog timeout.
    This is shorter than the supervisor timeout to ensure the raw thread
    failure is detected and handled appropriately.
*/
#define RAW_THREAD_WDOG_TIMEOUT_MS 400

/** Time to run normally before triggering the stuck condition.
    This gives the watchdog time to initialize and start running properly.
*/
#define NORMAL_RUN_DELAY_MS        2000

/** How long the raw thread will be stuck.
    This must exceed RAW_THREAD_WDOG_TIMEOUT_MS to trigger watchdog trip.
*/
#define STUCK_DURATION_MS          ( RAW_THREAD_WDOG_TIMEOUT_MS + 500 )

//------------------------------------------------------------------------------
// Raw Thread Implementation
//------------------------------------------------------------------------------

/// Counter for raw thread to track when to get stuck
static volatile uint32_t rawThreadCounter_ = 0;

/// Flag to trigger the stuck condition
static volatile bool triggerRawThreadStuck_ = false;

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
        KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread starting (wdog timeout: %u ms)",
                              (unsigned)RAW_THREAD_WDOG_TIMEOUT_MS );

        // Start watchdog monitoring for this thread
        KIT_SYSTEM_WATCHDOG_START_RAWTHREAD( m_wdog );

        // Run normally until we're told to get stuck
        while ( !triggerRawThreadStuck_ )
        {
            // Do some work
            Bsp_toggle_debug2();

            // Kick the watchdog to indicate we're alive
            KIT_SYSTEM_WATCHDOG_KICK_RAWTHREAD( m_wdog );

            // Wait a bit (less than our watchdog timeout)
            HAL_Delay( 100 );
            rawThreadCounter_ += 100;
        }

        // Now get stuck in a busy loop WITHOUT kicking the watchdog
        KIT_SYSTEM_TRACE_MSG( SECT_, "*** RAW THREAD STUCK - Entering busy loop ***" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Busy loop will last %u ms (thread timeout is %u ms)",
                              STUCK_DURATION_MS, RAW_THREAD_WDOG_TIMEOUT_MS );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor should detect timeout and trip watchdog" );

        uint32_t startTime = HAL_GetTick();
        while ( ( HAL_GetTick() - startTime ) < STUCK_DURATION_MS )
        {
            // Busy wait - watchdog will NOT be kicked
            Bsp_toggle_debug2();
            // Small delay to make LED visible
            for ( volatile uint32_t i = 0; i < 100000; i++ )
            {
                __NOP();
            }
        }

        // If we get here, the watchdog failed to reset us
        KIT_SYSTEM_TRACE_MSG( SECT_, "*** ERROR: Watchdog did not reset system! ***" );

        // Stop watchdog monitoring
        KIT_SYSTEM_WATCHDOG_STOP_RAWTHREAD( m_wdog );
    }
};

//------------------------------------------------------------------------------
// Main Application
//------------------------------------------------------------------------------

int main( void )
{
    // Initialize the board (HAL, clocks, GPIOs, UART)
    Bsp_initialize();

    // Initialize the KIT system
    Kit::System::initialize();

    KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Raw Thread Stuck Test" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "HW Watchdog Timeout: %u ms", HW_WATCHDOG_TIMEOUT_MS );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Raw Thread Timeout: %u ms", RAW_THREAD_WDOG_TIMEOUT_MS );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Stuck Duration: %u ms", STUCK_DURATION_MS );
    KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );

    // Check if this is a watchdog reset
    if ( __HAL_RCC_GET_FLAG( RCC_FLAG_IWDGRST ) != RESET )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "*** WATCHDOG RESET DETECTED ***" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Test PASSED" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "System was correctly reset by watchdog" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "when raw thread exceeded its timeout" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );
        __HAL_RCC_CLEAR_RESET_FLAGS();

        // Keep both LEDs on to indicate success
        Bsp_turn_on_debug1();
        Bsp_turn_on_debug2();

        // Infinite loop - test is complete
        while ( 1 )
        {
            HAL_Delay( 1000 );
        }
    }

    // First run - set up the test
    KIT_SYSTEM_TRACE_MSG( SECT_, "First run - setting up watchdog test" );

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

    // Run normally for a bit to establish that watchdog is working
    KIT_SYSTEM_TRACE_MSG( SECT_, "Running normally for %u ms...", NORMAL_RUN_DELAY_MS );

    uint32_t mainCounter = 0;
    while ( mainCounter < NORMAL_RUN_DELAY_MS )
    {
        Bsp_toggle_debug1();
        HAL_Delay( 500 );
        mainCounter += 500;
    }

    // Now trigger the stuck condition in the raw thread
    KIT_SYSTEM_TRACE_MSG( SECT_, "Triggering raw thread stuck condition..." );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread will stop kicking its watchdog" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Waiting for supervisor to detect and trip watchdog..." );

    triggerRawThreadStuck_ = true;

    // Wait for the watchdog to reset us
    // The raw thread will get stuck, supervisor will detect it, and trip the watchdog
    for ( int i = 0; i < 20; i++ )
    {
        HAL_Delay( 500 );
        Bsp_toggle_debug1();
        KIT_SYSTEM_TRACE_MSG( SECT_, "Still waiting for reset... (%d)", i );
    }

    // If we get here, something went wrong
    KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Test FAILED" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Watchdog did not reset the system" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread timeout was not detected" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );

    // Blink LED rapidly to indicate failure
    while ( 1 )
    {
        Bsp_toggle_debug1();
        HAL_Delay( 100 );
    }

    return 0;
}
