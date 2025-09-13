/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/SList.h"
#include "Kit/System/IEventFlag.h"
#include "Kit/System/IRunnable.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/api.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Semaphore.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/Tls.h"
#include "Kit/System/Trace.h"
#include "Kit/System/EventLoop.h"
#include <cstdint>
#include <string.h>
#include <inttypes.h>

#define SECT_ "_0test"
///
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
namespace {

class MyRunnable : public IRunnable
{
public:
    ///
    Thread& m_masterThread;
    ///
    Thread* m_nextThreadPtr;
    ///
    int m_loops;
    ///
    int m_maxLoops;
    ///
    Tls& m_tls;
    ///
    const char* m_expectedTlsValue;
    ///
    int m_tlsCompareResult;

public:
    ///
    MyRunnable( Thread&     masterThread,
                Thread*     nextThreadPtr,
                int         maxLoops,
                Tls&        tlsVariable,
                const char* expectedTlsValue )
        : m_masterThread( masterThread )
        , m_nextThreadPtr( nextThreadPtr )
        , m_loops( 0 )
        , m_maxLoops( maxLoops )
        , m_tls( tlsVariable )
        , m_expectedTlsValue( expectedTlsValue )
        , m_tlsCompareResult( -1 )
    {
    }

public:
    ///
    void entry() noexcept override
    {
        KIT_SYSTEM_TRACE_SCOPE( SECT_, Thread::myName() );

        m_tlsCompareResult = strcmp( (const char*)m_tls.get(), m_expectedTlsValue );
        KIT_SYSTEM_TRACE_MSG( SECT_, "TLS Compare: '%s' =? '%s', result=%d", (const char*)m_tls.get(), m_expectedTlsValue, m_tlsCompareResult );

        while ( m_loops < m_maxLoops )
        {
            m_loops++;
            KIT_SYSTEM_TRACE_MSG( SECT_, "Waiting.... (loops=%d)", m_loops );
            Thread::wait();
            KIT_SYSTEM_TRACE_MSG( SECT_, "Signaled.  (loops=%d)", m_loops );
            if ( m_nextThreadPtr )
            {
                KIT_SYSTEM_TRACE_MSG( SECT_, "Signaling: %s", m_nextThreadPtr->getName() );
                m_nextThreadPtr->signal();
            }

            KIT_SYSTEM_TRACE_MSG( SECT_, "Signaling Master: %s", m_masterThread.getName() );
            m_masterThread.signal();
        }
    }


    /// Store the thread's name in TLS
    void setThread( Kit::System::Thread* myThreadPtr ) noexcept override
    {
        if ( myThreadPtr )
        {
            IRunnable::setThread( myThreadPtr );
            m_tls.set( (void*)myThreadPtr->getName() );
        }
    }

