/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file 

    Hardware test to demonstrate supervisor thread failure detection.
    
    This test verifies that when the supervisor (event thread) gets stuck
    in a busy loop and exceeds the hardware watchdog timeout period, the
    MCU watchdog will reset the system.
    
    Expected behavior:
    - First run: System starts normally, then supervisor gets stuck after delay
    - Watchdog expires and resets the MCU
    - Second run: Detects watchdog reset flag and indicates test passed

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
    The supervisor will get stuck for longer than this period.
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

/** Time to run normally before triggering the stuck condition.
    This gives the watchdog time to initialize and start running properly.
*/
#define NORMAL_RUN_DELAY_MS        2000

/** How long the supervisor will be stuck.
    This must exceed the HW watchdog timeout to trigger a reset.
*/
#define STUCK_DURATION_MS          ( HW_WATCHDOG_TIMEOUT_MS + 500 )

//------------------------------------------------------------------------------
// Test State
//------------------------------------------------------------------------------

/// Counter to track when to trigger the stuck condition
static volatile uint32_t runCounter_ = 0;

/// Flag to indicate the stuck condition should be triggered
static volatile bool triggerStuck_ = false;

//------------------------------------------------------------------------------
// Custom Watcher for Stuck Simulation
//------------------------------------------------------------------------------

/** Custom watcher that fails health checks when triggerStuck_ is set.
    This causes the hardware watchdog to expire and reset the system.
 */
class TestWatcher : public WatchedEventThread
{
public:
    /** Constructor
        @param wdogTimeoutMs Software watchdog timeout in milliseconds
        @param healthCheckIntervalMs Health check interval in milliseconds
        @param isSupervisor True if this is the supervisor thread
     */
    TestWatcher( uint32_t wdogTimeoutMs, uint32_t healthCheckIntervalMs, bool isSupervisor ) noexcept
        : WatchedEventThread( wdogTimeoutMs, healthCheckIntervalMs, isSupervisor )
    {
    }

protected:
    /** Override to implement custom health check logic.
        When triggerStuck_ is true, this returns false to simulate a stuck thread,
        which prevents the hardware watchdog from being refreshed.
     */
    bool performHealthCheck() noexcept override
    {
        if ( triggerStuck_ )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "*** Health check FAILED - Simulating stuck condition ***" );
        }
        return !triggerStuck_;
    }
};

//------------------------------------------------------------------------------
// Static Objects
//------------------------------------------------------------------------------

/// Supervisor watchdog configuration (custom watcher)
static TestWatcher supervisorThreadWDogConfig_( 
    SUPERVISOR_WDOG_TIMEOUT_MS, 
    SUPERVISOR_HEALTH_CHECK_MS, 
    true );

/// Supervisor event loop (standard EventQueue::Server)
static Server supervisorEventLoop_( 
    OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD, 
    nullptr,
    &supervisorThreadWDogConfig_ );

//------------------------------------------------------------------------------
// Main Test Runnable (Runs in Raw Main Thread)
//------------------------------------------------------------------------------

/** Main test coordinator that runs in an initial raw thread (not watched).
    This approach allows trace output before the scheduler starts, avoiding
    the need for busy-wait serial driver logic.
 */
class MainTestRunnable : public IRunnable
{
private:
    bool m_wasWatchdogReset;

public:
    MainTestRunnable( bool wasWatchdogReset ) noexcept
        : m_wasWatchdogReset( wasWatchdogReset )
    {
    }

    void entry() noexcept override
    {
        // Check if this was a watchdog reset (expected for this test)
        if ( m_wasWatchdogReset )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "========================================" );
            KIT_SYSTEM_TRACE_MSG( SECT_, "*** WATCHDOG RESET DETECTED ***" );
            KIT_SYSTEM_TRACE_MSG( SECT_, "========================================" );
            KIT_SYSTEM_TRACE_MSG( SECT_, "Test PASSED - System was reset by watchdog" );
            KIT_SYSTEM_TRACE_MSG( SECT_, "========================================" );

