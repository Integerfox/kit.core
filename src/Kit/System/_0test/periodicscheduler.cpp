/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Trace.h"
#include "Kit/System/PeriodicScheduler.h"
#include <inttypes.h>

#define SECT_ "_0test"

using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////

static inline void displayInfo( const char* label, unsigned count, uint64_t currentTick, uint64_t intervalTick, void* context )
{
    KIT_SYSTEM_TRACE_MSG( SECT_, "%-7s: %d, tick=%" PRIu64 ", interval=%" PRIu64 ", context=%p", label, count, currentTick, intervalTick, context );
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

static uint64_t cherryLastCurrentTick_;
static uint64_t cherryLastCurrentInterval_;
static void*    cherryLastContext_;
static unsigned cherryCount_;

static void cherryProcessInterval( uint64_t currentTick, uint64_t currentInterval, void* context )
{
    cherryCount_++;
    cherryLastCurrentTick_     = currentTick;
    cherryLastCurrentInterval_ = currentInterval;
    cherryLastContext_         = context;
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

static PeriodicScheduler::Interval_T intervals_[] = {
    { appleProcessInterval, 10, (void*)0xCAFE },
    { orangeProcessInterval, 20, (void*)0xBEEF },
    { cherryProcessInterval, 7, (void*)0xFEED },
    KIT_SYSTEM_PERIODIC_SCHEDULER_END_INTERVALS
};

static uint64_t currentTick_;
static uint64_t now()
{
    return currentTick_;
}


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "PeriodicScheduler" )
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
    cherryCount_                   = 0;
    cherryLastCurrentTick_         = 0;
    cherryLastCurrentInterval_     = 0;
    cherryLastContext_             = 0;
    slippageCount_                 = 0;
    slippageLastCurrentTick_       = 0;
    slippageLastMissedInterval_    = 0;
    slippageLastContext_           = 0;
    startLoopCount_                = 0;
    startLoopCountLastCurrentTick_ = 0;
    endLoopCount_                  = 0;
    endLoopCountLastCurrentTick_   = 0;

    currentTick_ = 0;

    SECTION( "no slippage reporting" )
    {
        PeriodicScheduler uut( intervals_, loopStart, loopEnd, reportSlippage, now );
        uint64_t          intervalTime;

        currentTick_ = 5;
        KIT_SYSTEM_TRACE_MSG( SECT_, "== TICK: %" PRIu64, currentTick_ );
        uut.beginLoop();
        uut.executeScheduler();
        REQUIRE( appleCount_ == 0 );
        REQUIRE( orangeCount_ == 0 );
        REQUIRE( cherryCount_ == 0 );
        REQUIRE( startLoopCount_ == 1 );
        REQUIRE( startLoopCountLastCurrentTick_ == currentTick_ );
        REQUIRE( endLoopCount_ == 0 );
        currentTick_ += 5;
        KIT_SYSTEM_TRACE_MSG( SECT_, "== TICK: %" PRIu64, currentTick_ );
        uut.executeScheduler();

        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        intervalTime = 10;
        REQUIRE( appleCount_ == 1 );
        REQUIRE( appleLastCurrentTick_ == currentTick_ );
        REQUIRE( appleLastCurrentInterval_ == intervalTime );
        REQUIRE( orangeCount_ == 0 );
        intervalTime = 7;
        REQUIRE( cherryCount_ == 1 );
        REQUIRE( cherryLastCurrentTick_ == currentTick_ );
        REQUIRE( cherryLastCurrentInterval_ == intervalTime );

        currentTick_ += 5;
        KIT_SYSTEM_TRACE_MSG( SECT_, "== TICK: %" PRIu64, currentTick_ );
        uut.executeScheduler();

        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        REQUIRE( appleCount_ == 1 );
        REQUIRE( orangeCount_ == 0 );
        intervalTime = 14;
        REQUIRE( cherryCount_ == 2 );
        REQUIRE( cherryLastCurrentTick_ == currentTick_ );
        REQUIRE( cherryLastCurrentInterval_ == intervalTime );

        currentTick_ += 5;
        KIT_SYSTEM_TRACE_MSG( SECT_, "== TICK: %" PRIu64, currentTick_ );
        uut.executeScheduler();

        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        intervalTime = 20;
        REQUIRE( appleCount_ == 2 );
        REQUIRE( appleLastCurrentTick_ == currentTick_ );
        REQUIRE( appleLastCurrentInterval_ == intervalTime );
        intervalTime = 20;
        REQUIRE( orangeCount_ == 1 );
        REQUIRE( orangeLastCurrentTick_ == currentTick_ );
        REQUIRE( orangeLastCurrentInterval_ == intervalTime );
        REQUIRE( cherryCount_ == 2 );

        currentTick_ += 1;
        KIT_SYSTEM_TRACE_MSG( SECT_, "== TICK: %" PRIu64, currentTick_ );
        uut.executeScheduler();
        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        REQUIRE( appleCount_ == 2 );
        REQUIRE( orangeCount_ == 1 );
        intervalTime = 21;
        REQUIRE( cherryCount_ == 3 );
        REQUIRE( cherryLastCurrentTick_ == currentTick_ );
        REQUIRE( cherryLastCurrentInterval_ == intervalTime );

        uut.endLoop();
        REQUIRE( startLoopCount_ == 1 );
        REQUIRE( endLoopCountLastCurrentTick_ == currentTick_ );
        REQUIRE( endLoopCount_ == 1 );
    }

    SECTION( "Slippage" )
    {
        PeriodicScheduler uut( intervals_, nullptr, nullptr, reportSlippage, now );
        uint64_t          intervalTime;

        currentTick_ = 5;
        KIT_SYSTEM_TRACE_MSG( SECT_, "== TICK: %" PRIu64, currentTick_ );
        uut.beginLoop();
        uut.executeScheduler();
        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        REQUIRE( appleCount_ == 0 );
        REQUIRE( orangeCount_ == 0 );
        REQUIRE( cherryCount_ == 0 );
        REQUIRE( slippageCount_ == 0 );
        REQUIRE( startLoopCount_ == 0 );
        REQUIRE( endLoopCount_ == 0 );

        currentTick_ += 5;
        KIT_SYSTEM_TRACE_MSG( SECT_, "== TICK: %" PRIu64, currentTick_ );
        uut.executeScheduler();
        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        REQUIRE( appleCount_ == 1 );
        REQUIRE( orangeCount_ == 0 );
        REQUIRE( cherryCount_ == 1 );
        REQUIRE( slippageCount_ == 0 );

        // Slip a single interval
        currentTick_ += 20;
        KIT_SYSTEM_TRACE_MSG( SECT_, "== TICK: %" PRIu64, currentTick_ );
        uut.executeScheduler();
        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        intervalTime = 20;
        REQUIRE( appleCount_ == 2 );
        REQUIRE( appleLastCurrentTick_ == currentTick_ );
        REQUIRE( appleLastCurrentInterval_ == intervalTime );
        REQUIRE( orangeCount_ == 1 );
        intervalTime = 14;
        REQUIRE( cherryCount_ == 2 );
        REQUIRE( cherryLastCurrentTick_ == currentTick_ );
        REQUIRE( cherryLastCurrentInterval_ == intervalTime );
        REQUIRE( slippageCount_ == 2 );
        REQUIRE( slippageLastContext_ == (void*)0xFEED );

        currentTick_ += 10;
        KIT_SYSTEM_TRACE_MSG( SECT_, "== TICK: %" PRIu64, currentTick_ );
        uut.executeScheduler();
        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        intervalTime = 40;
        REQUIRE( appleCount_ == 3 );
        REQUIRE( appleLastCurrentTick_ == currentTick_ );
        REQUIRE( appleLastCurrentInterval_ == intervalTime );
        REQUIRE( orangeCount_ == 2 );
        intervalTime = ( 40 / 7 ) * 7;
        REQUIRE( cherryCount_ == 3 );
        REQUIRE( cherryLastCurrentTick_ == currentTick_ );
        REQUIRE( cherryLastCurrentInterval_ == intervalTime );
        REQUIRE( slippageCount_ == 2 );

        // Slip multiple intervals
        currentTick_ += 30;
        KIT_SYSTEM_TRACE_MSG( SECT_, "== TICK: %" PRIu64, currentTick_ );
        uut.executeScheduler();
        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        intervalTime = 40 + 10;
        REQUIRE( appleCount_ == 4 );
        REQUIRE( appleLastCurrentTick_ == currentTick_ );
        REQUIRE( appleLastCurrentInterval_ == intervalTime );
        intervalTime = ( 40 / 7 + 1 ) * 7;
        REQUIRE( cherryCount_ == 4 );
        REQUIRE( cherryLastCurrentTick_ == currentTick_ );
        REQUIRE( cherryLastCurrentInterval_ == intervalTime );

        // Verify interval boundaries after 'big' slippage
        currentTick_ += 10;
        KIT_SYSTEM_TRACE_MSG( SECT_, "== TICK: %" PRIu64, currentTick_ );
        uut.executeScheduler();
        displayInfo( "APPLE", appleCount_, appleLastCurrentTick_, appleLastCurrentInterval_, appleLastContext_ );
        displayInfo( "ORANGE", orangeCount_, orangeLastCurrentTick_, orangeLastCurrentInterval_, orangeLastContext_ );
        displayInfo( "CHERRY", cherryCount_, cherryLastCurrentTick_, cherryLastCurrentInterval_, cherryLastContext_ );
        intervalTime = 80;
        REQUIRE( appleCount_ == 5 );
        REQUIRE( appleLastCurrentTick_ == currentTick_ );
        REQUIRE( appleLastCurrentInterval_ == intervalTime );
        REQUIRE( orangeCount_ == 4 );
        intervalTime = ( 80 / 7 ) * 7;
        REQUIRE( cherryCount_ == 5 );
        REQUIRE( cherryLastCurrentTick_ == currentTick_ );
        REQUIRE( cherryLastCurrentInterval_ == intervalTime );
        REQUIRE( slippageCount_ == 4 );

        uut.endLoop();
        REQUIRE( startLoopCount_ == 0 );
        REQUIRE( endLoopCount_ == 0 );
    }


    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
