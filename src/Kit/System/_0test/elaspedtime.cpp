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
#include "Kit/System/api.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/Trace.h"
#include <inttypes.h>

#define SECT_ "_0test"
///
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "elapsedtime" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    ShutdownUnitTesting::clearAndUseCounter();

    uint64_t msecEx;
    uint32_t msecs;
    uint32_t secs;

    // Get current time
    msecs  = ElapsedTime::milliseconds();
    msecEx = ElapsedTime::millisecondsEx();
    secs   = ElapsedTime::seconds();

    // Sleep at least 1.5 second
    sleep( 1510 );


    // Verify delta time
    REQUIRE( ElapsedTime::expiredSeconds( secs, 1 ) == true );
    REQUIRE( ElapsedTime::expiredMilliseconds( msecs, 1500 ) == true );
    REQUIRE( ElapsedTime::expiredMillisecondsEx( msecEx, 1500 ) == true );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Post verify: sleep = 1.5" );

    // Get current time
    secs   = ElapsedTime::seconds();
    msecs  = ElapsedTime::milliseconds();
    msecEx = ElapsedTime::millisecondsEx();

    // Sleep at least 1.0 second
    sleep( 1010 );

    // Verify delta time
    REQUIRE( ElapsedTime::expiredSeconds( secs, 1 ) == true );
    REQUIRE( ElapsedTime::expiredMilliseconds( msecs, 1000 ) == true );
    REQUIRE( ElapsedTime::expiredMillisecondsEx( msecEx, 1000 ) == true );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Post verify: sleep = 1.0" );

    // Get current time
    msecEx = ElapsedTime::millisecondsEx();
    msecs     = ElapsedTime::milliseconds();
    secs      = ElapsedTime::seconds();

    // Sleep at least 3.7 second
    sleep( 3710 );

    // Verify delta time
    REQUIRE( ElapsedTime::expiredSeconds( secs, 3 ) == true );
    REQUIRE( ElapsedTime::expiredMilliseconds( msecs, 3700 ) == true );
    REQUIRE( ElapsedTime::expiredMillisecondsEx( msecEx, 3700 ) == true );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Post verify: sleep = 3.7" );
    KIT_SYSTEM_TRACE_MSG( SECT_, "msec=%" PRIu32 ", msecEx=%" PRIu32 ", secs=%" PRIu32, msecs, static_cast<uint32_t>(msecEx), secs ); // Note: The cast is work-around for compilers that do not support PRIu64

    int i;
    for ( i = 0; i < 50; i++ )
    {
        sleep( 21 );
        msecs     = ElapsedTime::milliseconds();
        msecEx    = ElapsedTime::millisecondsEx();
        secs      = ElapsedTime::seconds();

        REQUIRE( msecs <= msecEx );
        REQUIRE( secs <= msecs / 1000 );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
