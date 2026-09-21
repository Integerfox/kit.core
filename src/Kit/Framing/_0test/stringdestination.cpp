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
#include "Kit/Framing/StringDestination.h"
#include "Kit/Text/FString.h"
#include "Kit/System/Trace.h"
#include <string.h>

#define SECT_ "_0test"

///
using namespace Kit::System;
using namespace Kit::Framing;


////////////////////////////////////
TEST_CASE( "StringDestination" )
{
    ShutdownUnitTesting::clearAndUseCounter();
    Kit::Text::FString<32> dst;
    StringDestination      uut( dst );

    REQUIRE( uut.startOutput() == true );
    REQUIRE( dst.isEmpty() == true );
    REQUIRE( uut.appendOutput( "Hello", 5 ) == true );
    REQUIRE( uut.appendOutput( " Kitty!", 7 ) == true );
    REQUIRE( uut.endOutput() == true );
    REQUIRE( strcmp( dst.getString(), "Hello Kitty!" ) == 0 );

    // startOutput() clears any previous content
    REQUIRE( uut.startOutput() == true );
    REQUIRE( dst.isEmpty() == true );
    REQUIRE( uut.appendOutput( "Again", 5 ) == true );
    REQUIRE( uut.endOutput() == true );
    REQUIRE( strcmp( dst.getString(), "Again" ) == 0 );

    // A nullptr source buffer fails the append
    REQUIRE( uut.appendOutput( nullptr, 5 ) == false );

    // Truncation (buffer too small) is reported as a failed append
    Kit::Text::FString<4> tiny;
    StringDestination      uutTrunc( tiny );
    REQUIRE( uutTrunc.startOutput() == true );
    REQUIRE( uutTrunc.appendOutput( "TooLongForTheBuffer", 19 ) == false );
    REQUIRE( uutTrunc.endOutput() == true );

    // Default constructed instance has no output string set
    StringDestination uut2;
    REQUIRE( uut2.startOutput() == false );
    REQUIRE( uut2.appendOutput( "Kitty!", 6 ) == false );
    REQUIRE( uut2.endOutput() == false );

    // setOutput() attaches a destination string after construction
    uut2.setOutput( dst );
    REQUIRE( uut2.startOutput() == true );
    REQUIRE( uut2.appendOutput( "Meow", 4 ) == true );
    REQUIRE( uut2.endOutput() == true );
    REQUIRE( strcmp( dst.getString(), "Meow" ) == 0 );

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
