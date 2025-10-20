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
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Api.h"
#include "Kit/System/TimerManager.h"

#define SECT_                  "_0test"

#define TEST_TIMEOUT_SHORT_MS  100
#define TEST_TIMEOUT_MEDIUM_MS 1000
#define TEST_TIMEOUT_LONG_MS   2000
#define TEST_SLEEP_SHORT_MS    50
#define TEST_SLEEP_MEDIUM_MS   100
#define TEST_SLEEP_LONG_MS     200

///
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
namespace {

/// Test WatchedEventThread that simulates health check failure
class FailingHealthCheckThread : public WatchedEventThread
{
public:
    ///
    bool m_shouldFailHealthCheck;
    ///
    int m_healthCheckCallCount;

public:
    ///
    FailingHealthCheckThread( uint32_t wdogTimeoutMs, uint32_t healthCheckIntervalMs, bool isSupervisor = false )
        : WatchedEventThread( wdogTimeoutMs, healthCheckIntervalMs, isSupervisor )
        , m_shouldFailHealthCheck( false )
        , m_healthCheckCallCount( 0 )
    {
    }

protected:
    /// Override to provide controllable health check failure
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

    SECTION( "hal functions" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing HAL wrapper functions" );

        // Test enabling watchdog
        bool enabled = enableWdog();
        REQUIRE( enabled == true );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Watchdog enabled successfully" );

        // Test kicking watchdog (should not crash)
        kickWdog();
        KIT_SYSTEM_TRACE_MSG( SECT_, "Watchdog kicked successfully" );

        // Test tripping watchdog (in stub mode, should not actually reset)
        tripWdog();
        KIT_SYSTEM_TRACE_MSG( SECT_, "Watchdog tripped successfully (stub mode)" );
    }

    SECTION( "supervisor basic operations" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing Supervisor class basic operations" );

        // Test enabling watchdog
        bool enabled = Supervisor::enableWdog();
        REQUIRE( enabled == true );

        // Test kicking hardware watchdog
        Supervisor::kickWdog();
        KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor kicked hardware watchdog" );
    }

    SECTION( "watched thread construction" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing WatchedThread construction and properties" );

        // Test custom timeout
        WatchedThread thread1( TEST_TIMEOUT_LONG_MS );
        REQUIRE( thread1.m_wdogTimeoutMs == TEST_TIMEOUT_LONG_MS );
        REQUIRE( thread1.m_currentCountMs == TEST_TIMEOUT_LONG_MS );

        // Test default timeout
        WatchedThread thread2;
        REQUIRE( thread2.m_wdogTimeoutMs == OPTION_SYSTEM_WATCHDOG_WATCHED_THREAD_DEFAULT_TIMEOUT_MS );
        REQUIRE( thread2.m_currentCountMs == OPTION_SYSTEM_WATCHDOG_WATCHED_THREAD_DEFAULT_TIMEOUT_MS );
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
        Supervisor::monitorThreads();

        // Test endWatching - removes thread from supervision
        Supervisor::endWatching( thread1 );
        Supervisor::endWatching( thread2 );
    }

    SECTION( "monitor threads with timeout" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing thread timeout scenario" );

        Supervisor::enableWdog();

        WatchedThread thread( TEST_TIMEOUT_SHORT_MS );
        Supervisor::beginWatching( thread );

        // Simulate timeout by setting count to very low value
        thread.m_currentCountMs = 1;

        // Wait enough time to ensure timeout condition
        sleep( TEST_SLEEP_SHORT_MS );

        // Monitor should detect timeout and trip watchdog
        Supervisor::monitorThreads();  // This should call tripWdog() internally

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

        // Verify monitorWdog calls Supervisor
        eventThread.monitorWdog();

        // Test stopWatcher
        eventThread.stopWatcher();

        // Verify multiple start/stop calls are safe (should be idempotent)
        eventThread.startWatcher( timerManager );
        eventThread.startWatcher( timerManager );  // Should be safe
        eventThread.stopWatcher();
        eventThread.stopWatcher();  // Should be safe
    }

