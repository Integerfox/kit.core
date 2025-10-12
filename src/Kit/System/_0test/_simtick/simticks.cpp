/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/_0test/timercommon.h"
#include "Kit/System/SimTick.h"


////////////////////////////////////////////////////////////////////////////////
namespace {

class Master2 : public IRunnable
{
public:
    ///
    Semaphore& m_sema;
    ///
    Thread* m_appleThreadPtr;
    ///
    Thread* m_cherryThreadPtr;

public:
    ///
    Master2( Semaphore& sema )
        : m_sema( sema )
        , m_appleThreadPtr( 0 )
        , m_cherryThreadPtr( 0 )
    {
    }

public:
    ///
    void entry() noexcept override
    {
        // Wait for the test to be started
        Thread::wait();

        // TEST#1 Round-robing signaling of thread
        KIT_SYSTEM_TRACE_MSG( SECT_, "Signaling: %s", m_appleThreadPtr->getName() );
        m_appleThreadPtr->signal();

        // Test#1 Round Robin threads
        int i;
        for ( i = 0; i < 3 + 3 + 3; i++ )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "Wait %d", i + 1 );
            Thread::wait();
        }

        // Test#2 Timed semaphores
        KIT_SYSTEM_TRACE_MSG( SECT_, "Test#2 (Timed semaphores - wait to start)" );
        Thread::wait();
        KIT_SYSTEM_TRACE_MSG( SECT_, "Test#2 (Timed semaphores - starting)" );
        m_sema.signal();
        m_sema.signal();
        m_sema.signal();
        Thread::wait();

