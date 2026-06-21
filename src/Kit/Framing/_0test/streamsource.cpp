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
#include "Kit/Framing/StreamSource.h"
#include "Kit/Io/Ram/InputOutputAllocate.h"
#include "Kit/System/Trace.h"
#include <string.h>

#define SECT_ "_0test"

///
using namespace Kit::System;
using namespace Kit::Framing;


////////////////////////////////////
TEST_CASE( "StreamSource" )
{
    ShutdownUnitTesting::clearAndUseCounter();
    Kit::Io::Ram::InputOutputAllocate<64> src; // Must be large enough to hold the test string TWICE
    StreamSource                          uut( src );
    char                                  buffer[14];
    Kit::Type::SSize_T                    bytesRead;

    src.write( "Hello Kitty!" );
    REQUIRE( uut.read( buffer, 5, bytesRead ) == true );
    REQUIRE( bytesRead == 5 );
    REQUIRE( strncmp( buffer, "Hello", 5 ) == 0 );
    REQUIRE( uut.read( nullptr, 5, bytesRead ) == false );
    REQUIRE( bytesRead == 0 );
    REQUIRE( uut.read( buffer, sizeof( buffer ), bytesRead ) == true );
    REQUIRE( bytesRead == 7 );
    REQUIRE( strncmp( buffer, " Kitty!", 7 ) == 0 );
    REQUIRE( uut.read( buffer, sizeof( buffer ), bytesRead ) == false );
    REQUIRE( bytesRead == 0 );

    StreamSource uut2;
    memset( buffer, 0, sizeof( buffer ) );
    REQUIRE( uut2.read( buffer, 5, bytesRead ) == false );
    REQUIRE( bytesRead == 0 );
    uut2.setInput( src );
    src.write( "Hello Kitty!" );
    REQUIRE( uut2.read( buffer, 5, bytesRead ) == true );
    REQUIRE( bytesRead == 5 );
    REQUIRE( strncmp( buffer, "Hello", 5 ) == 0 );
    memset( buffer, 0, sizeof( buffer ) );
    REQUIRE( uut2.read( buffer, sizeof( buffer ), bytesRead ) == true );
    REQUIRE( bytesRead == 7 );
    REQUIRE( strncmp( buffer, " Kitty!", 7 ) == 0 );
    REQUIRE( uut2.read( buffer, sizeof( buffer ), bytesRead ) == false );
    REQUIRE( bytesRead == 0 );

    src.close();
    REQUIRE( uut2.read( buffer, sizeof( buffer ), bytesRead ) == false );

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
