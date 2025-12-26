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
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/Watchdog/Supervisor.h"
#include "Kit/System/Watchdog/WatchedEventThread.h"
#include "Kit/System/Watchdog/WatchedRawThread.h"
#include "Kit/System/EventLoop.h"
#include "Kit/EventQueue/Server.h"
#include <cstdint>

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

//------------------------------------------------------------------------------
// Static Objects
//------------------------------------------------------------------------------

/// Raw thread runnable instance
static RawThreadRunnable rawThreadRunnable_;

/// Test monitor instance
static TestMonitor testMonitor_;

/// Supervisor watchdog configuration
static SupervisorWatchdog supervisorThreadWDogConfig_( 
    SUPERVISOR_WDOG_TIMEOUT_MS, 
    SUPERVISOR_HEALTH_CHECK_MS, 
    true );

/// Supervisor event loop
static Server supervisorEventLoop_( 
    OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD, 
    nullptr, 
    &supervisorThreadWDogConfig_ );

//------------------------------------------------------------------------------
// Core Test Function
//------------------------------------------------------------------------------

/** Run the watchdog enable test.
    This function is platform independent and can be called from platform-specific
    main() implementations.
    @return True if test setup succeeded, false otherwise
 */
bool runTests()
{
    KIT_SYSTEM_TRACE_MSG( SECT_, "\r\n\r\n**** WATCHDOG ENABLE TEST START ****\r\n" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Watchdog Enable Test Starting\r\n" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "HW Watchdog Timeout: %u ms\r\n", HW_WATCHDOG_TIMEOUT_MS );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Test Duration: %u ms\r\n", TEST_DURATION_MS );

    // Create and start the supervisor thread
    auto* supervisorThread = Thread::create( supervisorEventLoop_, "SUPERVISOR" );
    if ( !supervisorThread )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "FAILED to create supervisor thread\r\n" );
        FatalError::logf( Shutdown::eFAILURE, "Failed to create supervisor thread" );
        return false;
    }

    KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor thread created\r\n" );

    // Create and start the raw thread
    auto* rawThread = Thread::create( rawThreadRunnable_, "RAW_THREAD" );
    if ( !rawThread )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "FAILED to create raw thread\r\n" );
        FatalError::logf( Shutdown::eFAILURE, "Failed to create raw thread" );
        return false;
    }

    KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread created\r\n" );

    // Create and start the test monitor thread
    auto* monitorThread = Thread::create( testMonitor_, "TEST_MONITOR" );
    if ( !monitorThread )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "FAILED to create test monitor thread\r\n" );
        FatalError::logf( Shutdown::eFAILURE, "Failed to create test monitor thread" );
        return false;
    }
    KIT_SYSTEM_TRACE_MSG( SECT_, "Test monitor thread created\r\n" );

    KIT_SYSTEM_TRACE_MSG( SECT_, "Starting scheduler...\r\n" );
    enableScheduling();

    // Should never reach here
    return true;
}
