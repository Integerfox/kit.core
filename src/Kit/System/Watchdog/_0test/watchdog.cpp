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
#include "Kit/System/Watchdog/RawThread.h"
#include "Kit/System/Watchdog/Hal.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Api.h"
#include "Kit/System/TimerManager.h"
#include <stdbool.h>
#include <stdlib.h>

#define SECT_ "_0test"

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
    FailingHealthCheckThread( uint32_t wdogTimeoutMs, uint32_t healthCheckIntervalMs, bool isSupervisor = false )
        : Kit::System::Watchdog::WatchedEventThread( wdogTimeoutMs, healthCheckIntervalMs, isSupervisor )
        , m_shouldFailHealthCheck( false )
        , m_healthCheckCallCount( 0 )
    {
    }


protected:
    bool performHealthCheck() noexcept override
    {
        m_healthCheckCallCount++;
        KIT_SYSTEM_TRACE_MSG( SECT_, "Health check called %d times, should fail: %s", m_healthCheckCallCount, m_shouldFailHealthCheck ? "true" : "false" );
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

        watchdogEnabled_ = false;
        kickCount_       = 0;
        tripCount_       = 0;

        bool enabled = Supervisor::enableWdog();
        REQUIRE( enabled == true );
        REQUIRE( watchdogEnabled_ == true );

        unsigned long tripCountBefore = tripCount_;
        Supervisor::tripWdog();
        KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor tripped hardware watchdog via API" );
        REQUIRE( tripCount_ == tripCountBefore + 1 );
    }

    SECTION( "watched thread construction" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing WatchedThread construction and properties" );

        WatchedThread thread1( TEST_TIMEOUT_LONG_MS );
        REQUIRE( thread1.m_wdogTimeoutMs == TEST_TIMEOUT_LONG_MS );
        REQUIRE( thread1.m_currentCountMs == TEST_TIMEOUT_LONG_MS );

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

        // Verify not in list initially
        REQUIRE( thread1.m_inListPtr_ == nullptr );
        REQUIRE( thread2.m_inListPtr_ == nullptr );

        // Begin watching and verify they are added to supervisor's list
        thread1.m_currentCountMs = thread1.m_wdogTimeoutMs - 1;
        thread2.m_currentCountMs = thread2.m_wdogTimeoutMs - 1;
        Supervisor::beginWatching( thread1 );
        Supervisor::beginWatching( thread2 );

        REQUIRE( thread1.m_currentCountMs == thread1.m_wdogTimeoutMs );
        REQUIRE( thread2.m_currentCountMs == thread2.m_wdogTimeoutMs );
        REQUIRE( thread1.m_inListPtr_ != nullptr );
        REQUIRE( thread2.m_inListPtr_ != nullptr );

        thread1.m_currentCountMs = TEST_TIMEOUT_SHORT_MS;
        Supervisor::reloadThread( thread1 );
        REQUIRE( thread1.m_currentCountMs == thread1.m_wdogTimeoutMs );

        unsigned long kickCountBefore = kickCount_;
        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
        {
            sleep( 10 );
            Supervisor::monitorThreads();
        }
        REQUIRE( kickCount_ > kickCountBefore );

        // End watching and verify they are removed from supervisor's list
        Supervisor::endWatching( thread1 );
        Supervisor::endWatching( thread2 );
        REQUIRE( thread1.m_inListPtr_ == nullptr );
        REQUIRE( thread2.m_inListPtr_ == nullptr );

        unsigned long kickCountAfterRemoval = kickCount_;
        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
        {
            sleep( 10 );
            Supervisor::monitorThreads();
        }
        REQUIRE( kickCount_ > kickCountAfterRemoval );
    }

    SECTION( "monitor threads with timeout" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing thread timeout scenario" );

        Supervisor::enableWdog();

        WatchedThread thread( TEST_TIMEOUT_SHORT_MS );

        // Verify not in list initially
        REQUIRE( thread.m_inListPtr_ == nullptr );

        // Begin watching and verify it's added to supervisor's list
        thread.m_currentCountMs = thread.m_wdogTimeoutMs - 1;
        Supervisor::beginWatching( thread );
        REQUIRE( thread.m_inListPtr_ != nullptr );
        REQUIRE( thread.m_currentCountMs == thread.m_wdogTimeoutMs );

        thread.m_currentCountMs = 1;

        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
        {
            sleep( 10 );
            Supervisor::monitorThreads();
        }

        sleep( TEST_SLEEP_MEDIUM_MS );

        unsigned long tripCountBefore = tripCount_;

        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
        {
            sleep( 10 );
            Supervisor::monitorThreads();
            if ( tripCount_ > tripCountBefore )
                break;
        }

        REQUIRE( tripCount_ > tripCountBefore );

        // End watching and verify it's removed from supervisor's list
        Supervisor::endWatching( thread );
        REQUIRE( thread.m_inListPtr_ == nullptr );
    }

    SECTION( "watched event thread construction" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing WatchedEventThread construction" );

        WatchedEventThread supervisorThread( 1500, 750, true );
        REQUIRE( supervisorThread.m_wdogTimeoutMs == 1500 );
        REQUIRE( supervisorThread.m_currentCountMs == 1500 );
        REQUIRE( supervisorThread.isSupervisorThread() == true );

        WatchedEventThread regularThread( TEST_TIMEOUT_MEDIUM_MS, TEST_TIMEOUT_MEDIUM_MS / 2, false );
        REQUIRE( regularThread.m_wdogTimeoutMs == TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( regularThread.isSupervisorThread() == false );

        WatchedEventThread defaultThread( TEST_TIMEOUT_MEDIUM_MS, TEST_TIMEOUT_MEDIUM_MS / 2 );
        REQUIRE( defaultThread.isSupervisorThread() == false );
    }

    SECTION( "watched event thread operations" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing WatchedEventThread start/stop operations" );

        Supervisor::enableWdog();
        TimerManager timerManager;

        FailingHealthCheckThread eventThread( TEST_TIMEOUT_MEDIUM_MS, TEST_TIMEOUT_SHORT_MS, false );

        eventThread.startWatcher( timerManager );
        REQUIRE( eventThread.m_currentCountMs == eventThread.m_wdogTimeoutMs );

        eventThread.stopWatcher();

        eventThread.startWatcher( timerManager );
        REQUIRE( eventThread.m_currentCountMs == eventThread.m_wdogTimeoutMs );

        eventThread.stopWatcher();
        eventThread.stopWatcher();

        eventThread.startWatcher( timerManager );
        REQUIRE( eventThread.m_currentCountMs == eventThread.m_wdogTimeoutMs );
        eventThread.stopWatcher();
    }

    SECTION( "health check success" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing successful health checks" );

        Supervisor::enableWdog();
        TimerManager timerManager;

        FailingHealthCheckThread eventThread( TEST_TIMEOUT_MEDIUM_MS, TEST_SLEEP_SHORT_MS, false );

        unsigned long kickCountBefore = kickCount_;
        unsigned long tripCountBefore = tripCount_;
        REQUIRE( eventThread.m_healthCheckCallCount == 0 );

        eventThread.m_shouldFailHealthCheck = false;
        eventThread.startWatcher( timerManager );

        REQUIRE( eventThread.m_currentCountMs == eventThread.m_wdogTimeoutMs );

        uint32_t startTime = ElapsedTime::milliseconds();
        while ( ElapsedTime::deltaMilliseconds( startTime, ElapsedTime::milliseconds() ) < TEST_TIMEOUT_MEDIUM_MS )
        {

            for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER; i++ )
            {
                sleep( 10 );
                timerManager.processTimers();
                Supervisor::monitorThreads();
            }
        }

        REQUIRE( eventThread.m_healthCheckCallCount > 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Health check called %d times, all successful", eventThread.m_healthCheckCallCount );

        REQUIRE( kickCount_ > kickCountBefore );
        REQUIRE( tripCount_ == tripCountBefore );
        REQUIRE( eventThread.m_currentCountMs > 0 );

        unsigned long kickCountAfterHealthChecks = kickCount_;

        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER; i++ )
        {
            sleep( 10 );
            Supervisor::monitorThreads();
        }

        REQUIRE( kickCount_ > kickCountAfterHealthChecks );
        REQUIRE( tripCount_ == tripCountBefore );
        REQUIRE( eventThread.m_currentCountMs > 0 );

        eventThread.stopWatcher();
        REQUIRE( eventThread.m_wdogTimeoutMs == TEST_TIMEOUT_MEDIUM_MS );
    }

    SECTION( "health check failure" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing health check failure triggers watchdog trip" );

        Supervisor::enableWdog();
        TimerManager timerManager;

        FailingHealthCheckThread eventThread( TEST_TIMEOUT_MEDIUM_MS, TEST_SLEEP_SHORT_MS, false );

        unsigned long tripCountBefore = tripCount_;
        REQUIRE( eventThread.m_healthCheckCallCount == 0 );

        eventThread.m_shouldFailHealthCheck = true;
        eventThread.startWatcher( timerManager );

        REQUIRE( eventThread.m_currentCountMs == eventThread.m_wdogTimeoutMs );

        uint32_t startTime = ElapsedTime::milliseconds();
        while ( ElapsedTime::deltaMilliseconds( startTime, ElapsedTime::milliseconds() ) < TEST_TIMEOUT_MEDIUM_MS )
        {

            for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER; i++ )
            {
                sleep( 10 );
                timerManager.processTimers();
                Supervisor::monitorThreads();
            }
        }

        REQUIRE( eventThread.m_healthCheckCallCount > 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Health check called %d times, all failed", eventThread.m_healthCheckCallCount );

        REQUIRE( tripCount_ > tripCountBefore );

        eventThread.stopWatcher();
    }

    SECTION( "supervisor timing and tick divider" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing supervisor timing and tick divider behavior" );

        Supervisor::enableWdog();

        WatchedThread thread( TEST_TIMEOUT_MEDIUM_MS );

        // Verify not in list initially
        REQUIRE( thread.m_inListPtr_ == nullptr );

        // Begin watching and verify it's added to supervisor's list
        thread.m_currentCountMs = thread.m_wdogTimeoutMs - 1;
        Supervisor::beginWatching( thread );
        REQUIRE( thread.m_inListPtr_ != nullptr );
        REQUIRE( thread.m_currentCountMs == thread.m_wdogTimeoutMs );


        unsigned long initialKickCount = kickCount_;
        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 5; i++ )
        {
            sleep( 10 );
            Supervisor::monitorThreads();
        }

        REQUIRE( kickCount_ > initialKickCount );
        unsigned long kicksAfterLoop1 = kickCount_;

        uint32_t originalCount  = thread.m_currentCountMs;
        thread.m_currentCountMs = TEST_TIMEOUT_MEDIUM_MS / 2;
        REQUIRE( thread.m_currentCountMs < originalCount );

        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
        {
            Supervisor::monitorThreads();
            sleep( 10 );
        }

        REQUIRE( kickCount_ > kicksAfterLoop1 );
        REQUIRE( thread.m_currentCountMs > 0 );

        // End watching and verify it's removed from supervisor's list
        Supervisor::endWatching( thread );
        REQUIRE( thread.m_inListPtr_ == nullptr );
    }

    SECTION( "multiple threads with different timeouts" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing multiple threads with varying timeout values" );

        Supervisor::enableWdog();

        WatchedThread shortThread( TEST_TIMEOUT_SHORT_MS );
        WatchedThread mediumThread( TEST_TIMEOUT_MEDIUM_MS );
        WatchedThread longThread( TEST_TIMEOUT_LONG_MS );

        REQUIRE( shortThread.m_wdogTimeoutMs == TEST_TIMEOUT_SHORT_MS );
        REQUIRE( mediumThread.m_wdogTimeoutMs == TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( longThread.m_wdogTimeoutMs == TEST_TIMEOUT_LONG_MS );

        // Verify not in list initially
        REQUIRE( shortThread.m_inListPtr_ == nullptr );
        REQUIRE( mediumThread.m_inListPtr_ == nullptr );
        REQUIRE( longThread.m_inListPtr_ == nullptr );

        // Begin watching and verify they are added to supervisor's list
        shortThread.m_currentCountMs  = shortThread.m_wdogTimeoutMs - 1;
        mediumThread.m_currentCountMs = mediumThread.m_wdogTimeoutMs - 1;
        longThread.m_currentCountMs   = longThread.m_wdogTimeoutMs - 1;
        Supervisor::beginWatching( shortThread );
        Supervisor::beginWatching( mediumThread );
        Supervisor::beginWatching( longThread );

        REQUIRE( shortThread.m_currentCountMs == shortThread.m_wdogTimeoutMs );
        REQUIRE( mediumThread.m_currentCountMs == mediumThread.m_wdogTimeoutMs );
        REQUIRE( longThread.m_currentCountMs == longThread.m_wdogTimeoutMs );
        REQUIRE( shortThread.m_inListPtr_ != nullptr );
        REQUIRE( mediumThread.m_inListPtr_ != nullptr );
        REQUIRE( longThread.m_inListPtr_ != nullptr );

        mediumThread.m_currentCountMs = TEST_TIMEOUT_SHORT_MS;
        REQUIRE( mediumThread.m_currentCountMs != mediumThread.m_wdogTimeoutMs );

        Supervisor::reloadThread( mediumThread );
        REQUIRE( mediumThread.m_currentCountMs == mediumThread.m_wdogTimeoutMs );

        REQUIRE( shortThread.m_currentCountMs == shortThread.m_wdogTimeoutMs );
        REQUIRE( longThread.m_currentCountMs == longThread.m_wdogTimeoutMs );

        unsigned long kickCountBefore = kickCount_;
        KIT_SYSTEM_TRACE_MSG( SECT_, "Retrying with extended timing for multiple threads test" );

        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 2; i++ )
        {
            sleep( 10 );
            Supervisor::monitorThreads();
        }

        REQUIRE( shortThread.m_currentCountMs <= ( TEST_TIMEOUT_SHORT_MS - OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER ) );
        REQUIRE( mediumThread.m_currentCountMs <= ( TEST_TIMEOUT_MEDIUM_MS - OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER ) );
        REQUIRE( longThread.m_currentCountMs <= ( TEST_TIMEOUT_LONG_MS - OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER ) );

        REQUIRE( shortThread.m_currentCountMs > 0 );
        REQUIRE( mediumThread.m_currentCountMs > 0 );
        REQUIRE( longThread.m_currentCountMs > 0 );

        REQUIRE( kickCount_ > kickCountBefore );

        // End watching and verify they are removed from supervisor's list
        Supervisor::endWatching( shortThread );
        Supervisor::endWatching( mediumThread );
        Supervisor::endWatching( longThread );
        REQUIRE( shortThread.m_inListPtr_ == nullptr );
        REQUIRE( mediumThread.m_inListPtr_ == nullptr );
        REQUIRE( longThread.m_inListPtr_ == nullptr );
    }

    SECTION( "watchedeventthread supervisor mode" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing WatchedEventThread in supervisor mode" );

        Supervisor::enableWdog();
        TimerManager timerManager;

        FailingHealthCheckThread supervisorThread( TEST_TIMEOUT_MEDIUM_MS, TEST_SLEEP_SHORT_MS, true );

        REQUIRE( supervisorThread.isSupervisorThread() == true );
        REQUIRE( supervisorThread.m_wdogTimeoutMs == TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( supervisorThread.m_currentCountMs == TEST_TIMEOUT_MEDIUM_MS );

        supervisorThread.startWatcher( timerManager );
        REQUIRE( supervisorThread.m_currentCountMs == supervisorThread.m_wdogTimeoutMs );

        unsigned long kickCountBefore = kickCount_;

        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
        {
            sleep( 10 );
            supervisorThread.monitorWdog();
        }

        REQUIRE( kickCount_ > kickCountBefore );


        kickCountBefore = kickCount_;

        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
        {
            sleep( 10 );
            supervisorThread.monitorWdog();
        }

        REQUIRE( kickCount_ > kickCountBefore );
        REQUIRE( supervisorThread.m_currentCountMs > 0 );
        supervisorThread.stopWatcher();

        supervisorThread.monitorWdog();
    }

    SECTION( "edge cases" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing edge cases and error conditions" );

        Supervisor::enableWdog();

        WatchedThread zeroThread( 0 );
        REQUIRE( zeroThread.m_wdogTimeoutMs == 0 );
        REQUIRE( zeroThread.m_currentCountMs == 0 );

        WatchedThread orphanThread( TEST_TIMEOUT_MEDIUM_MS );
        uint32_t      originalCount = orphanThread.m_currentCountMs;
        REQUIRE( orphanThread.m_inListPtr_ == nullptr );  // Not in supervisor's list
        Supervisor::reloadThread( orphanThread );
        REQUIRE( orphanThread.m_currentCountMs == originalCount );
        REQUIRE( orphanThread.m_inListPtr_ == nullptr );  // Still not in supervisor's list

        unsigned long kickCountBefore = kickCount_;
        Supervisor::monitorThreads();
        REQUIRE( kickCount_ >= kickCountBefore );

        WatchedThread thread( TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( thread.m_currentCountMs == TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( thread.m_inListPtr_ == nullptr );

        // Begin watching and verify it's added to supervisor's list
        thread.m_currentCountMs = thread.m_wdogTimeoutMs - 1;
        Supervisor::beginWatching( thread );
        REQUIRE( thread.m_currentCountMs == thread.m_wdogTimeoutMs );
        REQUIRE( thread.m_inListPtr_ != nullptr );

        // End watching and verify it's removed from supervisor's list
        Supervisor::endWatching( thread );
        REQUIRE( thread.m_wdogTimeoutMs == TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( thread.m_inListPtr_ == nullptr );

        // Begin watching again and verify it's added back to supervisor's list
        thread.m_currentCountMs = thread.m_wdogTimeoutMs - 1;
        Supervisor::beginWatching( thread );
        REQUIRE( thread.m_currentCountMs == thread.m_wdogTimeoutMs );
        REQUIRE( thread.m_inListPtr_ != nullptr );

        // End watching and verify it's removed from supervisor's list
        Supervisor::endWatching( thread );
        REQUIRE( thread.m_wdogTimeoutMs == TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( thread.m_inListPtr_ == nullptr );

        unsigned long tripCountBefore = tripCount_;

        Supervisor::tripWdog();
        REQUIRE( tripCount_ == tripCountBefore + 1 );

        tripCountBefore = tripCount_;
        Supervisor::tripWdog();
        REQUIRE( tripCount_ == tripCountBefore + 1 );

        Supervisor::tripWdog();
        REQUIRE( tripCount_ == tripCountBefore + 2 );
    }

    SECTION( "rawthread inline implementation" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing RawThread inlined wrapper methods" );

        Supervisor::enableWdog();

        RawThread rawThread1( TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( rawThread1.m_wdogTimeoutMs == TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( rawThread1.m_currentCountMs == TEST_TIMEOUT_MEDIUM_MS );

        RawThread rawThread2( TEST_TIMEOUT_SHORT_MS );
        REQUIRE( rawThread2.m_wdogTimeoutMs == TEST_TIMEOUT_SHORT_MS );
        REQUIRE( rawThread2.m_currentCountMs == TEST_TIMEOUT_SHORT_MS );

        // Test macro usage scenario with proper verification
        REQUIRE( rawThread1.m_inListPtr_ == nullptr );                 // Not in a list
        rawThread1.m_currentCountMs = rawThread1.m_wdogTimeoutMs - 1;  // Some other value than the reload timeout
        KIT_SYSTEM_WATCHDOG_START_RAWTHREAD( rawThread1 );
        REQUIRE( rawThread1.m_currentCountMs == rawThread1.m_wdogTimeoutMs );  // Now is the Reload timeout
        REQUIRE( rawThread1.m_inListPtr_ != nullptr );                         // In a list

        REQUIRE( rawThread2.m_inListPtr_ == nullptr );
        rawThread2.m_currentCountMs = rawThread2.m_wdogTimeoutMs - 1;
        KIT_SYSTEM_WATCHDOG_START_RAWTHREAD( rawThread2 );
        REQUIRE( rawThread2.m_currentCountMs == rawThread2.m_wdogTimeoutMs );
        REQUIRE( rawThread2.m_inListPtr_ != nullptr );

        rawThread1.m_currentCountMs = rawThread1.m_wdogTimeoutMs - 1;
        KIT_SYSTEM_WATCHDOG_KICK_RAWTHREAD( rawThread1 );
        REQUIRE( rawThread1.m_currentCountMs == rawThread1.m_wdogTimeoutMs );

        KIT_SYSTEM_WATCHDOG_STOP_RAWTHREAD( rawThread1 );
        REQUIRE( rawThread1.m_inListPtr_ == nullptr );
        KIT_SYSTEM_WATCHDOG_STOP_RAWTHREAD( rawThread2 );
        REQUIRE( rawThread2.m_inListPtr_ == nullptr );

        unsigned long kickCountBefore = kickCount_;
        for ( int i = 0; i < OPTION_KIT_SYSTEM_WATCHDOG_SUPERVISOR_TICK_DIVIDER + 1; i++ )
        {
            sleep( 10 );
            Supervisor::monitorThreads();
        }
        REQUIRE( kickCount_ >= kickCountBefore );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}