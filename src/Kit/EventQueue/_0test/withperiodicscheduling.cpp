/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/EventQueue/WithPeriodicScheduling.h"
#include "Kit/System/PeriodicScheduler.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Trace.h"
#include "Kit/System/Thread.h"
#include "Kit/System/api.h"
#include "Kit/EventQueue/WithPeriodicScheduling.h"
#include <inttypes.h>

#define SECT_                 "_0test"

#define TEST_DURATION_IN_MSEC 250

using namespace Kit::EventQueue;
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////

static inline void displayInfo( const char* label, unsigned count, uint64_t currentTick, uint64_t intervalTick, void* context )
{
    KIT_SYSTEM_TRACE_MSG( SECT_, "%-7s: %d, tick=%" PRIu32 ", interval=%" PRIu32 ", context=%p", label, count, (uint32_t)currentTick, (uint32_t)intervalTick, context );
}


static uint64_t appleLastCurrentTick_;
static uint64_t appleLastCurrentInterval_;
static void*    appleLastContext_;
static unsigned appleCount_;

static void appleProcessInterval( uint64_t currentTick, uint64_t currentInterval, void* context )
{
    appleCount_++;
    appleLastCurrentTick_     = currentTick;
    appleLastCurrentInterval_ = currentInterval;
    appleLastContext_         = context;
}

static uint64_t orangeLastCurrentTick_;
static uint64_t orangeLastCurrentInterval_;
static void*    orangeLastContext_;
static unsigned orangeCount_;

static void orangeProcessInterval( uint64_t currentTick, uint64_t currentInterval, void* context )
{
    orangeCount_++;
    orangeLastCurrentTick_     = currentTick;
    orangeLastCurrentInterval_ = currentInterval;
    orangeLastContext_         = context;
}

static unsigned slippageCount_;
static uint64_t slippageLastCurrentTick_;
static uint64_t slippageLastMissedInterval_;
static void*    slippageLastContext_;
static void     reportSlippage( PeriodicScheduler::Interval_T& intervalThatSlipped, uint64_t currentTick, uint64_t missedInterval )
{
    slippageCount_++;
    slippageLastCurrentTick_    = currentTick;
    slippageLastMissedInterval_ = missedInterval;
    slippageLastContext_        = intervalThatSlipped.context;
    displayInfo( "SLIPPAGE", slippageCount_, slippageLastCurrentTick_, slippageLastMissedInterval_, slippageLastContext_ );
}

static unsigned startLoopCount_;
static uint64_t startLoopCountLastCurrentTick_;
static void     loopStart( uint64_t currentTick )
{
    startLoopCount_++;
    startLoopCountLastCurrentTick_ = currentTick;
}

static unsigned endLoopCount_;
static uint64_t endLoopCountLastCurrentTick_;
static void     loopEnd( uint64_t currentTick )
{
    endLoopCount_++;
    endLoopCountLastCurrentTick_ = currentTick;
}

static unsigned idleCallCount_;
static uint64_t idleCallCountLastCurrentTick_;
static void     idleFunc( uint64_t currentTick, bool atLeastOneIntervalExecuted )
{
    idleCallCount_++;
    idleCallCountLastCurrentTick_ = currentTick;
}


static PeriodicScheduler::Interval_T intervals_[] = {
    { appleProcessInterval, 10, (void*)0xCAFE },
    { orangeProcessInterval, 100, (void*)0xBEEF },
    KIT_SYSTEM_PERIODIC_SCHEDULER_END_INTERVALS
};


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "EventLoopWithPScheduling" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    ShutdownUnitTesting::clearAndUseCounter();

    appleCount_                    = 0;
    appleLastCurrentTick_          = 0;
    appleLastCurrentInterval_      = 0;
    appleLastContext_              = 0;
    orangeCount_                   = 0;
    orangeLastCurrentTick_         = 0;
    orangeLastCurrentInterval_     = 0;
    orangeLastContext_             = 0;
    slippageCount_                 = 0;
    slippageLastCurrentTick_       = 0;
    slippageLastMissedInterval_    = 0;
    slippageLastContext_           = 0;
    startLoopCount_                = 0;
    startLoopCountLastCurrentTick_ = 0;
    endLoopCount_                  = 0;
    endLoopCountLastCurrentTick_   = 0;
    idleCallCount_                 = 0;


    SECTION( "happy path" )
    {
        WithPeriodicScheduling uut( intervals_, loopStart, loopEnd, reportSlippage, ElapsedTime::millisecondsEx, idleFunc );
        Thread*                testThread = Thread::create( uut, "TEST" );
        REQUIRE( testThread != nullptr );
        sleep( TEST_DURATION_IN_MSEC );
        REQUIRE( endLoopCount_ == 0 );
        uut.pleaseStop();

        REQUIRE( startLoopCount_ == 1 );
        REQUIRE( appleCount_ >= 10 );
        REQUIRE( appleCount_ <= ( TEST_DURATION_IN_MSEC / 10 ) + 1 );
        REQUIRE( orangeCount_ >= 1 );
        REQUIRE( orangeCount_ <= ( TEST_DURATION_IN_MSEC / 100 ) + 1 );
        REQUIRE( idleCallCount_ >= 10 );
        REQUIRE( idleCallCount_ <= ( TEST_DURATION_IN_MSEC / OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD ) + 1 );

        sleep( 100 );
        REQUIRE( testThread->isActive() == false );
        REQUIRE( endLoopCount_ == 1 );
        Thread::destroy( *testThread );
    }

    SECTION( "no-idle-func" )
    {
        WithPeriodicScheduling uut( intervals_, loopStart, loopEnd, reportSlippage, ElapsedTime::millisecondsEx );
        Thread*                testThread = Thread::create( uut, "TEST" );
        REQUIRE( testThread != nullptr );
        sleep( TEST_DURATION_IN_MSEC );
        REQUIRE( endLoopCount_ == 0 );
        uut.pleaseStop();

        REQUIRE( startLoopCount_ == 1 );
        REQUIRE( appleCount_ >= 10 );
        REQUIRE( appleCount_ <= ( TEST_DURATION_IN_MSEC / 10 ) + 1 );
        REQUIRE( orangeCount_ >= 1 );
        REQUIRE( orangeCount_ <= ( TEST_DURATION_IN_MSEC / 100 ) + 1 );
        REQUIRE( idleCallCount_ == 0 );

        sleep( 100 );
        REQUIRE( testThread->isActive() == false );
        REQUIRE( endLoopCount_ == 1 );
        Thread::destroy( *testThread );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