        // Test#3 Timed Thread waits
        KIT_SYSTEM_TRACE_MSG( SECT_, "Test#3 (Timed Thread waits - Part1 )" );
        m_cherryThreadPtr->signal();
        Thread::wait();
        KIT_SYSTEM_TRACE_MSG( SECT_, "Test#3 (Timed Thread waits - Part2)" );
        m_cherryThreadPtr->signal();
        m_cherryThreadPtr->signal();
        Thread::wait();
        KIT_SYSTEM_TRACE_MSG( SECT_, "Test#3 (Timed Thread waits - DONE)" );
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

public:
    ///
    MyRunnable( Thread& masterThread, Thread* nextThreadPtr, int maxLoops )
        : m_masterThread( masterThread )
        , m_nextThreadPtr( nextThreadPtr )
        , m_loops( 0 )
        , m_maxLoops( maxLoops )
    {
    }

public:
    ///
    void entry() noexcept override
    {
        KIT_SYSTEM_TRACE_SCOPE( SECT_, Thread::getCurrent().getName() );

        // Wait for the test to be started
        Thread::wait();

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

            m_masterThread.signal();
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
    bool m_waitResult1;
    ///
    bool m_waitResult2;
    ///
    uint32_t m_delta1;
    ///
    uint32_t m_delta2;
    ///
    uint32_t m_delta3;
    ///
    uint32_t m_delta4;
    ///
    uint32_t m_delta5;

public:
    ///
    MyRunnable2( Thread& masterThread, Semaphore& semaphore )
        : m_masterThread( masterThread ), m_sema( semaphore )
    {
    }

public:
    ///
    void entry() noexcept override
    {
        KIT_SYSTEM_TRACE_SCOPE( SECT_, Thread::getCurrent().getName() );

        KIT_SYSTEM_TRACE_MSG( SECT_, "Semaphore Time wait test1 (timeout occurs)..." );
        uint32_t start = ElapsedTime::milliseconds();
        m_sema.timedWait( 333 );
        m_waitResult1 = m_sema.tryWait();                         // Should be false;
        m_delta1      = ElapsedTime::deltaMilliseconds( start );  // Should be >= 333
        m_masterThread.signal();

        // What till the the test harness is ready for the next test
        m_sema.wait();
        sleep( 30 );  // Allow time for the semaphore to signaled more than once.

        KIT_SYSTEM_TRACE_MSG( SECT_, "Semaphore Time wait test2 (no timeout)..." );
        start         = ElapsedTime::milliseconds();
        m_waitResult2 = m_sema.tryWait();  // Should be true;
        m_sema.timedWait( 1000 );
        m_delta2 = ElapsedTime::deltaMilliseconds( start );  // Should be close to zero
        m_masterThread.signal();

        // What till the the test harness is ready for the next test
        Thread::wait();
        sleep( 1 );  // Allow time for the semaphore to signaled more than once.

        KIT_SYSTEM_TRACE_MSG( SECT_, "Thread Time wait test1 (timeout occurs)..." );
        start = ElapsedTime::milliseconds();
        Thread::timedWait( 333 );
        m_delta4 = ElapsedTime::deltaMilliseconds( start );  // Should be >= 333
        m_masterThread.signal();

        // What till the the test harness is ready for the next test
        Thread::wait();
        sleep( 1 );

        KIT_SYSTEM_TRACE_MSG( SECT_, "Thread Time wait test2 (no timeout)..." );
        start = ElapsedTime::milliseconds();
        Thread::timedWait( 1000 );
        m_delta5 = ElapsedTime::deltaMilliseconds( start );  // Should be close to zero
        m_masterThread.signal();


        KIT_SYSTEM_TRACE_MSG( SECT_,
                              "Results: wait1=%d =? 0, delta1=%" PRIu32 " >=? 333, wait2=%u =? 1, delta2=%" PRIu32 " <? 50, delta4=%" PRIu32 " >=? 333, delta5=%" PRIu32 " <? 50.",
                              m_waitResult1,
                              m_delta1,
                              m_waitResult2,
                              m_delta2,
                              m_delta4,
                              m_delta5 );

        KIT_SYSTEM_TRACE_MSG( SECT_, "Sleeping (40)..." );
        start = ElapsedTime::milliseconds();
        sleep( 40 );
        m_delta3 = ElapsedTime::deltaMilliseconds( start );
        KIT_SYSTEM_TRACE_MSG( SECT_, "DONE Sleeping (delta=%" PRIu32 ")", m_delta3 );
    }
};

class MyRunnable3 : public IRunnable
{
public:
    volatile bool m_run;

public:
    ///
    MyRunnable3()
        : m_run( true )
    {
    }

public:
    ///
    void entry() noexcept override
    {
        KIT_SYSTEM_TRACE_SCOPE( SECT_, Thread::myName() );

        while ( m_run )
        {
            sleep( 1 );
        }
    }


    /// Override default implementation
    void pleaseStop() noexcept override
    {
        m_run = false;
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


};  // end namespace

////////////////////////////////////////////////////////////////////////////////
// NOTE: Including the 'simtimer' test case in the same file as 'simticks' forces it
//       run first.  There is issue where running simtimer' AFTER 'simticks' locks
//       up the 'simtimer' test.  Running each test by themselves works - or if
//       'simticks' runs first (Have not been able to 'linkage' between the two cases!)

TEST_CASE( "simtimer" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    ShutdownUnitTesting::clearAndUseCounter();

    // Create my test sequencers
    Kit::Container::SList<IEventFlag> eventListFruits;
    Kit::Container::SList<IEventFlag> eventListFlowers;
    Kit::Container::SList<IEventFlag> eventListTrees;
    MasterT                           fruits( SLOW_TICKSRC_MS_PER_TICK, DELAY_, "apple", APPLE_T1, APPLE_T2, "orange", ORANGE_T1, ORANGE_T2, Thread::getCurrent(), eventListFruits );
    MasterT                           flowers( FAST_TICKSRC_MS_PER_TICK, DELAY_, "rose", ROSE_T1, ROSE_T2, "daisy", DAISY_T1, DAISY_T2, Thread::getCurrent(), eventListFlowers );
    MasterT                           trees( FAST_TICKSRC_MS_PER_TICK, DELAY_, "oak", OAK_T1, OAK_T2, "pine", PINE_T1, PINE_T2, Thread::getCurrent(), eventListTrees );

    // Create all of the threads
    Kit::System::Thread* t1 = Kit::System::Thread::create( fruits, "FRUITS" );
    Kit::System::Thread* t2 = Kit::System::Thread::create( trees, "TREES" );
    Kit::System::Thread* t3 = Kit::System::Thread::create( flowers, "FLOWERS" );
    REQUIRE( t1 );
    REQUIRE( t2 );
    REQUIRE( t3 );

    // Give time for all of threads to be created before starting the test sequence
    sleepInRealTime( 1000 );

    // Validate result of each sequence
    for ( int i = 0; i < NUM_SEQ_; i++ )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "***** STARTING TEST SEQUENCE %d *****", i + 1 );
        // Start a test sequence
        fruits.signalEvent( START_EVENT_NUM );
        flowers.signalEvent( START_EVENT_NUM );
        trees.signalEvent( START_EVENT_NUM );

