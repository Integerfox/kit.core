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
#include "Kit/Text/Parse.h"
#include <string.h>


///
using namespace Kit::Text;
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Parse" )
{

    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "hextobuffer" )
    {
        const char* text = "AA0055BBFC";
        uint8_t     buffer[5];

        size_t result = Parse::asciiHex( buffer, text, sizeof( buffer ) );
        REQUIRE( result == 5 );
        REQUIRE( buffer[0] == 0xAA );
        REQUIRE( buffer[1] == 0x00 );
        REQUIRE( buffer[2] == 0x55 );
        REQUIRE( buffer[3] == 0xBB );
        REQUIRE( buffer[4] == 0xFC );

        result = Parse::asciiHex( buffer, 0, sizeof( buffer ) );
        REQUIRE( result == SIZE_MAX );
        result = Parse::asciiHex( 0, text, sizeof( buffer ) );
        REQUIRE( result == SIZE_MAX );
        result = Parse::asciiHex( buffer, text, 0 );
        REQUIRE( result == SIZE_MAX );

        result = Parse::asciiHex( buffer, "AA3", sizeof( buffer ) );
        REQUIRE( result == SIZE_MAX );

        result = Parse::asciiHex( buffer, "bobo", sizeof( buffer ) );
        REQUIRE( result == SIZE_MAX );

        result = Parse::asciiHex( buffer, text, 3 );
        REQUIRE( result == SIZE_MAX );
    }

    SECTION( "timestamp-64" )
    {
        const char* text = "12:13:14.5";
        uint64_t    time;
        bool        result = Parse::timestamp( text, time );
        REQUIRE( result == true );
        uint64_t expected = 12LL * 60 * 60 * 1000 + 13 * 60 * 1000 + 14 * 1000 + 500;
        REQUIRE( time == expected );

        text     = "02 12:13:14.05";
        expected = 2LL * 24 * 60 * 60 * 1000 + 12LL * 60 * 60 * 1000 + 13 * 60 * 1000 + 14 * 1000 + 50;
        result   = Parse::timestamp( text, time );
        REQUIRE( result == true );
        REQUIRE( time == expected );

        text     = "2 12:3:14.005";
        expected = 2LL * 24 * 60 * 60 * 1000 + 12LL * 60 * 60 * 1000 + 3 * 60 * 1000 + 14 * 1000 + 5;
        result   = Parse::timestamp( text, time );
        REQUIRE( result == true );
        REQUIRE( time == expected );

        text     = "02 12:13:14";
        expected = 2LL * 24 * 60 * 60 * 1000 + 12LL * 60 * 60 * 1000 + 13 * 60 * 1000 + 14 * 1000;
        result   = Parse::timestamp( text, time );
        REQUIRE( result == true );
        REQUIRE( time == expected );

        text     = "12:13:14";
        expected = 12LL * 60 * 60 * 1000 + 13 * 60 * 1000 + 14 * 1000;
        result   = Parse::timestamp( text, time );
        REQUIRE( result == true );
        REQUIRE( time == expected );

        text   = "02-12:13:14";
        result = Parse::timestamp( text, time );
        REQUIRE( result == false );

        text   = "02-12:13:14.1234";
        result = Parse::timestamp( text, time );
        REQUIRE( result == false );
        result = Parse::timestamp( nullptr, time );
        REQUIRE( result == false );

        text     = "27375 12:13:14.5";
        expected = 27375LL * 24 * 60 * 60 * 1000 + 12LL * 60 * 60 * 1000 + 13 * 60 * 1000 + 14 * 1000 + 500;
        result   = Parse::timestamp( text, time );
        REQUIRE( result == true );
        REQUIRE( time == expected );
    }

    SECTION( "timestamp-32" )
    {
        const char* text = "12:13:14.5";
        uint32_t    time;
        bool        result = Parse::timestamp( text, time );
        REQUIRE( result == true );
        uint64_t expected = 12LL * 60 * 60 * 1000 + 13 * 60 * 1000 + 14 * 1000 + 500;
        REQUIRE( time == expected );

        text     = "02 12:13:14.05";
        expected = 2LL * 24 * 60 * 60 * 1000 + 12LL * 60 * 60 * 1000 + 13 * 60 * 1000 + 14 * 1000 + 50;
        result   = Parse::timestamp( text, time );
        REQUIRE( result == true );
        REQUIRE( time == expected );

        text     = "2 12:3:14.005";
        expected = 2LL * 24 * 60 * 60 * 1000 + 12LL * 60 * 60 * 1000 + 3 * 60 * 1000 + 14 * 1000 + 5;
        result   = Parse::timestamp( text, time );
        REQUIRE( result == true );
        REQUIRE( time == expected );

        text     = "02 12:13:14";
        expected = 2LL * 24 * 60 * 60 * 1000 + 12LL * 60 * 60 * 1000 + 13 * 60 * 1000 + 14 * 1000;
        result   = Parse::timestamp( text, time );
        REQUIRE( result == true );
        REQUIRE( time == expected );

        text     = "12:13:14";
        expected = 12LL * 60 * 60 * 1000 + 13 * 60 * 1000 + 14 * 1000;
        result   = Parse::timestamp( text, time );
        REQUIRE( result == true );
        REQUIRE( time == expected );

        text   = "02-12:13:14";
        result = Parse::timestamp( text, time );
        REQUIRE( result == false );

        text   = "02-12:13:14.1234";
        result = Parse::timestamp( text, time );
        REQUIRE( result == false );

        text     = "49 12:13:14.5";
        expected = 49LL * 24 * 60 * 60 * 1000 + 12LL * 60 * 60 * 1000 + 13 * 60 * 1000 + 14 * 1000 + 500;
        result   = Parse::timestamp( text, time );
        REQUIRE( result == true );
        REQUIRE( time == expected );
        text   = "50 12:13:14.5";
        result = Parse::timestamp( text, time );
        REQUIRE( result == false );
    }

    SECTION( "bintobuffer" )
    {
        const char* text = "00101100101001011111000100011111";  // 0x2CA5F11F
        uint8_t     buffer[4];

        size_t result = Parse::asciiBinary( buffer, text, sizeof( buffer ), true );
        REQUIRE( result == 4 * 8 );
        REQUIRE( buffer[0] == 0x1F );
        REQUIRE( buffer[1] == 0xF1 );
        REQUIRE( buffer[2] == 0xA5 );
        REQUIRE( buffer[3] == 0x2C );

        result = Parse::asciiBinary( buffer, text, sizeof( buffer ) );
        REQUIRE( result == 4 * 8 );
        REQUIRE( buffer[0] == 0x2C );
        REQUIRE( buffer[1] == 0xA5 );
        REQUIRE( buffer[2] == 0xF1 );
        REQUIRE( buffer[3] == 0x1F );

        result = Parse::asciiBinary( buffer, 0, sizeof( buffer ) );
        REQUIRE( result == SIZE_MAX );
        result = Parse::asciiBinary( 0, text, sizeof( buffer ) );
        REQUIRE( result == SIZE_MAX );
        result = Parse::asciiBinary( buffer, text, 0 );
        REQUIRE( result == SIZE_MAX );

        result = Parse::asciiBinary( buffer, text, 1 );
        REQUIRE( result == 8 );
        REQUIRE( buffer[0] == 0x2C );

        uint8_t buffer2[5];
        result = Parse::asciiBinary( buffer2, text, sizeof( buffer2 ), true );
        REQUIRE( result == 4 * 8 );
        REQUIRE( buffer2[0] == 0x1F );
        REQUIRE( buffer2[1] == 0xF1 );
        REQUIRE( buffer2[2] == 0xA5 );
        REQUIRE( buffer2[3] == 0x2C );

        result = Parse::asciiBinary( buffer2, text, sizeof( buffer2 ) );
        REQUIRE( result == 4 * 8 );
        REQUIRE( buffer2[0] == 0x2C );
        REQUIRE( buffer2[1] == 0xA5 );
        REQUIRE( buffer2[2] == 0xF1 );
        REQUIRE( buffer2[3] == 0x1F );

        text   = "10101";  // 0xA8
        result = Parse::asciiBinary( buffer, text, sizeof( buffer ) );
        REQUIRE( result == 5 );
        REQUIRE( buffer[0] == 0xA8 );

        result = Parse::asciiBinary( buffer, text, sizeof( buffer ), true );
        REQUIRE( result == 5 );
        REQUIRE( buffer[0] == 0xA8 );

        text   = "100 1";  // error
        result = Parse::asciiBinary( buffer, text, sizeof( buffer ) );
        REQUIRE( result == SIZE_MAX );

        result = Parse::asciiBinary( buffer, text, sizeof( buffer ), true );
        REQUIRE( result == SIZE_MAX );

        text   = " 1001";  // error
        result = Parse::asciiBinary( buffer, text, sizeof( buffer ) );
        REQUIRE( result == SIZE_MAX );

        result = Parse::asciiBinary( buffer, text, sizeof( buffer ), true );
        REQUIRE( result == SIZE_MAX );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
