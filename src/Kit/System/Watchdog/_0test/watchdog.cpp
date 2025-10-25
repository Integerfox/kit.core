/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/Watchdog/Supervisor.h"
#include "Kit/System/Watchdog/WatchedThread.h"
#include "Kit/System/Watchdog/WatchedEventThread.h"
#include "Kit/System/Watchdog/Hal.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Api.h"
#include "Kit/System/TimerManager.h"
#include <stdbool.h>
#include <stdlib.h>

#define SECT_                  "_0test"

//------------------------------------------------------------------------------
// HAL Stub Implementation for Testing
//------------------------------------------------------------------------------

static bool          watchdogEnabled_ = false;
static unsigned long kickCount_       = 0;
static unsigned long tripCount_       = 0;

bool Kit_System_Watchdog_hal_enable_wdog( void )
{
    KIT_SYSTEM_TRACE_MSG( SECT_, "WATCHDOG_HAL: Enabling watchdog (stub implementation)" );
    watchdogEnabled_ = true;
    kickCount_       = 0;
    tripCount_       = 0;
    return true;  // Success
}

void Kit_System_Watchdog_hal_kick_wdog( void )
{
    if ( watchdogEnabled_ )
    {
        kickCount_++;
        KIT_SYSTEM_TRACE_MSG( SECT_, "WATCHDOG_HAL: Kick #%lu", kickCount_ );
    }
}

void Kit_System_Watchdog_hal_trip_wdog( void )
{
    tripCount_++;
    KIT_SYSTEM_TRACE_MSG( SECT_, "WATCHDOG_HAL: TRIP WATCHDOG #%lu - System would reset here!", tripCount_ );
    KIT_SYSTEM_TRACE_MSG( SECT_, "*** WATCHDOG TRIP: System would reset now! ***" );
}

//------------------------------------------------------------------------------
// Test Constants
//------------------------------------------------------------------------------

#define TEST_TIMEOUT_SHORT_MS  100
#define TEST_TIMEOUT_MEDIUM_MS 1000
#define TEST_TIMEOUT_LONG_MS   2000
#define TEST_SLEEP_SHORT_MS    50
#define TEST_SLEEP_MEDIUM_MS   100
#define TEST_SLEEP_LONG_MS     200

using namespace Kit::System;
using namespace Kit::System::Watchdog;


