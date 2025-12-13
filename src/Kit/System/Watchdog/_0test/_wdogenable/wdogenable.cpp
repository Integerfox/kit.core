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
#include <string.h>

#define SECT_ "_0test"

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
#define SUPERVISOR_HEALTH_CHECK_MS 100

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
    /** Constructor. Initializes the raw thread with watchdog monitoring.
     */
    RawThreadRunnable() noexcept
        : m_wdog( RAW_THREAD_WDOG_TIMEOUT_MS )
    {
    }

public:
    /// Thread entry point
    void entry() noexcept override
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread starting\r\n" );

        // Start watchdog monitoring for this thread
        KIT_SYSTEM_WATCHDOG_START_RAWTHREAD( m_wdog );

        // Run for the test duration
        uint32_t iterations = 0;
        while ( rawThreadCounter_ < TEST_DURATION_MS )
        {
            // Do some work
            ++iterations;
            Bsp_toggle_debug2();

            // Kick the watchdog to indicate we're alive
            KIT_SYSTEM_WATCHDOG_KICK_RAWTHREAD( m_wdog );

            // Wait a bit (less than our watchdog timeout)
            sleep( 100 );

            rawThreadCounter_ += 100;
        }

        KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread completed %lu iterations\r\n", iterations );

        // Stop watchdog monitoring
        KIT_SYSTEM_WATCHDOG_STOP_RAWTHREAD( m_wdog );

        // Instead of terminating, go into idle loop
        for ( ;; )
        {
            sleep( 1000 );
        }
    }
};

//------------------------------------------------------------------------------
// Event Thread (Supervisor) Implementation
//------------------------------------------------------------------------------

/// Counter for supervisor iterations
static volatile uint32_t supervisorCounter_ = 0;

/// Custom supervisor watchdog that adds trace output
class SupervisorWatchdog : public WatchedEventThread
{
public:
    /** Constructor
        @param wdogTimeoutMs Watchdog timeout period in milliseconds
        @param healthCheckIntervalMs Health check interval in milliseconds
        @param isSupervisor Flag indicating if this is the supervisor thread
     */
    SupervisorWatchdog( uint32_t wdogTimeoutMs, uint32_t healthCheckIntervalMs, bool isSupervisor ) noexcept
        : WatchedEventThread( wdogTimeoutMs
                            , healthCheckIntervalMs
                            , isSupervisor )
    {
    }

protected:
    bool performHealthCheck() noexcept override
    {
        //KIT_SYSTEM_TRACE_MSG( SECT_, "***** SUPERVISOR HEALTH CHECK - GOOD *****\r\n" );
        return true;  // Always healthy
    }
};

//------------------------------------------------------------------------------
// Test Monitor Thread
//------------------------------------------------------------------------------

/// Test monitor runnable that coordinates the test
class TestMonitor : public IRunnable
{
public:
    void entry() noexcept override
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Test monitor starting\r\n" );
        
        // Give supervisor thread time to start, initialize, and have its
        // first health check timer fire. This is critical because the
        // supervisor needs to reload its watchdog timer via the health check
        // before the monitoring system will allow hardware watchdog kicks.
        sleep( 1000 );

        if ( !Supervisor::enableWdog() )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "FAILED to enable hardware watchdog!\r\n" );
            FatalError::logf( Shutdown::eFAILURE, "Failed to enable hardware watchdog\r\n" );
        }

        KIT_SYSTEM_TRACE_MSG( SECT_, "Hardware watchdog enabled successfully\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Test running - monitoring for 5 seconds...\r\n" );

        // Monitor progress for test duration
        while ( rawThreadCounter_ < TEST_DURATION_MS )
        {
            // Toggle LED to show we're alive
            Bsp_toggle_debug1();
            sleep( 500 );
            supervisorCounter_ += 500;
        }

        // Test completed successfully
        KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Test PASSED\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Ran for %u ms without watchdog reset\r\n", (unsigned)TEST_DURATION_MS );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor iterations: %lu\r\n", (unsigned long)(supervisorCounter_ / 500) );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread iterations: %lu\r\n", (unsigned long)(rawThreadCounter_ / 100) );
        KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================\r\n" );

        // Keep LEDs on to indicate success
        Bsp_turn_on_debug1();
        Bsp_turn_on_debug2();

        // Test complete - idle forever
        for ( ;; )
        {
            sleep( 1000 );
        }
    }
};

