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

/// Flag to trigger the stuck condition
static volatile bool triggerRawThreadStuck_ = false;

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
            sleep( 100 );
        }

        // Now get stuck in a busy loop WITHOUT kicking the watchdog
        KIT_SYSTEM_TRACE_MSG( SECT_, "*** RAW THREAD STUCK - Entering busy loop ***" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Busy loop will last %u ms (thread timeout is %u ms)",
                              STUCK_DURATION_MS, RAW_THREAD_WDOG_TIMEOUT_MS );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor should detect timeout and trip watchdog" );

        uint32_t startTime = ElapsedTime::milliseconds();
        while ( !ElapsedTime::expiredMilliseconds( startTime, STUCK_DURATION_MS ) )
        {
            // Busy wait - watchdog will NOT be kicked
            Bsp_toggle_debug2();
            // Small delay to make LED visible
            sleep( 50 );
        }

        // If we get here, the watchdog failed to reset us
        KIT_SYSTEM_TRACE_MSG( SECT_, "*** ERROR: Watchdog did not reset system! ***" );

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
// Static Objects
//------------------------------------------------------------------------------

/// Raw thread runnable instance
static RawThreadRunnable rawThreadRunnable_;

/// Supervisor watchdog configuration
static WatchedEventThread supervisorThreadWDogConfig_( 
    SUPERVISOR_WDOG_TIMEOUT_MS, 
    SUPERVISOR_HEALTH_CHECK_MS, 
    true );

/// Supervisor event loop
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
        KIT_SYSTEM_TRACE_MSG( SECT_, "       RAW THREAD STUCK TEST START" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "       HW Watchdog Timeout: %u ms", HW_WATCHDOG_TIMEOUT_MS );
        KIT_SYSTEM_TRACE_MSG( SECT_, "       Raw Thread Timeout: %u ms", RAW_THREAD_WDOG_TIMEOUT_MS );
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

        // Create and start the raw thread
        auto* rawThread = Thread::create( rawThreadRunnable_, "RAW_THREAD" );
        if ( !rawThread )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "FAILED to create raw thread" );
            FatalError::logf( Shutdown::eFAILURE, "Failed to create raw thread" );
            return;
        }
        KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread created" );

        // Give supervisor thread time to start, initialize, and have its
        // first health check timer fire
        sleep( 1000 );
        if ( !Supervisor::enableWdog() )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "FAILED to enable hardware watchdog!" );
            FatalError::logf( Shutdown::eFAILURE, "Failed to enable hardware watchdog" );
        }
        // Wait a bit before triggering the stuck condition
        KIT_SYSTEM_TRACE_MSG( SECT_, "Main thread sleeping %u ms before triggering stuck condition...",
                              NORMAL_RUN_DELAY_MS );
        sleep( NORMAL_RUN_DELAY_MS );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Main thread triggering raw thread stuck condition" );
        triggerRawThreadStuck_ = true;

        // Wait for the watchdog to reset us
        for ( int32_t i = 0; i < 20; ++i )
        {
            sleep( 500 );
            Bsp_toggle_debug1();
            KIT_SYSTEM_TRACE_MSG( SECT_, "Still waiting for reset... (%d)", (int)i );
        }

        // If we get here, something went wrong
        KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Test FAILED" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Watchdog did not reset the system" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread timeout was not detected" );
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

/** Run the raw thread stuck test.
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
