/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "timercommon.h"
#include "Kit/System/Watchdog/Supervisor.h"
#include "Kit/System/Watchdog/WatchedThread.h"
#include "Kit/System/Watchdog/WatchedEventThread.h"
#include "Kit/System/Watchdog/IWatchedEventLoop.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/Thread.h"
#include "Kit/System/EventLoop.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Api.h"
#include <memory>

#define SECT_                  "_0test"

#define TEST_TIMEOUT_SHORT_MS  100
#define TEST_TIMEOUT_MEDIUM_MS 1000
#define TEST_TIMEOUT_LONG_MS   2000
#define TEST_SLEEP_SHORT_MS    50
#define TEST_SLEEP_MEDIUM_MS   100
#define TEST_SLEEP_LONG_MS     200
#define TEST_KICK_ITERATIONS   5

///
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
namespace {

class TestWatchedThread : public WatchedThread
{
public:
    ///
    unsigned long m_kickCount;

public:
    ///
    TestWatchedThread( unsigned long timeoutMs = TEST_TIMEOUT_MEDIUM_MS )
        : WatchedThread( timeoutMs )
        , m_kickCount( 0 )
    {
    }

    ///
    void simulateKick()
    {
        m_kickCount++;
    }

    ///
    unsigned long getKickCount() const
    {
        return m_kickCount;
    }
};

/** WatchedRawThread - Simulates manual integration for plain threads.
    This represents the pattern used for non-EventLoop threads that need
    manual watchdog management.
 */
class WatchedRawThread : public WatchedThread
{
public:
    ///
    bool m_isWatching;

public:
    ///
    WatchedRawThread( unsigned long wdogTimeoutMs = TEST_TIMEOUT_MEDIUM_MS )
        : WatchedThread( wdogTimeoutMs )
        , m_isWatching( false )
    {
    }

public:
    /// Starts watchdog monitoring for this raw thread.
    /// Equivalent to Cpl::System::WatchedRawThread::startWatching()
    bool startWatching()
    {
        if ( m_isWatching )
        {
            return false;  // Already watching
        }

        Supervisor::beginWatching( *this );
        m_isWatching = true;
        return true;
    }

    /// Stops watchdog monitoring for this raw thread.
    /// Equivalent to Cpl::System::WatchedRawThread::stopWatching()
    bool stopWatching()
    {
        if ( !m_isWatching )
        {
            return false;  // Not watching
        }

        Supervisor::endWatching( *this );
        m_isWatching = false;
        return true;
    }

    /// Kicks the watchdog for this thread.
    /// Equivalent to Cpl::System::WatchedRawThread::kickWatchdog()
    void kickWatchdog()
    {
        if ( m_isWatching )
        {
            Supervisor::reloadThread( *this );
        }
    }

    ///
    bool isWatching() const
    {
        return m_isWatching;
    }
};

/// Mock runnable for testing raw threads
class MockRunnable : public IRunnable
{
public:
    ///
    unsigned long m_watchdogTimeoutMs;
    ///
    volatile bool m_shouldRun;
    ///
    volatile bool m_hasStarted;
    ///
    volatile bool m_hasStopped;
    ///
    volatile unsigned long m_kickCount;

public:
    ///
    MockRunnable( unsigned long watchdogTimeoutMs = TEST_TIMEOUT_MEDIUM_MS )
        : m_watchdogTimeoutMs( watchdogTimeoutMs )
        , m_shouldRun( true )
        , m_hasStarted( false )
        , m_hasStopped( false )
        , m_kickCount( 0 )
    {
    }

public:
    ///
    void entry() noexcept override
    {
        KIT_SYSTEM_TRACE_FUNC( SECT_ );
        m_hasStarted = true;

        // Simulate a raw thread with manual watchdog management
        WatchedThread watchedThread( m_watchdogTimeoutMs );

        // Start watching
        Supervisor::beginWatching( watchedThread );
        KIT_SYSTEM_TRACE_MSG( SECT_, "MockRunnable: Started watching thread, timeout=%lu ms", m_watchdogTimeoutMs );

        // Simulate thread work with periodic kicks
        while ( m_shouldRun && m_kickCount < TEST_KICK_ITERATIONS )
        {
            // Simulate some work
            sleep( TEST_SLEEP_MEDIUM_MS );

            // Kick the watchdog
            Supervisor::reloadThread( watchedThread );
            m_kickCount++;
            KIT_SYSTEM_TRACE_MSG( SECT_, "MockRunnable: Kicked watchdog %lu times", m_kickCount );
        }

        // Stop watching
        Supervisor::endWatching( watchedThread );
        KIT_SYSTEM_TRACE_MSG( SECT_, "MockRunnable: Stopped watching thread" );

        m_hasStopped = true;
    }