            // Keep both LEDs on to indicate success
            Bsp_turn_on_debug1();
            Bsp_turn_on_debug2();

            // Test passed - idle forever
            for ( ;; )
            {
                sleep( 1000 );
            }
        }

        KIT_SYSTEM_TRACE_MSG( SECT_, "========================================" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "       SUPERVISOR STUCK TEST START" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "       HW Watchdog Timeout: %u ms", HW_WATCHDOG_TIMEOUT_MS );
        KIT_SYSTEM_TRACE_MSG( SECT_, "       Stuck Duration: %u ms", STUCK_DURATION_MS );
        KIT_SYSTEM_TRACE_MSG( SECT_, "========================================" );

        // Create and start the supervisor thread
        auto* supervisorThread = Thread::create( supervisorEventLoop_, "SUPERVISOR" );
        if ( !supervisorThread )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "FAILED to create supervisor thread" );
            FatalError::logf( Shutdown::eFAILURE, "Failed to create supervisor thread" );
            return;
        }
        KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor thread created" );

        // Give supervisor thread time to start, initialize, and have its
        // first health check timer fire
        KIT_SYSTEM_TRACE_MSG( SECT_, "Waiting for supervisor to initialize..." );
        sleep( 1000 );

        // Enable the hardware watchdog
        if ( !Supervisor::enableWdog() )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "FAILED to enable hardware watchdog!" );
            FatalError::logf( Shutdown::eFAILURE, "Failed to enable hardware watchdog" );
        }

        KIT_SYSTEM_TRACE_MSG( SECT_, "Hardware watchdog enabled successfully" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Running normally for %u ms...", NORMAL_RUN_DELAY_MS );
        
        // Run normally for a bit to establish that watchdog is working
        while ( runCounter_ < NORMAL_RUN_DELAY_MS )
        {
            Bsp_toggle_debug1();
            sleep( 500 );
            runCounter_ += 500;
        }

        // Now trigger the stuck condition by setting the flag
        // The TestWatcher will fail health checks, preventing watchdog refresh
        KIT_SYSTEM_TRACE_MSG( SECT_, "Triggering stuck condition..." );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Waiting for watchdog reset..." );
        triggerStuck_ = true;

        // Wait for the watchdog to reset us
        for ( int32_t i = 0; i < 10; ++i )
        {
            sleep( 500 );
            KIT_SYSTEM_TRACE_MSG( SECT_, "Still waiting for reset... (%d)", (int)i );
        }

        // If we get here, something went wrong
        KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "       Test FAILED" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "       Watchdog did not reset the system" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );

        // Blink LED rapidly to indicate failure
        for ( ;; )
        {
            Bsp_toggle_debug1();
            sleep( 100 );
        }
    }
};

//------------------------------------------------------------------------------
// Core Test Function
//------------------------------------------------------------------------------

/** Run the supervisor stuck test.
    This function is platform independent and can be called from platform-specific
    main() implementations.
    
    This creates an initial raw main thread that is NOT watched by the supervisor.
    This allows trace output to work before the scheduler starts, without requiring
    busy-wait serial driver logic.
    
    @param wasWatchdogReset Flag indicating if the system was reset by watchdog
    @return True if test setup succeeded, false otherwise
 */
bool runTests( bool wasWatchdogReset )
{
    // Create main test runnable with watchdog reset status
    static MainTestRunnable mainTestRunnable_( wasWatchdogReset );

    // Create an initial raw main thread (not watched) to coordinate test setup
    auto* mainThread = Thread::create( mainTestRunnable_, "MAIN" );
    if ( !mainThread )
    {
        // Can't use trace here since scheduler isn't started yet
        FatalError::logf( Shutdown::eFAILURE, "Failed to create main thread" );
        return false;
    }

    // Start the scheduler - control transfers to mainTestRunnable_.entry()
    enableScheduling();

    // Should never reach here
    return true;
}