    /// Override default implementation
    void pleaseStop() noexcept override
    {
        if ( m_parentThreadPtr_ != nullptr )
        {
            m_parentThreadPtr_->signal();
        }
    }
};

class MyRunnable2 : public IRunnable
{
public:
    ///
    Thread& m_masterThread;
    ///
    Semaphore& m_sema;
    ///
    Tls& m_tls;
    ///
    const char* m_expectedTlsValue;
    ///
    int m_tlsCompareResult;
    ///
    bool m_waitResult1;
    ///
    bool m_waitResult2;
    ///
    unsigned long m_delta1;
    ///
    unsigned long m_delta2;
    ///
    unsigned long m_delta3;
    ///
    unsigned long m_delta4;

public:
    ///
    MyRunnable2( Thread& masterThread, Semaphore& semaphore, Tls& tlsVariable, const char* expectedTlsValue )
        : m_masterThread( masterThread ), m_sema( semaphore ), m_tls( tlsVariable ), m_expectedTlsValue( expectedTlsValue ), m_tlsCompareResult( -1 )
    {
    }

public:
    ///
    void entry() noexcept override
    {
        KIT_SYSTEM_TRACE_SCOPE( SECT_, Thread::getCurrent().getName() );


        m_tlsCompareResult = strcmp( (const char*)m_tls.get(), m_expectedTlsValue );
        KIT_SYSTEM_TRACE_MSG( SECT_, "TLS Compare: '%s' =? '%s', result=%d", (const char*)m_tls.get(), m_expectedTlsValue, m_tlsCompareResult );

        KIT_SYSTEM_TRACE_MSG( SECT_, "Semaphore Time wait test1 (timeout occurs)..." );
        unsigned long start = ElapsedTime::milliseconds();
        m_sema.timedWait( 333 );
        m_waitResult1 = m_sema.tryWait();                         // Should be false;
        m_delta1      = ElapsedTime::deltaMilliseconds( start );  // Should be >= 333
        m_masterThread.signal();
        Kit::System::sleep( 100 );  // Allow time for my test harness to run, i.e. increment the semaphore for the next test

        KIT_SYSTEM_TRACE_MSG( SECT_, "Semaphore Time wait test2 (no timeout)..." );
        start         = ElapsedTime::milliseconds();
        m_waitResult2 = m_sema.tryWait();  // Should be true;
        m_sema.timedWait( 1000 );
        m_delta2 = ElapsedTime::deltaMilliseconds( start );  // Should be close to zero
        m_masterThread.signal();

        KIT_SYSTEM_TRACE_MSG( SECT_, "Thread Time wait test1 (timeout occurs)..." );
        start = ElapsedTime::milliseconds();
        Thread::timedWait( 333 );
        m_delta3 = ElapsedTime::deltaMilliseconds( start );  // Should be >= 333
        m_masterThread.signal();
        Kit::System::sleep( 100 );  // Allow time for my test harness to run, i.e. increment the semaphore for the next test

        KIT_SYSTEM_TRACE_MSG( SECT_, "Thread Time wait test2 (no timeout)..." );
        start = ElapsedTime::milliseconds();
        Thread::timedWait( 1000 );
        m_delta4 = ElapsedTime::deltaMilliseconds( start );  // Should be close to zero
        m_masterThread.signal();

        KIT_SYSTEM_TRACE_MSG( SECT_,
                              "Results: wait1=%d =? 0, delta1=%d >? 333, wait2=%d =? 1, delta2=%d <? 50, delta3=%d >? 333, delta4=%d <? 50.",
                              m_waitResult1,
                              (unsigned)m_delta1,
                              m_waitResult2,
                              (unsigned)m_delta2,
                              (unsigned)m_delta3,
                              (unsigned)m_delta4 );
    }


    /// Store the thread's name in TLS
    void setThread( Kit::System::Thread* myThreadPtr ) noexcept override
    {
        if ( myThreadPtr )
        {
            IRunnable::setThread( myThreadPtr );
            m_tls.set( (void*)myThreadPtr->getName() );
        }
    }
};


class Lister : public Thread::ITraverser
{
public:
    ///
    bool m_foundApple;
    bool m_foundOrange;
    bool m_foundPear;

public:
    ///
    Lister()
        : m_foundApple( false ), m_foundOrange( false ), m_foundPear( false )
    {
    }
public:
    Kit::Type::TraverserStatus item( Thread& nextThread ) noexcept override
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "<%-10s %p>", nextThread.getName(), reinterpret_cast<void*>( nextThread.getId() ) );  // Note: Not all platforms use a pointer for the thread ID

        if ( strcmp( nextThread.getName(), "Apple" ) == 0 )
        {
            m_foundApple = true;
        }
        else if ( strcmp( nextThread.getName(), "Orange" ) == 0 )
        {
            m_foundOrange = true;
        }
        else if ( strcmp( nextThread.getName(), "Pear" ) == 0 )
        {
            m_foundPear = true;
        }


        return Kit::Type::TraverserStatus::eCONTINUE;
    }
};

