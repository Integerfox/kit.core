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
#include "Kit/Text/StringTo.h"
#include <string.h>


///
using namespace Kit::Text;
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "StringTo" )
{

    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "conversions...." )
    {
        const char* strPtr = "12.3";
        const char* endPtr = 0;
        int16_t     value  = -1;

        bool result = StringTo::signedInt<int16_t>( value, "123" );
        REQUIRE( result == true );
        REQUIRE( value == 123 );
        result = StringTo::signedInt<int16_t>( value, "444 " );
        REQUIRE( result == false );
        REQUIRE( value == 123 );  // Conversion failed -->value should not be changed
        result = StringTo::signedInt<int16_t>( value, " -1234(" );
        REQUIRE( result == false );
        REQUIRE( value == 123 );  // Conversion failed -->value should not be changed
        result = StringTo::signedInt<int16_t>( value, "12.3", ":,." );
        REQUIRE( result == true );
        REQUIRE( value == 12 );
        result = StringTo::signedInt<int16_t>( value, strPtr, ".,", &endPtr );
        REQUIRE( result == true );
        REQUIRE( value == 12 );
        REQUIRE( endPtr == strPtr + 2 );
        result = StringTo::signedInt<int16_t>( value, "1.23" );
        REQUIRE( result == false );
        REQUIRE( value == 12 );  // Conversion failed -->value should not be changed

        result = StringTo::signedInt<int16_t>( value, "0xF" );
        REQUIRE( result == false );
        REQUIRE( value == 12 );  // Conversion failed -->value should not be changed
        result = StringTo::signedInt<int16_t>( value, "0xF" );
        REQUIRE( result == false );
        REQUIRE( value == 12 );
        result = StringTo::signedInt<int16_t>( value, "FFFF" );
        REQUIRE( result == false );
        REQUIRE( value == 12 );
        result = StringTo::signedInt<int16_t>( value, nullptr );
        REQUIRE( result == false );
    }

    SECTION( "unsigned..." )
    {
        const char* strPtr = "-13";
        const char* endPtr = 0;
        uint8_t     value  = 0;

        bool result = StringTo::unsignedInt<uint8_t>( value, "123" );
        REQUIRE( value == 123 );
        REQUIRE( result == true );
        result = StringTo::unsignedInt<uint8_t>( value, strPtr, 10, 0, &endPtr );
        REQUIRE( result == false );
        REQUIRE( value == (uint8_t)123 );
        result = StringTo::unsignedInt<uint8_t>( value, "44 " );
        REQUIRE( result == false );
        strPtr = "32.0";
        result = StringTo::unsignedInt<uint8_t>( value, strPtr, 10, ".,", &endPtr );
        REQUIRE( result == true );
        REQUIRE( value == 32 );
        REQUIRE( endPtr == strPtr + 2 );

        result = StringTo::unsignedInt<uint8_t>( value, nullptr );
        REQUIRE( result == false );
    }

    SECTION( "floating" )
    {
        const char* strPtr = "-123.32";
        const char* endPtr = 0;
        float value;

        bool result = StringTo::floating( value, "123.32" );
        REQUIRE( result == true );
        REQUIRE( value == 123.32f );
        result = StringTo::floating( value, strPtr, nullptr, &endPtr );
        REQUIRE( result == true );
        REQUIRE( value == -123.32f );
        REQUIRE( endPtr == strPtr + 7 );
        result = StringTo::floating( value, "3.40e38" );
        REQUIRE( result == true );
        REQUIRE( value >= 3.40e38f );
        result = StringTo::floating( value, "1.7976931348623157E+308" );
        REQUIRE( result == true );
        REQUIRE( value == std::numeric_limits<float>::infinity() );
        double v2;
        result = StringTo::floating( v2, "1.7976931348623157E+308" );
        REQUIRE( result == true );
        REQUIRE( v2 >= 1.7976931348623157E+308 );

        result = StringTo::floating( value, nullptr );
        REQUIRE( result == false );
    }

    SECTION( "bool" )
    {
        const char* strPtr = "false";
        const char* endPtr = 0;
        bool value;

        bool result = StringTo::boolean( value, "T" );
        REQUIRE( value == true );
        REQUIRE( result == true );
        result = StringTo::boolean( value, strPtr, "true", "false", &endPtr );
        REQUIRE( value == false );
        REQUIRE( result == true );
        REQUIRE( endPtr == strPtr + 5 );
        result = StringTo::boolean( value, 0 );
        REQUIRE( result == false );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