        // Generate Simulated ticks
        uint64_t ticks = DELAY_ + DELAY_ / 2;
        while ( ticks )
        {
            uint64_t numTicks = ticks > DELAY_ / 8 ? DELAY_ / 8 : ticks;
            KIT_SYSTEM_TRACE_MSG( SECT_,  "Timer: advancing %" PRIu64 " ticks", numTicks );
            SimTick::advance( numTicks );
            sleepInRealTime( 100 );
            ticks -= numTicks;
        }

        // Wait for all event loops to complete a sequence
        Kit::System::Thread::wait();
        Kit::System::Thread::wait();
        Kit::System::Thread::wait();
        sleepInRealTime( 300 );

        uint32_t maxCount = MAX_COUNT_( (uint32_t)fruits.m_appObj1.m_deltaTime1, APPLE_T1 );
        fruits.m_appObj1.displayTimer1( maxCount );
        REQUIRE( fruits.m_appObj1.m_count1 <= maxCount );
        REQUIRE( fruits.m_appObj1.m_minTime1 >= fruits.m_appObj1.m_timer1Duration );
        maxCount = MAX_COUNT_( (uint32_t)fruits.m_appObj1.m_deltaTime2, APPLE_T2 );
        fruits.m_appObj1.displayTimer2( maxCount );
        REQUIRE( fruits.m_appObj1.m_count2 <= maxCount );
        REQUIRE( fruits.m_appObj1.m_minTime2 >= fruits.m_appObj1.m_timer2Duration );

        maxCount = MAX_COUNT_( (uint32_t)fruits.m_appObj2.m_deltaTime1, ORANGE_T1 );
        fruits.m_appObj2.displayTimer1( maxCount );
        REQUIRE( fruits.m_appObj2.m_count1 <= maxCount );
        REQUIRE( fruits.m_appObj2.m_minTime1 >= fruits.m_appObj2.m_timer1Duration );
        maxCount = MAX_COUNT_( (uint32_t)fruits.m_appObj2.m_deltaTime2, ORANGE_T2 );
        fruits.m_appObj2.displayTimer2( maxCount );
        REQUIRE( fruits.m_appObj2.m_count2 <= maxCount );
        REQUIRE( fruits.m_appObj2.m_minTime2 >= fruits.m_appObj2.m_timer2Duration );

        maxCount = MAX_COUNT_( (uint32_t)trees.m_appObj1.m_deltaTime1, OAK_T1 );
        trees.m_appObj1.displayTimer1( maxCount );
        REQUIRE( trees.m_appObj1.m_count1 <= maxCount );
        REQUIRE( trees.m_appObj1.m_minTime1 >= trees.m_appObj1.m_timer1Duration );
        maxCount = MAX_COUNT_( (uint32_t)trees.m_appObj1.m_deltaTime2, OAK_T2 );
        trees.m_appObj1.displayTimer2( maxCount );
        REQUIRE( trees.m_appObj1.m_count2 <= maxCount );
        REQUIRE( trees.m_appObj1.m_minTime2 >= trees.m_appObj1.m_timer2Duration );