class MyEventFlag : public EventFlagBase
{
public:
    int counter;
    int adder;
    MyEventFlag( Kit::Container::SList<IEventFlag>& eventList,
                 uint32_t                           eventMask,
                 int                                adder )
        : EventFlagBase( eventList, eventMask )
        , counter( 0 )
        , adder( adder )
    {
    }

    void notified( uint32_t eventMask ) noexcept override
    {
        // Check that eventMask does not have any bits set that are not in mask
        REQUIRE( ( eventMask & ~m_eventFlagsMask ) == 0 );
        counter += adder;
        KIT_SYSTEM_TRACE_MSG( SECT_, "Event Flag Changed: 0x%" PRIx32 " (new count: %d)", eventMask, counter );
    }
};

}  // end anonymous namespace

////////////////////////////////////////////////////////////////////////////////
#define EVENT_FLAG_BOB               0x00000001
#define EVENT_FLAG_ANN               0x00000002
#define EVENT_FLAG_SUE               0x00000004
#define EVENT_FLAG_JIM               0x00000008

#define EVENT_FLAGS_COUNTER_01       EVENT_FLAG_BOB
#define EVENT_FLAGS_COUNTER_02       ( EVENT_FLAG_BOB | EVENT_FLAG_JIM )
#define EVENT_FLAGS_COUNTER_03       ( EVENT_FLAG_ANN | EVENT_FLAG_SUE )
#define EVENT_FLAGS_COUNTER_04       ( EVENT_FLAG_BOB | EVENT_FLAG_ANN | EVENT_FLAG_JIM )

#define EVENT_FLAGS_COUNTER_01_ADDER 1
#define EVENT_FLAGS_COUNTER_02_ADDER -7
#define EVENT_FLAGS_COUNTER_03_ADDER 20
#define EVENT_FLAGS_COUNTER_04_ADDER 10