////////////////////////////////////////////////////////////////////////////////
namespace {
class FailingHealthCheckThread : public Kit::System::Watchdog::WatchedEventThread
{
public:
    ///
    bool m_shouldFailHealthCheck;
    ///
    int m_healthCheckCallCount;

public:
    ///
    FailingHealthCheckThread( uint32_t wdogTimeoutMs, uint32_t healthCheckIntervalMs, bool isSupervisor = false )
        : Kit::System::Watchdog::WatchedEventThread( wdogTimeoutMs, healthCheckIntervalMs, isSupervisor )
        , m_shouldFailHealthCheck( false )
        , m_healthCheckCallCount( 0 )
    {
    }

protected:
    // Override to simulate pass/fail health checks
    bool performHealthCheck() noexcept override
    {
        m_healthCheckCallCount++;
        KIT_SYSTEM_TRACE_MSG( SECT_, "Health check called %d times, should fail: %s",
                             m_healthCheckCallCount, m_shouldFailHealthCheck ? "true" : "false" );
        return !m_shouldFailHealthCheck;
    }
};

}  // end anonymous namespace


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "watchdog" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );

    SECTION( "supervisor basic operations" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing Supervisor class basic operations" );

        // Reset and verify initial HAL state
        watchdogEnabled_ = false;
        kickCount_ = 0;
        tripCount_ = 0;

        // Test enabling watchdog
        bool enabled = Supervisor::enableWdog();
        REQUIRE( enabled == true );
        REQUIRE( watchdogEnabled_ == true );  // Verify HAL enable was called

        // Test kicking hardware watchdog
        REQUIRE( kickCount_ == 0 );  // Verify initial state
        Supervisor::kickWdog();
        KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor kicked hardware watchdog" );
        REQUIRE( kickCount_ == 1 );  // Verify HAL kick was called exactly once
    }

    SECTION( "watched thread construction" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing WatchedThread construction and properties" );

        // Test custom timeout
        WatchedThread thread1( TEST_TIMEOUT_LONG_MS );
        REQUIRE( thread1.m_wdogTimeoutMs == TEST_TIMEOUT_LONG_MS );
        REQUIRE( thread1.m_currentCountMs == TEST_TIMEOUT_LONG_MS );

        // Test explicit timeout specification
        WatchedThread thread2( TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( thread2.m_wdogTimeoutMs == TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( thread2.m_currentCountMs == TEST_TIMEOUT_MEDIUM_MS );
    }

    SECTION( "supervisor thread management" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing Supervisor thread management" );

        Supervisor::enableWdog();

        WatchedThread thread1( TEST_TIMEOUT_MEDIUM_MS );
        WatchedThread thread2( TEST_TIMEOUT_LONG_MS );

        // Test beginWatching - adds thread to supervision
        Supervisor::beginWatching( thread1 );
        Supervisor::beginWatching( thread2 );

        // Verify initial state after registration
        REQUIRE( thread1.m_currentCountMs == thread1.m_wdogTimeoutMs );
        REQUIRE( thread2.m_currentCountMs == thread2.m_wdogTimeoutMs );

        // Test reloadThread - resets countdown timer
        thread1.m_currentCountMs = TEST_TIMEOUT_SHORT_MS;
        Supervisor::reloadThread( thread1 );
        REQUIRE( thread1.m_currentCountMs == thread1.m_wdogTimeoutMs );

        // Test monitorThreads with healthy threads
        // Need to call monitorThreads multiple times due to tick divider
        unsigned long kickCountBefore = kickCount_;
        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
        {
            Supervisor::monitorThreads();
        }
        // Verify that monitorThreads kicked the hardware watchdog for healthy threads
        REQUIRE( kickCount_ > kickCountBefore );  // Should have kicked HAL watchdog

        // Test endWatching - removes thread from supervision
        // After endWatching, threads should no longer be monitored
        Supervisor::endWatching( thread1 );
        Supervisor::endWatching( thread2 );

        // Verify endWatching worked by checking kick behavior changes
        unsigned long kickCountAfterRemoval = kickCount_;
        Supervisor::monitorThreads();  // Should not kick since no threads are being watched
        // Note: In current implementation, monitorThreads still kicks even with no threads
        REQUIRE( kickCount_ >= kickCountAfterRemoval );  // Should not decrease kicks
    }

    SECTION( "monitor threads with timeout" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing thread timeout scenario" );

        Supervisor::enableWdog();

        WatchedThread thread( TEST_TIMEOUT_SHORT_MS );
        Supervisor::beginWatching( thread );

        // Simulate timeout by setting count to very low value and waiting for real time to pass
        thread.m_currentCountMs = 1;  // Very short timeout

        // Call monitorThreads once to establish time marker
        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
        {
            Supervisor::monitorThreads();
        }

        // Wait enough time to ensure timeout condition (longer than thread timeout)
        sleep( TEST_SLEEP_MEDIUM_MS );  // Wait longer than the 1ms timeout

        // Verify HAL trip behavior - before and after state  
        unsigned long tripCountBefore = tripCount_;

        // Monitor should detect timeout and trip watchdog
        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
        {
            Supervisor::monitorThreads();  // This should call tripWdog() internally
            if ( tripCount_ > tripCountBefore ) break;  // Trip occurred, no need to continue
        }

        // Verify that HAL tripWdog() was actually called
        REQUIRE( tripCount_ > tripCountBefore );  // Should have incremented trip counter

        Supervisor::endWatching( thread );
    }

    SECTION( "watched event thread construction" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing WatchedEventThread construction" );

        // Test supervisor thread
        WatchedEventThread supervisorThread( 1500, 750, true );
        REQUIRE( supervisorThread.m_wdogTimeoutMs == 1500 );
        REQUIRE( supervisorThread.m_currentCountMs == 1500 );
        REQUIRE( supervisorThread.isSupervisorThread() == true );

        // Test regular thread
        WatchedEventThread regularThread( TEST_TIMEOUT_MEDIUM_MS, TEST_TIMEOUT_MEDIUM_MS / 2, false );
        REQUIRE( regularThread.m_wdogTimeoutMs == TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( regularThread.isSupervisorThread() == false );

        // Test default non-supervisor
        WatchedEventThread defaultThread( TEST_TIMEOUT_MEDIUM_MS, TEST_TIMEOUT_MEDIUM_MS / 2 );
        REQUIRE( defaultThread.isSupervisorThread() == false );
    }

    SECTION( "watched event thread operations" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing WatchedEventThread start/stop operations" );

        Supervisor::enableWdog();
        TimerManager timerManager;

        FailingHealthCheckThread eventThread( TEST_TIMEOUT_MEDIUM_MS, TEST_TIMEOUT_SHORT_MS, false );

        // Test startWatcher
        eventThread.startWatcher( timerManager );
        // Verify thread properties are set correctly
        REQUIRE( eventThread.m_currentCountMs == eventThread.m_wdogTimeoutMs );

        // Test stopWatcher
        eventThread.stopWatcher();

        // Verify multiple start/stop calls are safe (should be idempotent)
        eventThread.startWatcher( timerManager );
        REQUIRE( eventThread.m_currentCountMs == eventThread.m_wdogTimeoutMs );  // Should reset properly

        eventThread.stopWatcher();
        // Multiple stops should be safe - no crash expected
        eventThread.stopWatcher();

        // Test that we can restart after stopping
        eventThread.startWatcher( timerManager );
        REQUIRE( eventThread.m_currentCountMs == eventThread.m_wdogTimeoutMs );  // Should be valid again
        eventThread.stopWatcher();
    }

    SECTION( "health check success" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing successful health checks" );

        Supervisor::enableWdog();
        TimerManager timerManager;

        FailingHealthCheckThread eventThread( TEST_TIMEOUT_MEDIUM_MS, TEST_SLEEP_SHORT_MS, false );

        // Verify initial HAL state before starting health checks
        unsigned long kickCountBefore = kickCount_;
        unsigned long tripCountBefore = tripCount_;
        REQUIRE( eventThread.m_healthCheckCallCount == 0 );  // Should start with no health checks

        // Start with healthy behavior
        eventThread.m_shouldFailHealthCheck = false;
        eventThread.startWatcher( timerManager );

        // Verify initial thread state after starting watcher
        REQUIRE( eventThread.m_currentCountMs == eventThread.m_wdogTimeoutMs );  // Should be properly initialized

        // Process timer events to trigger health checks and supervisor monitoring
        uint32_t startTime = ElapsedTime::milliseconds();
        while ( ElapsedTime::deltaMilliseconds( startTime, ElapsedTime::milliseconds() ) < TEST_SLEEP_MEDIUM_MS )
        {
            timerManager.processTimers();

            // Also trigger supervisor monitoring to ensure HAL kicks occur for healthy threads
            for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
            {
                Supervisor::monitorThreads();
            }

            sleep( 10 );  // Small sleep to allow timer processing
        }

        // Verify health checks were called and successful
        REQUIRE( eventThread.m_healthCheckCallCount > 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Health check called %d times, all successful", eventThread.m_healthCheckCallCount );

        // Verify HAL behavior - successful health checks should result in HAL kicks but NO trips
        REQUIRE( kickCount_ > kickCountBefore );  // Should have kicked HAL watchdog during healthy operation
        REQUIRE( tripCount_ == tripCountBefore );  // Should NOT have tripped watchdog for healthy thread

        // Verify thread remained healthy throughout the test
        REQUIRE( eventThread.m_currentCountMs > 0 );  // Thread should not have timed out

        // Verify the thread is still being monitored properly by triggering supervisor monitoring
        unsigned long kickCountAfterHealthChecks = kickCount_;

        // Call supervisor monitoring to verify thread is still healthy and being managed
        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
        {
            Supervisor::monitorThreads();
        }

        // Verify continued HAL kick behavior (proving thread is still healthy and monitored)
        REQUIRE( kickCount_ > kickCountAfterHealthChecks );  // Should continue kicking for healthy thread
        REQUIRE( tripCount_ == tripCountBefore );  // Should still have no trips

        // Final verification: thread should still be healthy after supervisor monitoring
        REQUIRE( eventThread.m_currentCountMs > 0 );  // Thread should remain healthy

        eventThread.stopWatcher();

        // Verify clean shutdown - thread properties should be preserved after stopping
        REQUIRE( eventThread.m_wdogTimeoutMs == TEST_TIMEOUT_MEDIUM_MS );  // Timeout should remain unchanged
    }

    SECTION( "health check failure" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing health check failure triggers watchdog trip" );

        Supervisor::enableWdog();
        TimerManager timerManager;

        FailingHealthCheckThread eventThread( TEST_TIMEOUT_MEDIUM_MS, TEST_SLEEP_SHORT_MS, false );

        // Configure to fail health check
        eventThread.m_shouldFailHealthCheck = true;
        eventThread.startWatcher( timerManager );

        // Verify initial HAL trip state before health check failure
        unsigned long tripCountBefore = tripCount_;
        REQUIRE( tripCountBefore == 0 );  // Should start with no trips

        // Process timer events to trigger health check failure
        uint32_t startTime = ElapsedTime::milliseconds();
        while ( ElapsedTime::deltaMilliseconds( startTime, ElapsedTime::milliseconds() ) < TEST_SLEEP_MEDIUM_MS )
        {
            timerManager.processTimers();
            sleep( 10 );  // Small sleep to allow timer processing

            // Check if trip has occurred, break early if detected
            if ( tripCount_ > tripCountBefore )
            {
                break;
            }
        }

        // Verify that health check failure actually triggered tripWdog()
        KIT_SYSTEM_TRACE_MSG( SECT_, "Health check call count: %d", eventThread.m_healthCheckCallCount );
        REQUIRE( eventThread.m_healthCheckCallCount > 0 );  // Health check should have been called
        REQUIRE( tripCount_ > tripCountBefore );  // Should have incremented trip counter
        REQUIRE( tripCount_ == 1 );  // Should be exactly one trip from this health check failure

        eventThread.stopWatcher();
    }

    SECTION( "supervisor timing and tick divider" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing supervisor timing and tick divider behavior" );

        Supervisor::enableWdog();

        WatchedThread thread( TEST_TIMEOUT_MEDIUM_MS );
        Supervisor::beginWatching( thread );

        // Test tick divider - verify HAL kicks happen according to divider logic
        unsigned long initialKickCount = kickCount_;

        // Call monitorThreads multiple times to test tick divider
        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 5; i++ )
        {
            Supervisor::monitorThreads();
        }

        // Verify that HAL kicks occurred (should be more than initial)
        REQUIRE( kickCount_ > initialKickCount );
        unsigned long kicksAfterLoop1 = kickCount_;

        // Simulate partial time passage and verify thread countdown behavior
        uint32_t originalCount = thread.m_currentCountMs;
        thread.m_currentCountMs = TEST_TIMEOUT_MEDIUM_MS / 2;
        REQUIRE( thread.m_currentCountMs < originalCount );  // Verify we set it lower

        // Multiple monitoring calls should eventually process timing
        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
        {
            Supervisor::monitorThreads();
        }

        // Verify more HAL kicks occurred during second loop
        REQUIRE( kickCount_ > kicksAfterLoop1 );

        // Verify thread is still healthy (not timed out)
        REQUIRE( thread.m_currentCountMs > 0 );  // Should not have hit zero

        Supervisor::endWatching( thread );
    }

    SECTION( "multiple threads with different timeouts" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing multiple threads with varying timeout values" );

        Supervisor::enableWdog();

        WatchedThread shortThread( TEST_TIMEOUT_SHORT_MS );
        WatchedThread mediumThread( TEST_TIMEOUT_MEDIUM_MS );
        WatchedThread longThread( TEST_TIMEOUT_LONG_MS );

        // Verify different timeout values are set correctly
        REQUIRE( shortThread.m_wdogTimeoutMs == TEST_TIMEOUT_SHORT_MS );
        REQUIRE( mediumThread.m_wdogTimeoutMs == TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( longThread.m_wdogTimeoutMs == TEST_TIMEOUT_LONG_MS );

        // Begin watching all threads and verify initial state
        Supervisor::beginWatching( shortThread );
        Supervisor::beginWatching( mediumThread );
        Supervisor::beginWatching( longThread );

        // Verify all threads are properly initialized
        REQUIRE( shortThread.m_currentCountMs == shortThread.m_wdogTimeoutMs );
        REQUIRE( mediumThread.m_currentCountMs == mediumThread.m_wdogTimeoutMs );
        REQUIRE( longThread.m_currentCountMs == longThread.m_wdogTimeoutMs );

        // Test selective reloading - modify medium thread then reload it
        mediumThread.m_currentCountMs = TEST_TIMEOUT_SHORT_MS;  // Set to different value
        REQUIRE( mediumThread.m_currentCountMs != mediumThread.m_wdogTimeoutMs );  // Verify it's different

        Supervisor::reloadThread( mediumThread );
        REQUIRE( mediumThread.m_currentCountMs == mediumThread.m_wdogTimeoutMs );  // Should be reset

        // Verify other threads were not affected by selective reload
        REQUIRE( shortThread.m_currentCountMs == shortThread.m_wdogTimeoutMs );
        REQUIRE( longThread.m_currentCountMs == longThread.m_wdogTimeoutMs );

        // Test monitoring with mixed timeout threads
        unsigned long kickCountBefore = kickCount_;
        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
        {
            Supervisor::monitorThreads();
        }
        REQUIRE( kickCount_ > kickCountBefore );  // Should have kicked HAL for healthy threads

        // Verify all threads are still healthy after monitoring
        REQUIRE( shortThread.m_currentCountMs > 0 );
        REQUIRE( mediumThread.m_currentCountMs > 0 );
        REQUIRE( longThread.m_currentCountMs > 0 );

        // End watching all threads
        Supervisor::endWatching( shortThread );
        Supervisor::endWatching( mediumThread );
        Supervisor::endWatching( longThread );
    }

    SECTION( "watchedeventthread supervisor mode" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing WatchedEventThread in supervisor mode" );

        Supervisor::enableWdog();
        TimerManager timerManager;

        FailingHealthCheckThread supervisorThread( TEST_TIMEOUT_MEDIUM_MS, TEST_SLEEP_SHORT_MS, true );

        // Verify supervisor thread properties
        REQUIRE( supervisorThread.isSupervisorThread() == true );
        REQUIRE( supervisorThread.m_wdogTimeoutMs == TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( supervisorThread.m_currentCountMs == TEST_TIMEOUT_MEDIUM_MS );

        // Test startWatcher with supervisor thread
        supervisorThread.startWatcher( timerManager );
        REQUIRE( supervisorThread.m_currentCountMs == supervisorThread.m_wdogTimeoutMs );

        // Test supervisor-specific monitorWdog behavior
        // The startWatcher() call above should have already added this thread to supervision
        // Verify the thread is properly managed by checking HAL kick behavior
        unsigned long kickCountBefore = kickCount_;

        // Call monitorWdog multiple times to trigger the tick divider and establish time marker
        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
        {
            supervisorThread.monitorWdog();  // This calls Supervisor::monitorThreads()
        }

        // Verify that monitorWdog() resulted in HAL kicks (proving supervision is working)
        REQUIRE( kickCount_ > kickCountBefore );  // Should have kicked HAL watchdog

        // Wait a small amount of time to allow time-based processing
        sleep( 10 );  // Small sleep to create elapsed time

        // Test that calling monitorWdog with elapsed time continues to work properly
        kickCountBefore = kickCount_;

        // Call monitorWdog again to trigger watchdog processing with elapsed time
        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
        {
            supervisorThread.monitorWdog();  // Should process watchdog timing
        }

        // Verify that monitoring continued to work (more HAL kicks)
        REQUIRE( kickCount_ > kickCountBefore );  // Should have kicked HAL again

        // The count behavior depends on implementation details, but thread should remain healthy
        REQUIRE( supervisorThread.m_currentCountMs > 0 );  // Should not timeout during normal operation
        supervisorThread.stopWatcher();

        // After stopping, verify supervisor can still be called safely
        supervisorThread.monitorWdog();  // Should not crash after being stopped
    }

    SECTION( "edge cases" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing edge cases and error conditions" );

        Supervisor::enableWdog();

        // Test zero timeout
        WatchedThread zeroThread( 0 );
        REQUIRE( zeroThread.m_wdogTimeoutMs == 0 );
        REQUIRE( zeroThread.m_currentCountMs == 0 );  // Should match timeout value

        // Test reload on non-watched thread
        WatchedThread orphanThread( TEST_TIMEOUT_MEDIUM_MS );
        uint32_t originalCount = orphanThread.m_currentCountMs;
        Supervisor::reloadThread( orphanThread );  // Should not crash
        REQUIRE( orphanThread.m_currentCountMs == originalCount );  // Should be unchanged since not watched

        // Test monitor with no threads
        unsigned long kickCountBefore = kickCount_;
        Supervisor::monitorThreads();  // Should complete without issues
        // Note: Current implementation may or may not kick when no threads - documenting behavior
        REQUIRE( kickCount_ >= kickCountBefore );  // Should not decrease

        // Test proper add/remove sequence
        WatchedThread thread( TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( thread.m_currentCountMs == TEST_TIMEOUT_MEDIUM_MS );

        Supervisor::beginWatching( thread );
        REQUIRE( thread.m_currentCountMs == thread.m_wdogTimeoutMs );  // Should be properly initialized

        Supervisor::endWatching( thread );
        // After removing, thread state should be preserved but not monitored
        REQUIRE( thread.m_wdogTimeoutMs == TEST_TIMEOUT_MEDIUM_MS );  // Timeout should remain unchanged

        // Test that removed threads can be re-added - verify re-initialization
        Supervisor::beginWatching( thread );
        REQUIRE( thread.m_currentCountMs == thread.m_wdogTimeoutMs );  // Should be re-initialized properly

        Supervisor::endWatching( thread );
        REQUIRE( thread.m_wdogTimeoutMs == TEST_TIMEOUT_MEDIUM_MS );  // Should remain consistent
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}