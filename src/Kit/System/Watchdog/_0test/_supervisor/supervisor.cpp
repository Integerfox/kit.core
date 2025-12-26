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

/// Counter to track elapsed time in the event loop
static volatile uint32_t eventLoopCounter_ = 0;

/// Counter to track when to trigger the stuck condition
static volatile uint32_t runCounter_ = 0;

/// Flag to indicate the stuck condition should be triggered
static volatile bool triggerStuck_ = false;

//------------------------------------------------------------------------------
// Custom Event Loop with Stuck Simulation
//------------------------------------------------------------------------------

class TestEventLoop : public Server
{
public:
    /** Constructor
        @param timeoutPeriod Event loop timeout period
        @param wdogSetup Pointer to watchdog setup interface
     */
    TestEventLoop( unsigned long timeoutPeriod,
                   IWatchedEventLoop* wdogSetup ) noexcept
        : Server( timeoutPeriod
                , nullptr
                , wdogSetup )
    {
    }

protected:
    /// Override entry to inject stuck condition in the event loop thread
    void entry() noexcept override
    {
        startEventLoop();

        // Run the event loop until told to get stuck
        bool run = true;
        while ( run && !triggerStuck_ )
        {
            run = waitAndProcessEvents();
            eventLoopCounter_++;
        }

        // Check if we should trigger the stuck condition
        if ( triggerStuck_ )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "*** SUPERVISOR STUCK - Entering busy loop ***\r\n" );
            KIT_SYSTEM_TRACE_MSG( SECT_, "Busy loop will last %u ms (HW timeout is %u ms)\r\n",
                                  (unsigned)STUCK_DURATION_MS, (unsigned)HW_WATCHDOG_TIMEOUT_MS );

            // Get stuck in a busy loop (no watchdog refresh)
            uint32_t startTime = ElapsedTime::milliseconds();
            while ( !ElapsedTime::expiredMilliseconds( startTime, STUCK_DURATION_MS ) )
            {
                // Busy wait - watchdog will NOT be refreshed
                Bsp_toggle_debug2();
                // Small delay to make LED visible
                sleep( 50 );
            }

            // If we get here, the watchdog failed to reset us
            KIT_SYSTEM_TRACE_MSG( SECT_, "*** ERROR: Watchdog did not reset system! ***\r\n" );
            triggerStuck_ = false;
        }

        // Continue normal event loop until stopped
        while ( run )
        {
            run = waitAndProcessEvents();
        }

        stopEventLoop();
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
        // first health check timer fire
        sleep( 1000 );

        if ( !Supervisor::enableWdog() )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "FAILED to enable hardware watchdog!\r\n" );
            FatalError::logf( Shutdown::eFAILURE, "Failed to enable hardware watchdog\r\n" );
        }

        KIT_SYSTEM_TRACE_MSG( SECT_, "Hardware watchdog enabled successfully\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Running normally for %u ms...\r\n", NORMAL_RUN_DELAY_MS );
        // Run normally for a bit to establish that watchdog is working
        while ( runCounter_ < NORMAL_RUN_DELAY_MS )
        {
            Bsp_toggle_debug1();
            sleep( 500 );
            runCounter_ += 500;
        }

        // Now trigger the stuck condition
        KIT_SYSTEM_TRACE_MSG( SECT_, "Triggering stuck condition...\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Waiting for watchdog reset...\r\n" );

        triggerStuck_ = true;

        // Wait for the watchdog to reset us
        for ( int32_t i = 0; i < 10; ++i )
        {
            sleep( 500 );
            KIT_SYSTEM_TRACE_MSG( SECT_, "Still waiting for reset... (%d)\r\n", (int)i );
        }

        // If we get here, something went wrong
        KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Test FAILED\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Watchdog did not reset the system\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================\r\n" );

        // Blink LED rapidly to indicate failure
        for ( ;; )
        {
            Bsp_toggle_debug1();
            sleep( 100 );
        }
    }
};

//------------------------------------------------------------------------------
// Static Objects
//------------------------------------------------------------------------------

/// Test monitor instance
static TestMonitor testMonitor_;

/// Supervisor watchdog configuration
static WatchedEventThread supervisorThreadWDogConfig_( 
    SUPERVISOR_WDOG_TIMEOUT_MS, 
    SUPERVISOR_HEALTH_CHECK_MS, 
    true );

/// Supervisor event loop (custom with stuck simulation)
static TestEventLoop supervisorEventLoop_( 
    OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD, 
    &supervisorThreadWDogConfig_ );

//------------------------------------------------------------------------------
// Core Test Function
//------------------------------------------------------------------------------

/** Run the supervisor stuck test.
    This function is platform independent and can be called from platform-specific
    main() implementations.
    @return True if test setup succeeded, false otherwise
 */
bool runTests()
{
    KIT_SYSTEM_TRACE_MSG( SECT_, "\r\n\r\n**** SUPERVISOR STUCK TEST START ****\r\n" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor Stuck Test Starting\r\n" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "HW Watchdog Timeout: %u ms\r\n", HW_WATCHDOG_TIMEOUT_MS );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Stuck Duration: %u ms\r\n", STUCK_DURATION_MS );
    KIT_SYSTEM_TRACE_MSG( SECT_, "First run - setting up watchdog test\r\n" );

    // Create and start the supervisor thread
    auto* supervisorThread = Thread::create( supervisorEventLoop_, "SUPERVISOR" );
    if ( !supervisorThread )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "FAILED to create supervisor thread\r\n" );
        FatalError::logf( Shutdown::eFAILURE, "Failed to create supervisor thread" );
        return false;
    }
    KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor thread created\r\n" );

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