static Kit::Container::SList<IEventFlag> eventListTrees_;
static Kit::Container::SList<IEventFlag> eventListFruits_;
static Kit::Container::SList<IEventFlag> eventListFlowers_;
static MyEventFlag                       counter01Trees_( eventListTrees_, EVENT_FLAGS_COUNTER_01, EVENT_FLAGS_COUNTER_01_ADDER );
static MyEventFlag                       counter02Trees_( eventListTrees_, EVENT_FLAGS_COUNTER_02, EVENT_FLAGS_COUNTER_02_ADDER );
static MyEventFlag                       counter03Fruits_( eventListFruits_, EVENT_FLAGS_COUNTER_03, EVENT_FLAGS_COUNTER_03_ADDER );
static MyEventFlag                       counter04Flowers_( eventListFlowers_, EVENT_FLAGS_COUNTER_04, EVENT_FLAGS_COUNTER_04_ADDER );


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "basic" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    ShutdownUnitTesting::clearAndUseCounter();
    Tls runnableName;


    SECTION( "main test" )
    {
        MyRunnable appleRun( Thread::getCurrent(), 0, 3, runnableName, "Apple" );
        Thread*    appleThreadPtr = Thread::create( appleRun, "Apple" );
        REQUIRE( appleThreadPtr != nullptr );
        REQUIRE( &( appleThreadPtr->getRunnable() ) == &appleRun );

        MyRunnable orangeRun( Thread::getCurrent(), appleThreadPtr, 4, runnableName, "Orange" );
        Thread*    orangeThreadPtr = Thread::create( orangeRun, "Orange" );
        REQUIRE( orangeThreadPtr != nullptr );
        REQUIRE( &( orangeThreadPtr->getRunnable() ) == &orangeRun );

        MyRunnable pearRun( Thread::getCurrent(), orangeThreadPtr, 3, runnableName, "Pear" );
        Thread*    pearThreadPtr = Thread::create( pearRun, "Pear" );
        REQUIRE( pearThreadPtr != nullptr );
        REQUIRE( &( pearThreadPtr->getRunnable() ) == &pearRun );


        Lister myThreadList;
        Kit::System::sleep( 200 );  // Give time to ensure all threads are active.
        Thread::traverse( myThreadList );
        REQUIRE( myThreadList.m_foundApple );
        REQUIRE( myThreadList.m_foundOrange );
        REQUIRE( myThreadList.m_foundPear );
        REQUIRE( appleRun.getThread() == appleThreadPtr);
        REQUIRE( orangeRun.getThread() == orangeThreadPtr);
        REQUIRE( pearRun.getThread() == pearThreadPtr);

        KIT_SYSTEM_TRACE_MSG( SECT_, "Signaling: %s", appleThreadPtr->getName() );
        appleRun.m_nextThreadPtr = pearThreadPtr;
        appleThreadPtr->signal();

        int i;
        for ( i = 0; i < 3 + 3 + 3; i++ )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "Wait %d", i + 1 );
            Thread::wait();
        }

        Thread::destroy( *pearThreadPtr );
        REQUIRE( orangeThreadPtr->isActive() );
        Thread::destroy( *orangeThreadPtr, 100 );  // Note: this is still an active thread at this point
        Thread::destroy( *appleThreadPtr );

        REQUIRE( appleRun.m_tlsCompareResult == 0 );
        REQUIRE( orangeRun.m_tlsCompareResult == 0 );
        REQUIRE( pearRun.m_tlsCompareResult == 0 );

        REQUIRE( appleRun.m_loops == 3 );
        REQUIRE( orangeRun.m_loops == 4 );
        REQUIRE( pearRun.m_loops == 3 );
        REQUIRE( Thread::tryWait() == false );

        Semaphore   sema;
        MyRunnable2 cherryRun( Thread::getCurrent(), sema, runnableName, "Cherry" );
        Thread*     cherryThreadPtr = Thread::create( cherryRun, "Cherry" );
        Thread::wait();
        sema.signal();
        sema.signal();
        Thread::wait();
        Thread::wait();
        cherryThreadPtr->signal();
        Thread::wait();
        REQUIRE( cherryRun.m_tlsCompareResult == 0 );
        REQUIRE( cherryRun.m_waitResult1 == false );
        REQUIRE( cherryRun.m_delta1 >= 333 - 2 );  // Use a tolerance for the test since the elapsed time and timed semaphore are guaranteed to have the same timing source
        REQUIRE( cherryRun.m_waitResult2 == true );
        REQUIRE( cherryRun.m_delta2 < 50 );
        REQUIRE( cherryRun.m_delta3 >= 333 - 2 );
        REQUIRE( cherryRun.m_delta4 < 50 );

        Thread::destroy( *cherryThreadPtr, 100 );  // Allow time for the Cherry thread to self terminate
    }

    SECTION( "Semaphores" )
    {
        Thread::getCurrent().signal();
        Thread::wait();
        bool result = Thread::tryWait();
        REQUIRE( result == false );
        Thread::getCurrent().signal();
        result = Thread::tryWait();
        REQUIRE( result == true );
        Thread::getCurrent().signal();
        result = Thread::timedWait( 100 );
        REQUIRE( result == true );
        result = Thread::timedWait( 100 );
        REQUIRE( result == false );
    }

    SECTION( "SuspendScheduling" )
    {
        // Cant really test on a desktop OS - but at least we can make sure that there is an implementation for (i.e. verify compile & link)
        Kit::System::suspendScheduling();
        Kit::System::resumeScheduling();
    }

