/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Time/BootTime.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Trace.h"
#include "Kit/System/ElapsedTime.h"
#include <inttypes.h>

#define SECT_ "_0test"

using namespace Kit::Time;

// TODO: Need Model Point support
// static Kit::Dm::ModelDatabase modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );
// static Kit::Dm::Mp::Uint32    mp_bootCounter( modelDb_, "bootCounter" );


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "boottime" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    // mp_bootCounter.setInvalid(); // TODO: Need Model Point support

    SECTION( "errors" )
    {
        // Not initialized
        uint64_t now         = Kit::System::ElapsedTime::millisecondsEx();
        uint64_t bootTime    = getBootTime();
        uint16_t bootCounter = 0;
        uint64_t nowTime     = 0;
        parseBootTime( bootTime, bootCounter, nowTime );
        REQUIRE( bootCounter == 0 );
        REQUIRE( nowTime >= now );

        // MP invalid
        // initializeBootTime( mp_bootCounter ); // TODO: Need Model Point support
        // mp_bootCounter.setInvalid();
        now            = Kit::System::ElapsedTime::milliseconds();
        bootTime       = getBootTime();
        uint64_t later = Kit::System::ElapsedTime::milliseconds();
        bootCounter    = 0;
        nowTime        = 0;
        parseBootTime( bootTime, bootCounter, nowTime );
        REQUIRE( bootCounter == 0 );
        REQUIRE( nowTime >= now );
        REQUIRE( nowTime <= later );
    }

    SECTION( "happy-path" )
    {
        // TODO: Need Model Point support
        // mp_bootCounter.write( 2 );
        // initializeBootTime( mp_bootCounter );

        uint64_t now         = Kit::System::ElapsedTime::milliseconds();
        uint64_t bootTime    = getBootTime();
        uint64_t later       = Kit::System::ElapsedTime::milliseconds();
        uint16_t bootCounter = 0;
        uint64_t nowTime     = 0;
        parseBootTime( bootTime, bootCounter, nowTime );
        // REQUIRE( bootCounter == 2 ); // TODO: Need Model Point support
        REQUIRE( bootCounter == 0 );

        REQUIRE( nowTime >= now );
        REQUIRE( nowTime <= later );

        uint16_t bcount      = 0x7FFF;
        uint64_t elapsedTime = 0x00008000FFFF0123LL;
        uint64_t bt          = constructBootTime( bcount, elapsedTime );
        parseBootTime( bt, bootCounter, nowTime );
        REQUIRE( bootCounter == bcount );
        REQUIRE( nowTime == elapsedTime );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}