// Allocating static storage for objects that must persist after main() exits
// when the scheduler starts. These cannot be stack-allocated because FreeRTOS
// will reuse/corrupt the main stack.

static uint8_t supervisorWdogStorage_[sizeof(SupervisorWatchdog)] __attribute__((aligned(8)));
static uint8_t eventLoopStorage_[sizeof(Server)] __attribute__((aligned(8)));
static uint8_t rawRunnableStorage_[sizeof(RawThreadRunnable)] __attribute__((aligned(8)));
static uint8_t testMonitorStorage_[sizeof(TestMonitor)] __attribute__((aligned(8)));

//------------------------------------------------------------------------------
// Main Application
//------------------------------------------------------------------------------

int main( void )
{
    // Initialize the board (HAL, clocks, GPIOs, UART)
    Bsp_initialize();

    // Small delay to let UART stabilize
    HAL_Delay( 200 );

    const char* msg = "\r\n\r\n**** WATCHDOG ENABLE TEST START ****\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), 1000);
    HAL_Delay( 100 );

    // Initialize the KIT system
    Kit::System::initialize();

    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eVERBOSE );

    // Check if this is a watchdog reset BEFORE printing anything else
    bool isWatchdogReset = ( __HAL_RCC_GET_FLAG( RCC_FLAG_IWDGRST ) != RESET );
    if ( isWatchdogReset )
    {
        const char* failMsg = "*** UNEXPECTED WATCHDOG RESET DETECTED ***\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t*)failMsg, strlen(failMsg), 1000);
        const char* failMsg2 = "Test FAILED - System was reset by watchdog\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t*)failMsg2, strlen(failMsg2), 1000);

        __HAL_RCC_CLEAR_RESET_FLAGS();
        Bsp_turn_on_debug1();

        // Blink LED rapidly to indicate failure
        while ( 1 )
        {
            Bsp_toggle_debug1();
            HAL_Delay( 100 );
        }
    }

    const char* msg2 = "KIT System initialized\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t*)msg2, strlen(msg2), 1000);

    char buf[200];
    snprintf(buf, sizeof(buf), "Watchdog Enable Test Starting\r\nHW Watchdog Timeout: %u ms\r\nTest Duration: %u ms\r\n", 
             HW_WATCHDOG_TIMEOUT_MS, TEST_DURATION_MS);
    HAL_UART_Transmit(&huart3, (uint8_t*)buf, strlen(buf), 1000);

    // Configure the supervisor thread (event thread)
    SupervisorWatchdog* supervisorWdog = new (supervisorWdogStorage_) SupervisorWatchdog( 
        SUPERVISOR_WDOG_TIMEOUT_MS,
        SUPERVISOR_HEALTH_CHECK_MS,
        true );  // This is the supervisor thread

    // Create the event loop for the supervisor thread
    Server* eventLoop = new (eventLoopStorage_) Server( 
        OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
        nullptr,
        supervisorWdog );

    // Create and start the supervisor thread
    auto* supervisorThread = Thread::create( *eventLoop, "SUPERVISOR" );
    if ( !supervisorThread )
    {
        FatalError::logf( Shutdown::eFAILURE, "Failed to create supervisor thread" );
    }

    const char* msg4 = "Supervisor thread created\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t*)msg4, strlen(msg4), 1000);

    // Create and start the raw thread
    RawThreadRunnable* rawRunnable = new (rawRunnableStorage_) RawThreadRunnable();
    auto* rawThread = Thread::create( *rawRunnable, "RAW_THREAD" );
    if ( !rawThread )
    {
        FatalError::logf( Shutdown::eFAILURE, "Failed to create raw thread" );
    }

    const char* msg5 = "Raw thread created\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t*)msg5, strlen(msg5), 1000);

    // Create and start the test monitor thread
    TestMonitor* testMonitor = new (testMonitorStorage_) TestMonitor();
    auto* monitorThread = Thread::create( *testMonitor, "TEST_MONITOR" );
    if ( !monitorThread )
    {
        FatalError::logf( Shutdown::eFAILURE, "Failed to create test monitor thread" );
    }

    const char* msg6 = "Test monitor thread created\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t*)msg6, strlen(msg6), 1000);

    KIT_SYSTEM_TRACE_MSG( SECT_, "Starting scheduler...\r\n" );
    enableScheduling();

    // Should never reach here
    return 0;
}