        maxCount = MAX_COUNT_( (uint32_t)trees.m_appObj2.m_deltaTime1, PINE_T1 );
        trees.m_appObj2.displayTimer1( maxCount );
        REQUIRE( trees.m_appObj2.m_count1 <= maxCount );
        REQUIRE( trees.m_appObj2.m_minTime1 >= trees.m_appObj2.m_timer1Duration );
        maxCount = MAX_COUNT_( (uint32_t)trees.m_appObj2.m_deltaTime2, PINE_T2 );
        trees.m_appObj2.displayTimer2( maxCount );
        REQUIRE( trees.m_appObj2.m_count2 <= maxCount );
        REQUIRE( trees.m_appObj2.m_minTime2 >= trees.m_appObj2.m_timer2Duration );

        maxCount = MAX_COUNT_( (uint32_t)flowers.m_appObj1.m_deltaTime1, ROSE_T1 );
        flowers.m_appObj1.displayTimer1( maxCount );
        REQUIRE( flowers.m_appObj1.m_count1 <= maxCount );
        REQUIRE( flowers.m_appObj1.m_minTime1 >= flowers.m_appObj1.m_timer1Duration );
        maxCount = MAX_COUNT_( (uint32_t)flowers.m_appObj1.m_deltaTime2, ROSE_T2 );
        flowers.m_appObj1.displayTimer2( maxCount );
        REQUIRE( flowers.m_appObj1.m_count2 <= maxCount );
        REQUIRE( flowers.m_appObj1.m_minTime2 >= flowers.m_appObj1.m_timer2Duration );

        maxCount = MAX_COUNT_( (uint32_t)flowers.m_appObj2.m_deltaTime1, DAISY_T1 );
        flowers.m_appObj2.displayTimer1( maxCount );
        REQUIRE( flowers.m_appObj2.m_count1 <= maxCount );
        REQUIRE( flowers.m_appObj2.m_minTime1 >= flowers.m_appObj2.m_timer1Duration );
        maxCount = MAX_COUNT_( (uint32_t)flowers.m_appObj2.m_deltaTime2, DAISY_T2 );
        flowers.m_appObj2.displayTimer2( maxCount );
        REQUIRE( flowers.m_appObj2.m_count2 <= maxCount );
        REQUIRE( flowers.m_appObj2.m_minTime2 >= flowers.m_appObj2.m_timer2Duration );
    }


    // Shutdown threads
    fruits.pleaseStop();
    trees.pleaseStop();
    flowers.pleaseStop();

    // Generate Simulated ticks
    SimTick::advance( DELAY_ );

    sleepInRealTime( 300 );  // allow time for threads to stop
    REQUIRE( t1->isActive() == false );
    REQUIRE( t2->isActive() == false );
    REQUIRE( t3->isActive() == false );

    Kit::System::Thread::destroy( *t1 );
    Kit::System::Thread::destroy( *t2 );
    Kit::System::Thread::destroy( *t3 );

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "simbasic" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    ShutdownUnitTesting::clearAndUseCounter();

    KIT_SYSTEM_TRACE_MSG( SECT_,  "Creating Test Threads.  Real Elapsed time=%" PRIu32, ElapsedTime::millisecondsInRealTime() );

    Semaphore sema;
    Master2   masterRun( sema );
    Thread*   masterThreadPtr = Thread::create( masterRun, "MASTER", KIT_SYSTEM_THREAD_PRIORITY_NORMAL + KIT_SYSTEM_THREAD_PRIORITY_LOWER );

    MyRunnable appleRun( *masterThreadPtr, 0, 3 );
    Thread*    appleThreadPtr = Thread::create( appleRun, "Apple", KIT_SYSTEM_THREAD_PRIORITY_NORMAL );

    MyRunnable orangeRun( *masterThreadPtr, appleThreadPtr, 3 );
    Thread*    orangeThreadPtr = Thread::create( orangeRun, "Orange", KIT_SYSTEM_THREAD_PRIORITY_NORMAL );

