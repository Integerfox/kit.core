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
#include "Kit/System/BSemaMutex.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/IRunnable.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Trace.h"

#define SECT_ "_0test"
///
using namespace Kit::System;

/// Tolerance (in msec) for the blocking checks since the elapsed time and the
/// thread scheduling are not guaranteed to share the same timing source
#define TIMING_TOLERANCE_MS_ 2

/// How long the master thread holds the mutex while verifying that a 2nd thread blocks
#define HOLD_TIME_MS_ 250

///
#define NUM_INCREMENTERS_ 3

///
#define NUM_INCREMENTS_ 1000


////////////////////////////////////////////////////////////////////////////////
namespace {

/// Announces itself, then blocks until the master thread releases the mutex
class Blocker : public IRunnable
{
public:
    ///
    Thread& m_masterThread;
    ///
    BSemaMutex& m_mutex;
    ///
    volatile bool m_acquired;

public:
    ///
    Blocker( Thread& masterThread, BSemaMutex& mutex )
        : m_masterThread( masterThread )
        , m_mutex( mutex )
        , m_acquired( false )
    {
    }

public:
    ///
    void entry() noexcept override
    {
        m_masterThread.signal();  // Tell the master thread that I am about to block
        m_mutex.lock();
        m_acquired = true;
        KIT_SYSTEM_TRACE_MSG( SECT_, "Blocker acquired the mutex" );
        m_mutex.unlock();
        m_masterThread.signal();
    }
};

/// Releases a mutex that it never acquired
class Unlocker : public IRunnable
{
public:
    ///
    BSemaMutex& m_mutex;
    ///
    uint32_t m_delayMs;

public:
    ///
    Unlocker( BSemaMutex& mutex, uint32_t delayMs )
        : m_mutex( mutex )
        , m_delayMs( delayMs )
    {
    }

public:
    ///
    void entry() noexcept override
    {
        sleep( m_delayMs );
        m_mutex.unlock();
    }
};

/// Hammers a shared counter through the mutex
class Incrementer : public IRunnable
{
public:
    ///
    Thread& m_masterThread;
    ///
    BSemaMutex& m_mutex;
    ///
    uint32_t& m_counter;
    ///
    int m_iterations;

public:
    ///
    Incrementer( Thread& masterThread, BSemaMutex& mutex, uint32_t& counter, int iterations )
        : m_masterThread( masterThread )
        , m_mutex( mutex )
        , m_counter( counter )
        , m_iterations( iterations )
    {
    }

public:
    ///
    void entry() noexcept override
    {
        for ( int i = 0; i < m_iterations; i++ )
        {
            BSemaMutex::ScopeLock criticalSection( m_mutex );
            m_counter++;
        }
        m_masterThread.signal();
    }
};

}  // end anonymous namespace


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "bsemamutex" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "uncontended lock/unlock" )
    {
        // The mutex starts unlocked -->these calls must not block
        BSemaMutex mutex;
        mutex.lock();
        mutex.unlock();
        mutex.lock();
        mutex.unlock();
    }

    SECTION( "ScopeLock releases on scope exit" )
    {
        BSemaMutex mutex;
        {
            BSemaMutex::ScopeLock criticalSection( mutex );
        }

        // Would deadlock here if the ScopeLock destructor did not unlock
        mutex.lock();
        mutex.unlock();
    }

    SECTION( "a 2nd thread blocks until the mutex is released" )
    {
        BSemaMutex mutex;
        mutex.lock();

        Blocker blockerRun( Thread::getCurrent(), mutex );
        Thread* blockerPtr = Thread::create( blockerRun, "Blocker" );
        REQUIRE( blockerPtr != nullptr );

        REQUIRE( Thread::timedWait( 5000 ) );  // Wait for the Blocker thread to reach its lock() call
        sleep( HOLD_TIME_MS_ );
        REQUIRE( blockerRun.m_acquired == false );

        uint32_t start = ElapsedTime::milliseconds();
        mutex.unlock();
        REQUIRE( Thread::timedWait( 5000 ) );
        REQUIRE( blockerRun.m_acquired == true );
        REQUIRE( ElapsedTime::deltaMilliseconds( start ) < HOLD_TIME_MS_ );

        Thread::destroy( *blockerPtr, 100 );  // Allow time for the Blocker thread to self terminate
    }

    SECTION( "unlock from a different thread" )
    {
        // There is no 'owner' -->any thread may release the mutex
        BSemaMutex mutex;
        mutex.lock();

        Unlocker unlockerRun( mutex, 100 );
        Thread*  unlockerPtr = Thread::create( unlockerRun, "Unlocker" );
        REQUIRE( unlockerPtr != nullptr );

        uint32_t start = ElapsedTime::milliseconds();
        mutex.lock();
        REQUIRE( ElapsedTime::deltaMilliseconds( start ) >= 100 - TIMING_TOLERANCE_MS_ );
        mutex.unlock();

        Thread::destroy( *unlockerPtr, 100 );  // Allow time for the Unlocker thread to self terminate
    }

    SECTION( "mutual exclusion under contention" )
    {
        BSemaMutex mutex;
        uint32_t   counter = 0;

        Incrementer run1( Thread::getCurrent(), mutex, counter, NUM_INCREMENTS_ );
        Incrementer run2( Thread::getCurrent(), mutex, counter, NUM_INCREMENTS_ );
        Incrementer run3( Thread::getCurrent(), mutex, counter, NUM_INCREMENTS_ );

        Thread* t1 = Thread::create( run1, "Incr1" );
        Thread* t2 = Thread::create( run2, "Incr2" );
        Thread* t3 = Thread::create( run3, "Incr3" );
        REQUIRE( t1 != nullptr );
        REQUIRE( t2 != nullptr );
        REQUIRE( t3 != nullptr );

        for ( int i = 0; i < NUM_INCREMENTERS_; i++ )
        {
            REQUIRE( Thread::timedWait( 10000 ) );
        }
        REQUIRE( counter == (uint32_t)( NUM_INCREMENTERS_ * NUM_INCREMENTS_ ) );

        // Would deadlock here if the mutex was not left unlocked by the last thread
        mutex.lock();
        mutex.unlock();

        Thread::destroy( *t1, 100 );  // Allow time for the threads to self terminate
        Thread::destroy( *t2 );
        Thread::destroy( *t3 );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