#define NUM_SEQ_    3
#define EVENT_FLAGS 4

    SECTION( "events" )
    {
        Kit::System::EventLoop fruits( OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD, &eventListFruits_ );
        Kit::System::EventLoop trees( OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD, &eventListTrees_ );
        Kit::System::EventLoop flowers( OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD, &eventListFlowers_ );

        // Create all of the threads
        Kit::System::Thread* t1 = Kit::System::Thread::create( fruits, "FRUITS" );
        Kit::System::Thread* t2 = Kit::System::Thread::create( trees, "TREES" );
        Kit::System::Thread* t3 = Kit::System::Thread::create( flowers, "FLOWERS" );

        // Give time for all of threads to be created before starting the test sequence
        Kit::System::sleep( 100 );

        // Run the sequence N times
        KIT_SYSTEM_TRACE_MSG( SECT_, " *** Now testing single event signals..." );
        for ( int j = 1; j <= NUM_SEQ_; j++ )
        {
            // Signal the event flags
            for ( int i = 0; i < EVENT_FLAGS; i++ )
            {
                if ( i != 2 )  // Skip the 'SUE' event
                {
                    // Start a test sequence
                    fruits.signalEvent( i );
                    flowers.signalEvent( i );
                    trees.signalEvent( i );
                    Kit::System::sleep( 100 );  // Allow other threads to process
                }
            }

            // Check the results
            KIT_SYSTEM_TRACE_MSG( SECT_,
                                  "Sequence %d complete: C1=%d, C2=%d, C3=%d, C4=%d",
                                  j,
                                  counter01Trees_.counter,
                                  counter02Trees_.counter,
                                  counter03Fruits_.counter,
                                  counter04Flowers_.counter );

            REQUIRE( counter01Trees_.counter == j );
            REQUIRE( counter02Trees_.counter == j * ( EVENT_FLAGS_COUNTER_02_ADDER * 2 ) );
            REQUIRE( counter03Fruits_.counter == j * EVENT_FLAGS_COUNTER_03_ADDER );
            REQUIRE( counter04Flowers_.counter == j * ( EVENT_FLAGS_COUNTER_04_ADDER * 3 ) );
        }

        // Run the sequence N times
        KIT_SYSTEM_TRACE_MSG( SECT_, " *** Now testing multiple event signals..." );
        counter01Trees_.counter   = 0;
        counter02Trees_.counter   = 0;
        counter03Fruits_.counter  = 0;
        counter04Flowers_.counter = 0;
        for ( int j = 1; j <= NUM_SEQ_; j++ )
        {
            // Signal the event flags
            uint32_t mask = 1;
            for ( int i = 0; i < EVENT_FLAGS; i++, mask <<= 1 )
            {
                // Start a test sequence
                fruits.signalMultipleEvents( mask | EVENT_FLAG_SUE );
                flowers.signalMultipleEvents( mask | EVENT_FLAG_SUE );
                trees.signalMultipleEvents( mask | EVENT_FLAG_SUE );
                Kit::System::sleep( 100 );  // Allow other threads to process
            }

            // Check the results
            KIT_SYSTEM_TRACE_MSG( SECT_,
                                  "Sequence %d complete: C1=%d, C2=%d, C3=%d, C4=%d",
                                  j,
                                  counter01Trees_.counter,
                                  counter02Trees_.counter,
                                  counter03Fruits_.counter,
                                  counter04Flowers_.counter );

            REQUIRE( counter01Trees_.counter == j );
            REQUIRE( counter02Trees_.counter == j * ( EVENT_FLAGS_COUNTER_02_ADDER * 2 ) );
            REQUIRE( counter03Fruits_.counter == j * EVENT_FLAGS_COUNTER_03_ADDER * EVENT_FLAGS);
            REQUIRE( counter04Flowers_.counter == j * ( EVENT_FLAGS_COUNTER_04_ADDER * 3 ) );
        }

        // Shutdown threads
        fruits.pleaseStop();
        trees.pleaseStop();
        flowers.pleaseStop();

        Kit::System::sleep( 300 );  // allow time for threads to stop
        REQUIRE( t1->isActive() == false );
        REQUIRE( t2->isActive() == false );
        REQUIRE( t3->isActive() == false );

        Kit::System::Thread::destroy( *t1 );
        Kit::System::Thread::destroy( *t2 );
        Kit::System::Thread::destroy( *t3 );
        Kit::System::sleep( 300 );  // allow time for threads to stop
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
