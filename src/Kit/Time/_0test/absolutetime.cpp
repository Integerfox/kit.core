/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Time/AbsoluteTime.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Trace.h"
#include <inttypes.h>
#include <time.h>

#define SECT_ "_0test"

////////////////////////////////////////////////////////////////////////////////
// NOTE: The test assumes/uses the Posix/Win32 implementation based on the build host
TEST_CASE( "absolutetime" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "happy-path" )
    {
        // Get host time using the ANSI C API
        time_t now;
        time( &now );
        auto now64 = static_cast<uint64_t>( now );

        uint64_t nowSec   = Kit::Time::getAbsoluteTimeSeconds();
        uint64_t nowMsec  = Kit::Time::getAbsoluteTimeMilliseconds();
        time_t   ansiTime = now;
        KIT_SYSTEM_TRACE_MSG( SECT_, "AnsiNow=%" PRIu64 ", UTC time=%s, sizeof(time_t)=%zu", now64, asctime( gmtime( &ansiTime ) ), sizeof( time_t ) );
        ansiTime = nowSec;
        KIT_SYSTEM_TRACE_MSG( SECT_, "uutSec=%" PRIu64 ", UTC time=%s, sizeof(time_t)=%zu", nowSec, asctime( gmtime( &ansiTime ) ), sizeof( time_t ) );

        // Make sure times are 'consistent'
        REQUIRE( (nowSec*1000L) <= nowMsec );
        REQUIRE( nowMsec < ( (nowSec + 1)*1000L ) );
        REQUIRE( now64 <= nowSec );
        REQUIRE( nowSec <= ( now64 + 2 ) );  // Allow for 2 seconds of 'slop'
        REQUIRE( now64*1000L <= nowMsec );
        REQUIRE( nowMsec <= ( ( now64 + 2 ) * 1000L ) );  // Allow for 2 seconds of 'slop'
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