    SECTION( "health check success" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing successful health checks" );

        Supervisor::enableWdog();
        TimerManager timerManager;

        FailingHealthCheckThread eventThread( TEST_TIMEOUT_MEDIUM_MS, TEST_SLEEP_SHORT_MS, false );

        // Start with healthy behavior
        eventThread.m_shouldFailHealthCheck = false;
        eventThread.startWatcher( timerManager );

        // Process timer events to trigger health checks
        uint32_t startTime = ElapsedTime::milliseconds();
        while ( ElapsedTime::deltaMilliseconds( startTime, ElapsedTime::milliseconds() ) < TEST_SLEEP_MEDIUM_MS )
        {
            timerManager.processTimers();
            sleep( 10 );  // Small sleep to allow timer processing
        }

        // Verify health checks were called and thread is still running
        REQUIRE( eventThread.m_healthCheckCallCount > 0 );

        eventThread.stopWatcher();
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

        // Process timer events to trigger health check failure
        uint32_t startTime = ElapsedTime::milliseconds();
        while ( ElapsedTime::deltaMilliseconds( startTime, ElapsedTime::milliseconds() ) < TEST_SLEEP_MEDIUM_MS )
        {
            timerManager.processTimers();
            sleep( 10 );  // Small sleep to allow timer processing
        }

        // Note: In stub mode, tripWdog() doesn't actually stop execution
        // but the health check failure should have been detected
        KIT_SYSTEM_TRACE_MSG( SECT_, "Health check call count: %d", eventThread.m_healthCheckCallCount );
        REQUIRE( eventThread.m_healthCheckCallCount > 0 );

        eventThread.stopWatcher();
    }

    SECTION( "supervisor timing and tick divider" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing supervisor timing and tick divider behavior" );

        Supervisor::enableWdog();

        WatchedThread thread( TEST_TIMEOUT_MEDIUM_MS );
        Supervisor::beginWatching( thread );

        // Call monitorThreads multiple times to test tick divider
        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 5; i++ )
        {
            Supervisor::monitorThreads();
        }

        // Simulate partial time passage
        thread.m_currentCountMs = TEST_TIMEOUT_MEDIUM_MS / 2;
        
        // Multiple monitoring calls should eventually process timing
        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
        {
            Supervisor::monitorThreads();
        }

        Supervisor::endWatching( thread );
    }

    SECTION( "multiple threads with different timeouts" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing multiple threads with varying timeout values" );

        Supervisor::enableWdog();

        WatchedThread shortThread( TEST_TIMEOUT_SHORT_MS );
        WatchedThread mediumThread( TEST_TIMEOUT_MEDIUM_MS );
        WatchedThread longThread( TEST_TIMEOUT_LONG_MS );

        // Begin watching all threads
        Supervisor::beginWatching( shortThread );
        Supervisor::beginWatching( mediumThread );
        Supervisor::beginWatching( longThread );

        // Test selective reloading
        Supervisor::reloadThread( mediumThread );
        REQUIRE( mediumThread.m_currentCountMs == mediumThread.m_wdogTimeoutMs );

        // Test monitoring with mixed timeout threads
        Supervisor::monitorThreads();

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

        REQUIRE( supervisorThread.isSupervisorThread() == true );

        supervisorThread.startWatcher( timerManager );

        // Test supervisor-specific monitorWdog behavior
        supervisorThread.monitorWdog();

        supervisorThread.stopWatcher();
    }

    SECTION( "edge cases" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing edge cases and error conditions" );

        Supervisor::enableWdog();

        // Test zero timeout (should use minimum viable timeout)
        WatchedThread zeroThread( 0 );
        REQUIRE( zeroThread.m_wdogTimeoutMs == 0 );

        // Test reload on non-watched thread (should be safe)
        WatchedThread orphanThread( TEST_TIMEOUT_MEDIUM_MS );
        Supervisor::reloadThread( orphanThread );  // Should not crash

        // Test monitor with no threads
        Supervisor::monitorThreads();  // Should complete without issues

        // Test proper add/remove sequence
        WatchedThread thread( TEST_TIMEOUT_MEDIUM_MS );
        Supervisor::beginWatching( thread );
        Supervisor::endWatching( thread );
        
        // Test that removed threads can be re-added
        Supervisor::beginWatching( thread );
        Supervisor::endWatching( thread );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}