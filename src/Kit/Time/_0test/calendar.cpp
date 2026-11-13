/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Time/calendar.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Trace.h"

#define SECT_ "_0test"

using namespace Kit::Time;

namespace {

struct Expected_T
{
    uint64_t ms;
    int      year, mon, mday, hour, min, sec, wday, yday;
    uint16_t msec;
};

// Expected values generated with Python's datetime module
const Expected_T expected_[] = {
    { 0ULL, 70, 0, 1, 0, 0, 0, 4, 0, 0 },                           // The Epoch
    { 951825600001ULL, 100, 1, 29, 12, 0, 0, 2, 59, 1 },            // Leap day in a leap century
    { 1709210096789ULL, 124, 1, 29, 12, 34, 56, 4, 59, 789 },       // Leap day
    { 4107542399999ULL, 200, 1, 28, 23, 59, 59, 0, 58, 999 },       // Non-leap century
    { 4107542400000ULL, 200, 2, 1, 0, 0, 0, 1, 59, 0 },             // ...and the following day
    { 253402300799999ULL, 8099, 11, 31, 23, 59, 59, 5, 364, 999 },  // Last millisecond of 9999
    { 2147483648000ULL, 138, 0, 19, 3, 14, 8, 2, 18, 0 },           // One second past the 32-bit time_t limit
};

}  // end anonymous namespace

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "calendar" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "msecToCalendar" )
    {
        for ( const Expected_T& e : expected_ )
        {
            struct tm cal;
            uint16_t  msec = 0xFFFF;
            msecToCalendar( e.ms, cal, msec );
            REQUIRE( cal.tm_year == e.year );
            REQUIRE( cal.tm_mon == e.mon );
            REQUIRE( cal.tm_mday == e.mday );
            REQUIRE( cal.tm_hour == e.hour );
            REQUIRE( cal.tm_min == e.min );
            REQUIRE( cal.tm_sec == e.sec );
            REQUIRE( cal.tm_wday == e.wday );
            REQUIRE( cal.tm_yday == e.yday );
            REQUIRE( cal.tm_isdst == 0 );
            REQUIRE( msec == e.msec );
        }
    }

    SECTION( "gmtimeMT" )
    {
        time_t     t = 1709210096;
        struct tm  cal;
        struct tm  ref;
        struct tm* result = gmtimeMT( &t, &cal );
        REQUIRE( result == &cal );
        uint16_t msec;
        msecToCalendar( 1709210096789ULL, ref, msec );
        REQUIRE( cal.tm_year == ref.tm_year );
        REQUIRE( cal.tm_mon == ref.tm_mon );
        REQUIRE( cal.tm_mday == ref.tm_mday );
        REQUIRE( cal.tm_hour == ref.tm_hour );
        REQUIRE( cal.tm_min == ref.tm_min );
        REQUIRE( cal.tm_sec == ref.tm_sec );
        REQUIRE( cal.tm_wday == ref.tm_wday );
        REQUIRE( cal.tm_yday == ref.tm_yday );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
