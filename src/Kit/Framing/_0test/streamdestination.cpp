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
#include "Kit/Framing/StreamDestination.h"
#include "Kit/Io/Ram/InputOutputAllocate.h"
#include "Kit/System/Trace.h"
#include <string.h>

#define SECT_ "_0test"

///
using namespace Kit::System;
using namespace Kit::Framing;


////////////////////////////////////
TEST_CASE( "StreamDestination" )
{
    ShutdownUnitTesting::clearAndUseCounter();
    Kit::Io::Ram::InputOutputAllocate<20> dst;
    StreamDestination                     uut( dst );
    char                                  buffer[10];
    Kit::Type::SSize_T                    bytesRead;

    REQUIRE( uut.getStream() == &dst );
    REQUIRE( uut.startOutput() == true );
    REQUIRE( uut.appendOutput( "Hello", 5 ) == true );
    REQUIRE( dst.available() == true );
    REQUIRE( dst.read( buffer, 5, bytesRead ) == true );
    REQUIRE( bytesRead == 5 );
    REQUIRE( strncmp( buffer, "Hello", 5 ) == 0 );
    REQUIRE( uut.endOutput() == true );

    REQUIRE( uut.startOutput() == true );
    REQUIRE( uut.appendOutput( "Kitty!", 6 ) == true );
    REQUIRE( uut.appendOutput( nullptr, 6 ) == false );
    REQUIRE( dst.available() == true );
    REQUIRE( dst.read( buffer, 6, bytesRead ) == true );
    REQUIRE( bytesRead == 6 );
    REQUIRE( strncmp( buffer, "Kitty!", 6 ) == 0 );
    REQUIRE( uut.endOutput() == true );

    dst.close();
    REQUIRE( uut.startOutput() == true );
    REQUIRE( uut.appendOutput( "Kitty!", 6 ) == false );
    REQUIRE( uut.endOutput() == true );

    StreamDestination uut2;
    REQUIRE( uut2.getStream() == nullptr );
    REQUIRE( uut2.startOutput() == false );
    REQUIRE( uut2.appendOutput( "Kitty!", 6 ) == false );
    REQUIRE( uut2.endOutput() == false );

    StreamDestination uut3;
    uut3.setOutput( dst );
    REQUIRE( uut3.getStream() == &dst );
    REQUIRE( uut3.startOutput() == true );
    REQUIRE( uut3.appendOutput( "Kitty!", 6 ) == false );
    REQUIRE( uut3.endOutput() == true );

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
