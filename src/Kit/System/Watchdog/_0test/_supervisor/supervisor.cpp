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
// Custom Event Loop with Stuck Simulation
//------------------------------------------------------------------------------

class TestEventLoop : public Server
{
public:
    TestEventLoop( unsigned long timeoutPeriod,
                   IWatchedEventLoop* wdogSetup )
        : Server( timeoutPeriod, nullptr, wdogSetup )
    {
    }

protected:
    /// Override entry to inject stuck condition in the event loop thread
    void entry() noexcept override
    {
        startEventLoop();

        // Run the event loop for NORMAL_RUN_DELAY_MS
        uint32_t startTime = HAL_GetTick();
        bool run = true;
        while ( run && ( ( HAL_GetTick() - startTime ) < NORMAL_RUN_DELAY_MS ) )
        {
            run = waitAndProcessEvents();
        }

        // Check if we should trigger the stuck condition
        if ( triggerStuck_ )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "*** SUPERVISOR STUCK - Entering busy loop ***" );
            KIT_SYSTEM_TRACE_MSG( SECT_, "Busy loop will last %u ms (HW timeout is %u ms)",
                                  (unsigned)STUCK_DURATION_MS, (unsigned)HW_WATCHDOG_TIMEOUT_MS );

            // Get stuck in a busy loop (no watchdog refresh)
            startTime = HAL_GetTick();
            while ( ( HAL_GetTick() - startTime ) < STUCK_DURATION_MS )
            {
                // Busy wait - watchdog will NOT be refreshed
                Bsp_toggle_debug2();
                // Small delay to make LED visible
                for ( volatile uint32_t i = 0; i < 100000; i++ )
                {
                    __NOP();
                }
            }

            // If we get here, the watchdog failed to reset us
            KIT_SYSTEM_TRACE_MSG( SECT_, "*** ERROR: Watchdog did not reset system! ***" );
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
// Main Application
//------------------------------------------------------------------------------

int main( void )
{
    // Initialize the board (HAL, clocks, GPIOs, UART)
    Bsp_initialize();

    // Initialize the KIT system
    Kit::System::initialize();

    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eVERBOSE );

    KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor Stuck Test" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "HW Watchdog Timeout: %u ms", HW_WATCHDOG_TIMEOUT_MS );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Stuck Duration: %u ms", STUCK_DURATION_MS );
    KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );

    // Check if this is a watchdog reset
    if ( __HAL_RCC_GET_FLAG( RCC_FLAG_IWDGRST ) != RESET )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "*** WATCHDOG RESET DETECTED ***" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Test PASSED" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "System was correctly reset by watchdog" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "when supervisor thread was stuck" );
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

    // Create the custom event loop for the supervisor thread
    TestEventLoop eventLoop( OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
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

    // Run normally for a bit to establish that watchdog is working
    KIT_SYSTEM_TRACE_MSG( SECT_, "Running normally for %u ms...", NORMAL_RUN_DELAY_MS );

    while ( runCounter_ < NORMAL_RUN_DELAY_MS )
    {
        Bsp_toggle_debug1();
        HAL_Delay( 500 );
        runCounter_ += 500;
    }

    // Now trigger the stuck condition
    KIT_SYSTEM_TRACE_MSG( SECT_, "Triggering stuck condition..." );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Waiting for watchdog reset..." );

    triggerStuck_ = true;

    // Wait a bit more - the supervisor thread will get stuck in its event loop
    // and the watchdog should reset us
    for ( int i = 0; i < 10; i++ )
    {
        HAL_Delay( 500 );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Still waiting for reset... (%d)", i );
    }

    // If we get here, something went wrong
    KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Test FAILED" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Watchdog did not reset the system" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================" );

    // Blink LED rapidly to indicate failure
    while ( 1 )
    {
        Bsp_toggle_debug1();
        HAL_Delay( 100 );
    }

    return 0;
}