    ///
    void pleaseStop() noexcept override
    {
        m_shouldRun = false;
    }

    ///
    bool hasStarted() const
    {
        return m_hasStarted;
    }

    ///
    bool hasStopped() const
    {
        return m_hasStopped;
    }

    ///
    unsigned long getKickCount() const
    {
        return m_kickCount;
    }
};

}  // end anonymous namespace


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "watchdog" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );

    SECTION( "hal" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing core HAL watchdog functionality" );

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

    SECTION( "supervisor" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing core Supervisor watchdog operations" );

        bool enabled = Supervisor::enableWdog();
        REQUIRE( enabled == true );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor enabled watchdog successfully" );

        // Test kicking hardware watchdog
        Supervisor::kickWdog();
        KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor kicked hardware watchdog" );

        // Test tripping watchdog
        Supervisor::tripWdog();
        KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor tripped watchdog (stub mode)" );
    }

    SECTION( "watched thread" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing WatchedThread construction and basic properties" );

        TestWatchedThread thread( TEST_TIMEOUT_LONG_MS );
        REQUIRE( thread.wdogTimeoutMs == TEST_TIMEOUT_LONG_MS );
        REQUIRE( thread.currentCountMs == TEST_TIMEOUT_LONG_MS );
        KIT_SYSTEM_TRACE_MSG( SECT_, "WatchedThread timeout: %lu ms, current count: %lu ms", thread.wdogTimeoutMs, thread.currentCountMs );

        // Test default construction
        TestWatchedThread defaultThread;
        REQUIRE( defaultThread.wdogTimeoutMs == TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( defaultThread.currentCountMs == TEST_TIMEOUT_MEDIUM_MS );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Default timeout: %lu ms", defaultThread.wdogTimeoutMs );
    }

    SECTION( "supervisor mgmt" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing core thread begin/end watching functionality" );

        bool enabled = Supervisor::enableWdog();
        REQUIRE( enabled == true );

        TestWatchedThread thread1( TEST_TIMEOUT_MEDIUM_MS );
        TestWatchedThread thread2( TEST_TIMEOUT_LONG_MS );

        // Begin watching threads
        Supervisor::beginWatching( thread1 );
        Supervisor::beginWatching( thread2 );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Started watching threads (%lu ms, %lu ms)", thread1.wdogTimeoutMs, thread2.wdogTimeoutMs );

        // Test thread reloading
        thread1.currentCountMs = TEST_TIMEOUT_SHORT_MS * 3;
        Supervisor::reloadThread( thread1 );
        REQUIRE( thread1.currentCountMs == thread1.wdogTimeoutMs );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Thread reload successful, count reset to: %lu ms", thread1.currentCountMs );

        // Monitor threads
        Supervisor::monitorThreads();
        KIT_SYSTEM_TRACE_MSG( SECT_, "Thread monitoring completed" );

        // End watching threads
        Supervisor::endWatching( thread1 );
        Supervisor::endWatching( thread2 );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Stopped watching threads successfully" );
    }

    SECTION( "raw thread integration" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing raw thread with manual watchdog management" );

        bool enabled = Supervisor::enableWdog();
        REQUIRE( enabled == true );

        auto          runnable    = std::make_unique<MockRunnable>( TEST_TIMEOUT_MEDIUM_MS );
        MockRunnable* runnablePtr = runnable.get();

        // Create and start raw thread with manual watchdog management
        Thread* thread = Thread::create( *runnable, "TestWatchdogRawThread" );
        REQUIRE( thread != nullptr );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Created raw thread: %s", thread->getName() );

        // Wait for thread to start and perform watchdog operations
        sleep( TEST_SLEEP_LONG_MS * 2 );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread executed, kick count: %lu", runnablePtr->getKickCount() );

        // Stop the thread
        runnable->pleaseStop();
        Thread::destroy( *thread );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Destroyed raw thread" );

        // Verify raw thread executed watchdog operations properly
        REQUIRE( runnablePtr->hasStarted() == true );
        REQUIRE( runnablePtr->hasStopped() == true );
        REQUIRE( runnablePtr->getKickCount() > 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread watchdog integration successful, kicks: %lu", runnablePtr->getKickCount() );
    }

    SECTION( "raw thread manual" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing WatchedRawThread manual start/stop/kick operations" );

        bool enabled = Supervisor::enableWdog();
        REQUIRE( enabled == true );

        WatchedRawThread rawThread( TEST_TIMEOUT_LONG_MS );

        // Initial state - not watching
        REQUIRE( rawThread.isWatching() == false );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Initial state: not watching" );

        // Start watching
        bool started = rawThread.startWatching();
        REQUIRE( started == true );
        REQUIRE( rawThread.isWatching() == true );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Started watching raw thread, timeout: %lu ms", rawThread.wdogTimeoutMs );

        // Manual watchdog kick
        rawThread.currentCountMs = TEST_TIMEOUT_MEDIUM_MS;
        rawThread.kickWatchdog();
        REQUIRE( rawThread.currentCountMs == rawThread.wdogTimeoutMs );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Manual kick successful, count reset to: %lu ms", rawThread.currentCountMs );

        // Stop watching
        bool stopped = rawThread.stopWatching();
        REQUIRE( stopped == true );
        REQUIRE( rawThread.isWatching() == false );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Stopped watching raw thread" );
    }

    SECTION( "event thread" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing WatchedEventThread for event-based threads" );

        // Test custom construction with supervisor designation
        WatchedEventThread supervisorThread( 1500, true );
        REQUIRE( supervisorThread.wdogTimeoutMs == 1500 );
        REQUIRE( supervisorThread.currentCountMs == 1500 );
        REQUIRE( supervisorThread.isSupervisorThread() == true );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor WatchedEventThread: timeout=%lu ms, isSupervisor=%s", supervisorThread.wdogTimeoutMs, supervisorThread.isSupervisorThread() ? "true" : "false" );

        // Test default construction
        WatchedEventThread defaultThread;
        REQUIRE( defaultThread.wdogTimeoutMs == TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( defaultThread.currentCountMs == TEST_TIMEOUT_MEDIUM_MS );
        REQUIRE( defaultThread.isSupervisorThread() == false );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Default WatchedEventThread: timeout=%lu ms, isSupervisor=%s", defaultThread.wdogTimeoutMs, defaultThread.isSupervisorThread() ? "true" : "false" );

        // Test non-supervisor thread
        WatchedEventThread regularThread( TEST_TIMEOUT_MEDIUM_MS, false );
        REQUIRE( regularThread.isSupervisorThread() == false );
        REQUIRE( regularThread.wdogTimeoutMs == TEST_TIMEOUT_MEDIUM_MS );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Regular WatchedEventThread created successfully" );
    }

    SECTION( "eventloop integration" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing EventLoop integration with event-based watchdog thread" );

        bool enabled = Supervisor::enableWdog();
        REQUIRE( enabled == true );

        // Create watched event thread for EventLoop
        auto watchedThread = std::make_unique<WatchedEventThread>( TEST_TIMEOUT_MEDIUM_MS, true );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Created WatchedEventThread for EventLoop: timeout=%lu ms", watchedThread->wdogTimeoutMs );

        // Create event loop with watchdog support
        EventLoop eventLoop( TEST_TIMEOUT_MEDIUM_MS, nullptr, watchedThread.get() );

        // Start the event loop thread
        Thread* thread = Thread::create( eventLoop, "EventLoopWatchdogThread" );
        REQUIRE( thread != nullptr );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Started EventLoop thread: %s", thread->getName() );

        // Let EventLoop run with watchdog integration
        sleep( TEST_SLEEP_LONG_MS + 100 );
        KIT_SYSTEM_TRACE_MSG( SECT_, "EventLoop ran with watchdog for %d ms", TEST_SLEEP_LONG_MS + 100 );

        // Stop the event loop
        eventLoop.pleaseStop();
        Thread::destroy( *thread );
        KIT_SYSTEM_TRACE_MSG( SECT_, "EventLoop with watchdog integration completed successfully" );
    }

    SECTION( "raw vs event thread" )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Testing raw vs event-based thread watchdog patterns" );

        bool enabled = Supervisor::enableWdog();
        REQUIRE( enabled == true );

        WatchedRawThread   rawThread( 1500 );
        WatchedEventThread eventThread( 1500, false );

        // Raw thread requires manual start/stop/kick
        REQUIRE( rawThread.startWatching() == true );
        REQUIRE( rawThread.isWatching() == true );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread: manual management, watching=%s", rawThread.isWatching() ? "true" : "false" );

        // Event thread integrates with EventLoop automatically
        REQUIRE( eventThread.wdogTimeoutMs == 1500 );
        REQUIRE( eventThread.isSupervisorThread() == false );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Event thread: automatic integration, timeout=%lu ms", eventThread.wdogTimeoutMs );

        // Raw thread manual kick demonstration
        rawThread.currentCountMs = TEST_TIMEOUT_SHORT_MS * 5;
        rawThread.kickWatchdog();
        REQUIRE( rawThread.currentCountMs == 1500 );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread manual kick: count reset to %lu ms", rawThread.currentCountMs );

        // Clean up raw thread
        REQUIRE( rawThread.stopWatching() == true );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Raw vs Event thread patterns comparison completed" );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}