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
#include "Kit/Framing/StringSource.h"
#include "Kit/System/Trace.h"
#include <string.h>

#define SECT_ "_0test"

///
using namespace Kit::System;
using namespace Kit::Framing;


////////////////////////////////////
TEST_CASE( "StringSource" )
{
    ShutdownUnitTesting::clearAndUseCounter();
    const char*                           testString = "Hello Kitty!";
    StringSource                          uut( testString);
    char                                  buffer[14];
    Kit::Type::SSize_T                    bytesRead;

    REQUIRE( uut.read( buffer, 5, bytesRead ) == true );
    REQUIRE( bytesRead == 5 );
    REQUIRE( strncmp( buffer, "Hello", 5 ) == 0 );
    REQUIRE( uut.read( nullptr, 5, bytesRead ) == false );
    REQUIRE( bytesRead == 0 );
    REQUIRE( uut.read( buffer, sizeof( buffer ), bytesRead ) == true );
    REQUIRE( bytesRead == 7 );
    REQUIRE( strcmp( buffer, " Kitty!" ) == 0 );
    REQUIRE( uut.read( buffer, sizeof( buffer ), bytesRead ) == false );
    REQUIRE( bytesRead == 0 );

    StringSource uut2;
    memset( buffer, 0, sizeof( buffer ) );
    REQUIRE( uut2.read( buffer, 5, bytesRead ) == false );
    REQUIRE( bytesRead == 0 );
    uut2.setInput( testString );
    REQUIRE( uut2.read( buffer, 5, bytesRead ) == true );
    REQUIRE( bytesRead == 5 );
    REQUIRE( strncmp( buffer, "Hello", 5 ) == 0 );
    memset( buffer, 0, sizeof( buffer ) );
    REQUIRE( uut2.read( buffer, sizeof( buffer ), bytesRead ) == true );
    REQUIRE( bytesRead == 7 );
    REQUIRE( strcmp( buffer, " Kitty!" ) == 0 );
    REQUIRE( uut2.read( buffer, sizeof( buffer ), bytesRead ) == false );
    REQUIRE( bytesRead == 0 );

    StringSource uut3( nullptr );
    REQUIRE( uut2.read( buffer, 5, bytesRead ) == false );
    REQUIRE( bytesRead == 0 );
    uut3.setInput( nullptr );
    REQUIRE( uut2.read( buffer, 5, bytesRead ) == false );
    REQUIRE( bytesRead == 0 );

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
