/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/

#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/Text/ToString.h"
#include <cstdint>
#include <string.h>


///
using namespace Kit::Text;
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "ToString" )
{
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "signed" )
    {
        char buffer[32];
        int8_t i8 = -122;
        const char* result = ToString::signedInt<int8_t>( i8, buffer, 5+1, 10, ' ' );
        REQUIRE( result != nullptr );
        REQUIRE( strcmp( result, "-122" ) == 0 );
        REQUIRE( strcmp( buffer, " -122" ) == 0 );
        i8 = 122;
        result = ToString::signedInt<int8_t>( i8, buffer, 4+1, 10, ' ' );
        REQUIRE( result != nullptr );
        REQUIRE( strcmp( result, "122" ) == 0 );
        REQUIRE( strcmp( buffer, " 122" ) == 0 );

        int16_t i16 = -12234;
        result = ToString::signedInt<int16_t>( i16, buffer, 9+1, 10, ' ' );
        REQUIRE( result != nullptr );
        REQUIRE( strcmp( result, "-12234" ) == 0 );
        REQUIRE( strcmp( buffer, "   -12234" ) == 0 );
        i16 = 2123;
        result = ToString::signedInt<int16_t>( i16, buffer, 8+1, 10, ' ' );
        REQUIRE( result != nullptr );
        REQUIRE( strcmp( result, "2123" ) == 0 );
        REQUIRE( strcmp( buffer, "    2123" ) == 0 );

        int32_t i32 = -2147483648;
        result = ToString::signedInt<int32_t>( i32, buffer, 13+1, 10, ' ' );
        REQUIRE( result != nullptr );
        REQUIRE( strcmp( result, "-2147483648" ) == 0 );
        REQUIRE( strcmp( buffer, "  -2147483648" ) == 0 );
        i32 = INT32_MAX;
        result = ToString::signedInt<int32_t>( i32, buffer, 12+1, 10, ' ' );
        REQUIRE( result != nullptr );
        REQUIRE( strcmp( result, "2147483647" ) == 0 );
        REQUIRE( strcmp( buffer, "  2147483647" ) == 0 );

        int64_t i64 = INT64_MIN;
        result = ToString::signedInt<int64_t>( i64, buffer, 20+1, 10, ' ' );
        REQUIRE( result == nullptr );
        i64 = INT64_MIN + 1;
        result = ToString::signedInt<int64_t>( i64, buffer, 22+1, 10, ' ' );
        REQUIRE( strcmp( result, "-9223372036854775807" ) == 0 );
        REQUIRE( strcmp( buffer, "  -9223372036854775807" ) == 0 );
        i64 = INT64_MAX;
        result = ToString::signedInt<int64_t>( i64, buffer, 22+1, 10, ' ' );
        REQUIRE( result != nullptr );
        REQUIRE( strcmp( result, "9223372036854775807" ) == 0 );
        REQUIRE( strcmp( buffer, "   9223372036854775807" ) == 0 );
    }

    SECTION( "unsigned" )
    {
        char buffer[32];
        uint8_t i8 = 12;
        const char* result = ToString::unsignedInt<uint8_t>( i8, buffer, 3+1, 10, ' ' );
        REQUIRE( result != nullptr );
        REQUIRE( strcmp( buffer, " 12" ) == 0 );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
