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

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "timer" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "nominal" )
    {
        // Create my test sequencers
        Kit::Container::SList<IEventFlag> eventListFruits;
        Kit::Container::SList<IEventFlag> eventListFlowers;
        Kit::Container::SList<IEventFlag> eventListTrees;
        MasterT                           fruits( SLOW_TICKSRC_MS_PER_TICK, DELAY_, "apple", APPLE_T1, APPLE_T2, "orange", ORANGE_T1, ORANGE_T2, Thread::getCurrent(), eventListFruits );
        MasterT                           flowers( FAST_TICKSRC_MS_PER_TICK, DELAY_, "rose", ROSE_T1, ROSE_T2, "daisy", DAISY_T1, DAISY_T2, Thread::getCurrent(), eventListFlowers );
        MasterT                           trees( FAST_TICKSRC_MS_PER_TICK, DELAY_, "oak", OAK_T1, OAK_T2, "pine", PINE_T1, PINE_T2, Thread::getCurrent(), eventListTrees );

        // Create all of the threads
        Thread* t1 = Thread::create( fruits, "FRUITS" );
        Thread* t2 = Thread::create( trees, "TREES" );
        Thread* t3 = Thread::create( flowers, "FLOWERS" );

        // Give time for all of threads to be created before starting the test sequence
        sleep( 100 );


        // Validate result of each sequence
        for ( int i = 0; i < NUM_SEQ_; i++ )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "***** STARTING TEST SEQUENCE %d *****", i + 1 );
            // Start a test sequence
            fruits.signalEvent( START_EVENT_NUM );
            flowers.signalEvent( START_EVENT_NUM );
            trees.signalEvent( START_EVENT_NUM );

            // Wait for all event loops to complete a sequence
            Thread::wait();
            Thread::wait();
            Thread::wait();
            sleep( 50 );

            uint32_t maxCount = MAX_COUNT_( fruits.m_appObj1.m_deltaTime1, APPLE_T1 );
            fruits.m_appObj1.displayTimer1( maxCount );
            REQUIRE( fruits.m_appObj1.m_count1 <= maxCount );
            REQUIRE( fruits.m_appObj1.m_minTime1 >= fruits.m_appObj1.m_timer1Duration );
            maxCount = MAX_COUNT_( fruits.m_appObj1.m_deltaTime2, APPLE_T2 );
            fruits.m_appObj1.displayTimer2( maxCount );
            REQUIRE( fruits.m_appObj1.m_count2 <= maxCount );
            REQUIRE( fruits.m_appObj1.m_minTime2 >= fruits.m_appObj1.m_timer2Duration );

            maxCount = MAX_COUNT_( fruits.m_appObj2.m_deltaTime1, ORANGE_T1 );
            fruits.m_appObj2.displayTimer1( maxCount );
            REQUIRE( fruits.m_appObj2.m_count1 <= maxCount );
            REQUIRE( fruits.m_appObj2.m_minTime1 >= fruits.m_appObj2.m_timer1Duration );
            maxCount = MAX_COUNT_( fruits.m_appObj2.m_deltaTime2, ORANGE_T2 );
            fruits.m_appObj2.displayTimer2( maxCount );
            REQUIRE( fruits.m_appObj2.m_count2 <= maxCount );
            REQUIRE( fruits.m_appObj2.m_minTime2 >= fruits.m_appObj2.m_timer2Duration );

            maxCount = MAX_COUNT_( trees.m_appObj1.m_deltaTime1, OAK_T1 );
            trees.m_appObj1.displayTimer1( maxCount );
            REQUIRE( trees.m_appObj1.m_count1 <= maxCount );
            REQUIRE( trees.m_appObj1.m_minTime1 >= trees.m_appObj1.m_timer1Duration );
            maxCount = MAX_COUNT_( trees.m_appObj1.m_deltaTime2, OAK_T2 );
            trees.m_appObj1.displayTimer2( maxCount );
            REQUIRE( trees.m_appObj1.m_count2 <= maxCount );
            REQUIRE( trees.m_appObj1.m_minTime2 >= trees.m_appObj1.m_timer2Duration );

            maxCount = MAX_COUNT_( trees.m_appObj2.m_deltaTime1, PINE_T1 );
            trees.m_appObj2.displayTimer1( maxCount );
            REQUIRE( trees.m_appObj2.m_count1 <= maxCount );
            REQUIRE( trees.m_appObj2.m_minTime1 >= trees.m_appObj2.m_timer1Duration );
            maxCount = MAX_COUNT_( trees.m_appObj2.m_deltaTime2, PINE_T2 );
            trees.m_appObj2.displayTimer2( maxCount );
            REQUIRE( trees.m_appObj2.m_count2 <= maxCount );
            REQUIRE( trees.m_appObj2.m_minTime2 >= trees.m_appObj2.m_timer2Duration );

            maxCount = MAX_COUNT_( flowers.m_appObj1.m_deltaTime1, ROSE_T1 );
            flowers.m_appObj1.displayTimer1( maxCount );
            REQUIRE( flowers.m_appObj1.m_count1 <= maxCount );
            REQUIRE( flowers.m_appObj1.m_minTime1 >= flowers.m_appObj1.m_timer1Duration );
            maxCount = MAX_COUNT_( flowers.m_appObj1.m_deltaTime2, ROSE_T2 );
            flowers.m_appObj1.displayTimer2( maxCount );
            REQUIRE( flowers.m_appObj1.m_count2 <= maxCount );
            REQUIRE( flowers.m_appObj1.m_minTime2 >= flowers.m_appObj1.m_timer2Duration );

            maxCount = MAX_COUNT_( flowers.m_appObj2.m_deltaTime1, DAISY_T1 );
            flowers.m_appObj2.displayTimer1( maxCount );
            REQUIRE( flowers.m_appObj2.m_count1 <= maxCount );
            REQUIRE( flowers.m_appObj2.m_minTime1 >= flowers.m_appObj2.m_timer1Duration );
            maxCount = MAX_COUNT_( flowers.m_appObj2.m_deltaTime2, DAISY_T2 );
            flowers.m_appObj2.displayTimer2( maxCount );
            REQUIRE( flowers.m_appObj2.m_count2 <= maxCount );
            REQUIRE( flowers.m_appObj2.m_minTime2 >= flowers.m_appObj2.m_timer2Duration );
        }


        // Shutdown threads
        fruits.pleaseStop();
        trees.pleaseStop();
        flowers.pleaseStop();

        sleep( 300 );  // allow time for threads to stop
        REQUIRE( t1->isActive() == false );
        REQUIRE( t2->isActive() == false );
        REQUIRE( t3->isActive() == false );

        Thread::destroy( *t1 );
        Thread::destroy( *t2 );
        Thread::destroy( *t3 );
    }

    SECTION( "late-timing-source" )
    {
        // Create my test sequencers
        Kit::Container::SList<IEventFlag> eventListFruits;
        Kit::Container::SList<IEventFlag> eventListFlowers;
        Kit::Container::SList<IEventFlag> eventListTrees;
        MasterT                           fruits( SLOW_TICKSRC_MS_PER_TICK, DELAY_, "apple", APPLE_T1, APPLE_T2, "orange", ORANGE_T1, ORANGE_T2, Thread::getCurrent(), eventListFruits, "late-timing-source" );
        MasterT                           flowers( FAST_TICKSRC_MS_PER_TICK, DELAY_, "rose", ROSE_T1, ROSE_T2, "daisy", DAISY_T1, DAISY_T2, Thread::getCurrent(), eventListFlowers, "late-timing-source" );
        MasterT                           trees( FAST_TICKSRC_MS_PER_TICK, DELAY_, "oak", OAK_T1, OAK_T2, "pine", PINE_T1, PINE_T2, Thread::getCurrent(), eventListTrees, "late-timing-source" );

        // Create all of the threads
        Thread* t1 = Thread::create( fruits, "FRUITS" );
        Thread* t2 = Thread::create( trees, "TREES" );
        Thread* t3 = Thread::create( flowers, "FLOWERS" );

        // Give time for all of threads to be created before starting the test sequence
        sleep( 200 );
        fruits.m_appObj1.m_timer1.setTimingSource( fruits );
        fruits.m_appObj1.m_timer2.setTimingSource( fruits );
        fruits.m_appObj2.m_timer1.setTimingSource( fruits );
        fruits.m_appObj2.m_timer2.setTimingSource( fruits );
        trees.m_appObj1.m_timer1.setTimingSource( trees );
        trees.m_appObj1.m_timer2.setTimingSource( trees );
        trees.m_appObj2.m_timer1.setTimingSource( trees );
        trees.m_appObj2.m_timer2.setTimingSource( trees );
        flowers.m_appObj1.m_timer1.setTimingSource( flowers );
        flowers.m_appObj1.m_timer2.setTimingSource( flowers );
        flowers.m_appObj2.m_timer1.setTimingSource( flowers );
        flowers.m_appObj2.m_timer2.setTimingSource( flowers );

        // Validate result of each sequence (only need to run ONCE)
        for ( int i = 0; i < 1; i++ )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "***** STARTING TEST SEQUENCE %d *****", i + 1 );
            // Start a test sequence
            fruits.signalEvent( START_EVENT_NUM );
            flowers.signalEvent( START_EVENT_NUM );
            trees.signalEvent( START_EVENT_NUM );

            // Wait for all event loops to complete a sequence
            Thread::wait();
            Thread::wait();
            Thread::wait();
            sleep( 50 );

            uint32_t maxCount = MAX_COUNT_( fruits.m_appObj1.m_deltaTime1, APPLE_T1 );
            fruits.m_appObj1.displayTimer1( maxCount );
            REQUIRE( fruits.m_appObj1.m_count1 <= maxCount );
            REQUIRE( fruits.m_appObj1.m_minTime1 >= fruits.m_appObj1.m_timer1Duration );
            maxCount = MAX_COUNT_( fruits.m_appObj1.m_deltaTime2, APPLE_T2 );
            fruits.m_appObj1.displayTimer2( maxCount );
            REQUIRE( fruits.m_appObj1.m_count2 <= maxCount );
            REQUIRE( fruits.m_appObj1.m_minTime2 >= fruits.m_appObj1.m_timer2Duration );

            maxCount = MAX_COUNT_( fruits.m_appObj2.m_deltaTime1, ORANGE_T1 );
            fruits.m_appObj2.displayTimer1( maxCount );
            REQUIRE( fruits.m_appObj2.m_count1 <= maxCount );
            REQUIRE( fruits.m_appObj2.m_minTime1 >= fruits.m_appObj2.m_timer1Duration );
            maxCount = MAX_COUNT_( fruits.m_appObj2.m_deltaTime2, ORANGE_T2 );
            fruits.m_appObj2.displayTimer2( maxCount );
            REQUIRE( fruits.m_appObj2.m_count2 <= maxCount );
            REQUIRE( fruits.m_appObj2.m_minTime2 >= fruits.m_appObj2.m_timer2Duration );

            maxCount = MAX_COUNT_( trees.m_appObj1.m_deltaTime1, OAK_T1 );
            trees.m_appObj1.displayTimer1( maxCount );
            REQUIRE( trees.m_appObj1.m_count1 <= maxCount );
            REQUIRE( trees.m_appObj1.m_minTime1 >= trees.m_appObj1.m_timer1Duration );
            maxCount = MAX_COUNT_( trees.m_appObj1.m_deltaTime2, OAK_T2 );
            trees.m_appObj1.displayTimer2( maxCount );
            REQUIRE( trees.m_appObj1.m_count2 <= maxCount );
            REQUIRE( trees.m_appObj1.m_minTime2 >= trees.m_appObj1.m_timer2Duration );

            maxCount = MAX_COUNT_( trees.m_appObj2.m_deltaTime1, PINE_T1 );
            trees.m_appObj2.displayTimer1( maxCount );
            REQUIRE( trees.m_appObj2.m_count1 <= maxCount );
            REQUIRE( trees.m_appObj2.m_minTime1 >= trees.m_appObj2.m_timer1Duration );
            maxCount = MAX_COUNT_( trees.m_appObj2.m_deltaTime2, PINE_T2 );
            trees.m_appObj2.displayTimer2( maxCount );
            REQUIRE( trees.m_appObj2.m_count2 <= maxCount );
            REQUIRE( trees.m_appObj2.m_minTime2 >= trees.m_appObj2.m_timer2Duration );

            maxCount = MAX_COUNT_( flowers.m_appObj1.m_deltaTime1, ROSE_T1 );
            flowers.m_appObj1.displayTimer1( maxCount );
            REQUIRE( flowers.m_appObj1.m_count1 <= maxCount );
            REQUIRE( flowers.m_appObj1.m_minTime1 >= flowers.m_appObj1.m_timer1Duration );
            maxCount = MAX_COUNT_( flowers.m_appObj1.m_deltaTime2, ROSE_T2 );
            flowers.m_appObj1.displayTimer2( maxCount );
            REQUIRE( flowers.m_appObj1.m_count2 <= maxCount );
            REQUIRE( flowers.m_appObj1.m_minTime2 >= flowers.m_appObj1.m_timer2Duration );

            maxCount = MAX_COUNT_( flowers.m_appObj2.m_deltaTime1, DAISY_T1 );
            flowers.m_appObj2.displayTimer1( maxCount );
            REQUIRE( flowers.m_appObj2.m_count1 <= maxCount );
            REQUIRE( flowers.m_appObj2.m_minTime1 >= flowers.m_appObj2.m_timer1Duration );
            maxCount = MAX_COUNT_( flowers.m_appObj2.m_deltaTime2, DAISY_T2 );
            flowers.m_appObj2.displayTimer2( maxCount );
            REQUIRE( flowers.m_appObj2.m_count2 <= maxCount );
            REQUIRE( flowers.m_appObj2.m_minTime2 >= flowers.m_appObj2.m_timer2Duration );
        }


        // Shutdown threads
        fruits.pleaseStop();
        trees.pleaseStop();
        flowers.pleaseStop();

        sleep( 300 );  // allow time for threads to stop
        REQUIRE( t1->isActive() == false );
        REQUIRE( t2->isActive() == false );
        REQUIRE( t3->isActive() == false );

        Thread::destroy( *t1 );
        Thread::destroy( *t2 );
        Thread::destroy( *t3 );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}