    MyRunnable pearRun( *masterThreadPtr, orangeThreadPtr, 3 );
    Thread*    pearThreadPtr = Thread::create( pearRun, "Pear", KIT_SYSTEM_THREAD_PRIORITY_NORMAL );
    appleRun.m_nextThreadPtr = pearThreadPtr;

    MyRunnable2 cherryRun( *masterThreadPtr, sema );
    Thread*     cherryThreadPtr = Thread::create( cherryRun, "Cherry", KIT_SYSTEM_THREAD_PRIORITY_NORMAL );

    MyRunnable3 timerRun;
    Thread*     timerThreadPtr = Thread::create( timerRun, "Timer", KIT_SYSTEM_THREAD_PRIORITY_NORMAL );


    // Update my master thread with additional info
    masterRun.m_appleThreadPtr  = appleThreadPtr;
    masterRun.m_cherryThreadPtr = cherryThreadPtr;

    Lister myThreadList;
    sleepInRealTime( 500 );  // Give time to ensure all threads are active.
    KIT_SYSTEM_TRACE_MSG( SECT_, "ALL Threads should have been created and up and running.  Real Elapsed time=%" PRIu32, ElapsedTime::millisecondsInRealTime() );
    Thread::traverse( myThreadList );
    REQUIRE( myThreadList.m_foundApple );
    REQUIRE( myThreadList.m_foundOrange );
    REQUIRE( myThreadList.m_foundPear );

    // Start the tests
    masterThreadPtr->signal();
    appleThreadPtr->signal();
    pearThreadPtr->signal();
    orangeThreadPtr->signal();

    // Advance simulated time for the FIRST test
    while ( appleThreadPtr->isActive() ||
            orangeThreadPtr->isActive() ||
            pearThreadPtr->isActive() )
    {
        if ( !SimTick::advance( 1 ) )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "EXITED tick loop early (waiting on apple, orange, & pear)" );
            break;
        }
    }


    KIT_SYSTEM_TRACE_MSG( SECT_, "Cleaning up Pear, Apple, and Orange... [SimTime=%" PRIu64 "]", SimTick::current() );
    Thread::destroy( *orangeThreadPtr );
    Thread::destroy( *pearThreadPtr );
    Thread::destroy( *appleThreadPtr );

    // Advance simulated time for the SECOND test (and to finish the terminate of the orange thread)
    KIT_SYSTEM_TRACE_MSG( SECT_,  "Waiting for timed Semaphore test to complete" );
    while ( cherryThreadPtr->isActive() ||
            masterThreadPtr->isActive() )
    {
        if ( !SimTick::advance( 1 ) )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "EXITED tick loop early (waiting on cherry & master)" );
            break;
        }
    }

    // Shutdown timer thread
    KIT_SYSTEM_TRACE_MSG( SECT_,  "Shutting down timer thread..." ) ;
    timerRun.pleaseStop();
    while ( timerThreadPtr->isActive() )
    {
        if ( !SimTick::advance( 1 ) )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "EXITED tick loop early (waiting on timer)" );
            break;
        }
    }

    // Clean-up
    KIT_SYSTEM_TRACE_MSG( SECT_, "ALL simulated ticks completed.  [SimTime=%" PRIu64 "]", SimTick::current() );
    Thread::destroy( *cherryThreadPtr );
    Thread::destroy( *masterThreadPtr );
    Thread::destroy( *timerThreadPtr );

    REQUIRE( appleRun.m_loops == 3 );
    REQUIRE( orangeRun.m_loops == 3 );
    REQUIRE( pearRun.m_loops == 3 );


    REQUIRE( cherryRun.m_waitResult1 == false );
    REQUIRE( cherryRun.m_delta1 >= 333 );
    REQUIRE( cherryRun.m_waitResult2 == true );
    REQUIRE( cherryRun.m_delta2 < 50 );
    REQUIRE( cherryRun.m_delta3 >= 40 );
    REQUIRE( cherryRun.m_delta4 >= 333 );
    REQUIRE( cherryRun.m_delta5 < 200 );

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
