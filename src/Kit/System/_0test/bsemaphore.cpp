/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Api.h"
#include "Kit/System/BSemaphore.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/IRunnable.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Trace.h"

#define SECT_ "_0test"
///
using namespace Kit::System;

/// Tolerance (in msec) for the timed-wait checks since the elapsed time and the
/// semaphore timeout are not guaranteed to share the same timing source
#define TIMING_TOLERANCE_MS_ 2


////////////////////////////////////////////////////////////////////////////////
namespace {

/// Blocks on the semaphore 'maxLoops' times, signaling the master thread after each wait
class Waiter : public IRunnable
{
public:
    ///
    Thread& m_masterThread;
    ///
    BSemaphore& m_sema;
    ///
    int m_maxLoops;
    ///
    int m_loops;

public:
    ///
    Waiter( Thread& masterThread, BSemaphore& sema, int maxLoops )
        : m_masterThread( masterThread )
        , m_sema( sema )
        , m_maxLoops( maxLoops )
        , m_loops( 0 )
    {
    }

public:
    ///
    void entry() noexcept override
    {
        while ( m_loops < m_maxLoops )
        {
            m_sema.wait();
            m_loops++;
            KIT_SYSTEM_TRACE_MSG( SECT_, "Waiter signaled. (loops=%d)", m_loops );
            m_masterThread.signal();
        }
    }
};

/// Signals the semaphore once, after a delay
class DelayedSignaler : public IRunnable
{
public:
    ///
    BSemaphore& m_sema;
    ///
    uint32_t m_delayMs;

public:
    ///
    DelayedSignaler( BSemaphore& sema, uint32_t delayMs )
        : m_sema( sema )
        , m_delayMs( delayMs )
    {
    }

public:
    ///
    void entry() noexcept override
    {
        sleep( m_delayMs );
        m_sema.signal();
    }
};

}  // end anonymous namespace


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "bsemaphore" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "initial state" )
    {
        BSemaphore empty;
        REQUIRE( empty.tryWait() == false );

        BSemaphore full( true );
        REQUIRE( full.tryWait() == true );
        REQUIRE( full.tryWait() == false );
    }

    SECTION( "signal saturates at one" )
    {
        BSemaphore sema;
        sema.signal();
        sema.signal();
        sema.signal();

        // Binary semantics -->the redundant signals are discarded
        REQUIRE( sema.tryWait() == true );
        REQUIRE( sema.tryWait() == false );
    }

    SECTION( "wait consumes the signal" )
    {
        BSemaphore sema( true );
        sema.wait();  // Does not block since the semaphore starts signaled
        REQUIRE( sema.tryWait() == false );

        sema.signal();
        sema.wait();
        REQUIRE( sema.tryWait() == false );
    }

    SECTION( "timedWait - expires" )
    {
        BSemaphore sema;
        uint32_t   start = ElapsedTime::milliseconds();
        REQUIRE( sema.timedWait( 333 ) == false );
        REQUIRE( ElapsedTime::deltaMilliseconds( start ) >= 333 - TIMING_TOLERANCE_MS_ );
    }

    SECTION( "timedWait - already signaled" )
    {
        BSemaphore sema( true );
        uint32_t   start = ElapsedTime::milliseconds();
        REQUIRE( sema.timedWait( 1000 ) == true );
        REQUIRE( ElapsedTime::deltaMilliseconds( start ) < 100 );

        // The signal was consumed -->the next wait must time out
        REQUIRE( sema.timedWait( 50 ) == false );
    }

    SECTION( "timedWait - signaled by another thread" )
    {
        BSemaphore      sema;
        DelayedSignaler signalerRun( sema, 100 );
        Thread*         signalerPtr = Thread::create( signalerRun, "Signaler" );
        REQUIRE( signalerPtr != nullptr );

        uint32_t start = ElapsedTime::milliseconds();
        REQUIRE( sema.timedWait( 5000 ) == true );
        REQUIRE( ElapsedTime::deltaMilliseconds( start ) >= 100 - TIMING_TOLERANCE_MS_ );

        Thread::destroy( *signalerPtr, 100 );  // Allow time for the Signaler thread to self terminate
    }

    SECTION( "wait - signaled by another thread" )
    {
        BSemaphore sema;
        Waiter     waiterRun( Thread::getCurrent(), sema, 3 );
        Thread*    waiterPtr = Thread::create( waiterRun, "Waiter" );
        REQUIRE( waiterPtr != nullptr );

        sleep( 100 );  // Give time for the Waiter thread to block on the semaphore
        REQUIRE( waiterRun.m_loops == 0 );

        for ( int i = 1; i <= 3; i++ )
        {
            sema.signal();
            REQUIRE( Thread::timedWait( 5000 ) );
            REQUIRE( waiterRun.m_loops == i );
        }

        Thread::destroy( *waiterPtr, 100 );  // Allow time for the Waiter thread to self terminate
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
