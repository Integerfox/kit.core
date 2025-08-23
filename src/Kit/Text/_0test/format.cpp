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
#include "Kit/System/Trace.h"
#include "Kit/Text/Format.h"
#include "Kit/Text/FString.h"

#define SECT_ "_0test"

///
using namespace Kit::Text;
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Format" )
{
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "string" )
    {
        uint8_t      buffer[5] = { 0, 0xAA, 0x55, 0xF0, 0x31 };
        FString<128> s1;
        FString<3>   s2;

        bool result = Format::string( buffer, sizeof( buffer ), s1 );
        REQUIRE( s1 == "..U.1" );
        REQUIRE( result == true );

        s1 = "buffer=";
        Format::string( buffer, sizeof( buffer ), s1, true );
        REQUIRE( s1 == "buffer=..U.1" );

        s1 = "no change";
        Format::string( 0, sizeof( buffer ), s1 );
        REQUIRE( s1 == "no change" );
        Format::string( buffer, 0, s1 );
        REQUIRE( s1 == "no change" );
        result = Format::string( buffer, sizeof( buffer ), s2 );
        REQUIRE( result == false );
    }

    SECTION( "ASCIIHex" )
    {
        uint8_t      buffer[5] = { 0, 0xAA, 0x55, 0xF0, 0x31 };
        FString<64> s1;
        FString<3>   s2;

        bool result = Format::asciiHex( buffer, sizeof( buffer ), s1 );
        REQUIRE( s1 == "00AA55F031" );
        REQUIRE( result == true );
        Format::asciiHex( buffer, sizeof( buffer ), s1, false );
        REQUIRE( s1 == "00aa55f031" );


        s1 = "buffer=";
        Format::asciiHex( buffer, sizeof( buffer ), s1, true, true );
        REQUIRE( s1 == "buffer=00AA55F031" );
        s1 = "buffer=";
        Format::asciiHex( buffer, sizeof( buffer ), s1, true, true, ' ' );
        REQUIRE( s1 == "buffer=00 AA 55 F0 31" );

        s1 = "no change";
        Format::asciiHex( 0, sizeof( buffer ), s1 );
        REQUIRE( s1 == "no change" );
        Format::asciiHex( buffer, 0, s1 );
        REQUIRE( s1 == "no change" );
        result = Format::asciiHex( buffer, sizeof( buffer ), s2 );
        REQUIRE( result == false );
    }

    SECTION( "ASCIIBinary" )
    {
        uint8_t buffer[5] = { 0x84, 0x4a, 0, 0, 0x01 };

        FString<128> s1;
        FString<3>   s2;

        bool result = Format::asciiBinary( buffer, 2, s1 );
        REQUIRE( s1 == "1000010001001010" );
        REQUIRE( result == true );
        Format::asciiBinary( buffer, 2, s1, false, true );
        REQUIRE( s1 == "0100101010000100" );


        s1 = "buffer=";
        Format::asciiBinary( buffer, 2, s1, true, true );
        REQUIRE( s1 == "buffer=0100101010000100" );

        s1 = "no change";
        Format::asciiBinary( 0, sizeof( buffer ), s1 );
        REQUIRE( s1 == "no change" );
        Format::asciiBinary( buffer, 0, s1 );
        REQUIRE( s1 == "no change" );
        result = Format::asciiBinary( buffer, sizeof( buffer ), s2 );
        REQUIRE( result == false );
    }

    SECTION( "Viewer" )
    {
        uint8_t      buffer[5] = { 0, 0xAA, 0x55, 0xF0, 0x31 };
        FString<64> s1;
        FString<3>   s2;
        bool result = Format::viewer( buffer, sizeof( buffer ), s1, sizeof( buffer ) );
        REQUIRE( s1 == "00AA55F031    ..U.1" );
        REQUIRE( result == true );
        result = Format::viewer( buffer, sizeof( buffer ), s1, sizeof( buffer ), "    ", false );
        REQUIRE( result == true );
        REQUIRE( s1 == "00aa55f031    ..U.1" );
        result = Format::viewer( buffer, sizeof( buffer ), s1, sizeof( buffer ), "============================================================");
        REQUIRE( result == false );


        s1 = "buffer=";
        Format::viewer( buffer, sizeof( buffer ), s1, sizeof( buffer ), " -- ", true, true );
        REQUIRE( s1 == "buffer=00AA55F031 -- ..U.1" );

        result = Format::viewer( buffer, 4, s1, 5 );
        KIT_SYSTEM_TRACE_MSG( SECT_,  "act=[%s], exp=[A55F0      ..U.]", s1.getString() );
        REQUIRE( s1 == "00AA55F0      ..U." );
        REQUIRE( result == true );


        s1 = "no change";
        Format::viewer( 0, sizeof( buffer ), s1 );
        REQUIRE( s1 == "no change" );
        Format::viewer( buffer, 0, s1 );
        REQUIRE( s1 == "no change" );
        result = Format::viewer( buffer, sizeof( buffer ), s2 );
        REQUIRE( result == false );
    }

    SECTION( "timestamp-32" )
    {
        FString<128> s1;
        FString<5>   s2;

        uint32_t timestamp = 10983003;  // 3 hours, 3 minutes, 3 seconds, 3msec
        bool     result    = Format::timestamp( s1, timestamp );
        REQUIRE( result == true );
        REQUIRE( s1 == "00 03:03:03.003" );
        result = Format::timestamp( s1, timestamp, false );
        REQUIRE( result == true );
        REQUIRE( s1 == "00 03:03:03" );
        result = Format::timestamp( s1, timestamp, false, false );
        REQUIRE( result == true );
        REQUIRE( s1 == "03:03:03" );
        s1     = "T:";
        result = Format::timestamp( s1, timestamp, true, true, true );
        REQUIRE( result == true );
        REQUIRE( s1 == "T:00 03:03:03.003" );
        s1     = "t:";
        result = Format::timestamp( s1, timestamp, false, true, true );
        REQUIRE( result == true );
        REQUIRE( s1 == "t:00 03:03:03" );
        s1     = "::";
        result = Format::timestamp( s1, timestamp, true, false, true );
        REQUIRE( result == true );
        REQUIRE( s1 == "::03:03:03.003" );

        uint64_t t2 = 270183003;  // 3 days, 3 hours, 3 minutes, 3 seconds,
        result      = Format::timestamp( s1, t2 );
        REQUIRE( result == true );
        REQUIRE( s1 == "03 03:03:03.003" );
        s1     = "T:";
        result = Format::timestamp( s1, t2, true, true, true );
        REQUIRE( result == true );
        REQUIRE( s1 == "T:03 03:03:03.003" );
        s1     = "t:";
        result = Format::timestamp( s1, t2, false, true, true );
        REQUIRE( result == true );
        REQUIRE( s1 == "t:03 03:03:03" );
        s1     = "::";
        result = Format::timestamp( s1, t2, true, false, true );
        REQUIRE( result == true );
        REQUIRE( s1 == "::75:03:03.003" );

        s2 = "hi";
        result = Format::timestamp( s2, t2, true, false, true );
        REQUIRE( result == false );
        REQUIRE( s2 == "hi75:" );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
