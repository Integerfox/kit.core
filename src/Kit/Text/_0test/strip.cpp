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
#include "Kit/Text/strapi.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/Text/Strip.h"
#include <string.h>


///
using namespace Kit::Text;
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Strip" )
{

    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "space" )
    {
        const char* input = "  hello  world, \t bobs your uncle ";
        const char* ptr;

        ptr = Strip::space( input );
        REQUIRE( strcmp( ptr, "hello  world, \t bobs your uncle " ) == 0 );
        ptr = Strip::space( Strip::notSpace( ptr ) );
        REQUIRE( strcmp( ptr, "world, \t bobs your uncle " ) == 0 );
        ptr = Strip::space( Strip::notSpace( ptr ) );
        REQUIRE( strcmp( ptr, "bobs your uncle " ) == 0 );
        ptr = Strip::space( Strip::notSpace( ptr ) );
        REQUIRE( strcmp( ptr, "your uncle " ) == 0 );
        ptr = Strip::space( Strip::notSpace( ptr ) );
        REQUIRE( strcmp( ptr, "uncle " ) == 0 );
        ptr = Strip::space( Strip::notSpace( ptr ) );
        REQUIRE( strcmp( ptr, "" ) == 0 );

        REQUIRE( Strip::space( nullptr ) == nullptr );
        REQUIRE( Strip::space( Strip::notSpace( nullptr ) ) == nullptr );
    }

    SECTION( "chars" )
    {
        const char* input = "::hello,world,\t bobs your . uncle, ";
        const char* ptr;

        ptr = Strip::chars( input, ":.," );
        REQUIRE( strcmp( ptr, "hello,world,\t bobs your . uncle, " ) == 0 );
        ptr = Strip::chars( Strip::notChars( ptr, ":.," ), ":.," );
        REQUIRE( strcmp( ptr, "world,\t bobs your . uncle, " ) == 0 );
        ptr = Strip::chars( Strip::notChars( ptr, ":.," ), ":.," );
        REQUIRE( strcmp( ptr, "\t bobs your . uncle, " ) == 0 );
        ptr = Strip::chars( Strip::notChars( ptr, ":.," ), ":.," );
        REQUIRE( strcmp( ptr, " uncle, " ) == 0 );
        ptr = Strip::chars( Strip::notChars( ptr, ":.," ), ":.," );
        REQUIRE( strcmp( ptr, " " ) == 0 );
        ptr = Strip::chars( Strip::notChars( ptr, ":.," ), ":.," );
        REQUIRE( strcmp( ptr, "" ) == 0 );

        REQUIRE( Strip::chars( nullptr, ":.," ) == nullptr );
        ptr = Strip::chars( input, nullptr );
        REQUIRE( strcmp( ptr, input ) == 0 );
        REQUIRE( Strip::notChars( nullptr, ":.," ) == nullptr );
        ptr = Strip::notChars( input, nullptr );
        REQUIRE( strcmp( ptr, input ) == 0 );
    }

    SECTION( "trailing-space" )
    {
        char        input[] = " hello  world   x\t";
        const char* ptr;
        ptr = Strip::trailingSpace( input );
        REQUIRE( *ptr == 'x' );
        Strip::removeTrailingSpace( input );
        REQUIRE( strcmp( input, " hello  world   x" ) == 0 );

        input[strlen( input ) - 1] = '\0';  // Remove the last character
        ptr                        = Strip::trailingSpace( input );
        REQUIRE( *ptr == 'd' );
        Strip::removeTrailingSpace( input );
        REQUIRE( strcmp( input, " hello  world" ) == 0 );

        REQUIRE( Strip::trailingSpace( nullptr ) == nullptr );
        Strip::removeTrailingSpace( nullptr );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
        memcpy( input, "   ", 3 );
        input[3] = '\0';  // Make it a null-terminated string
        REQUIRE( *input == ' ' );
        Strip::removeTrailingSpace( input );
        REQUIRE( *input == '\0' );

        const char* input2 = "    ";
        ptr                = Strip::trailingSpace( input2 );
        REQUIRE( ptr == input2 );
        input2 = "";
        ptr    = Strip::trailingSpace( input2 );
        REQUIRE( ptr == input2 );
    }

    SECTION( "trailing-char" )
    {
        char        input[] = " hello  world ;.\t.";
        const char* ptr;
        ptr = Strip::trailingChars( input, ";." );
        REQUIRE( *ptr == '\t' );
        Strip::removeTrailingChars( input, ";." );
        REQUIRE( strcmp( input, " hello  world ;.\t" ) == 0 );

        input[strlen( input ) - 1] = '\0';  // Remove the last character
        ptr                        = Strip::trailingChars( input, ";." );
        REQUIRE( *ptr == ' ' );
        Strip::removeTrailingChars( input, ";." );
        REQUIRE( strcmp( input, " hello  world " ) == 0 );

        REQUIRE( Strip::trailingChars( nullptr, ";." ) == nullptr );
        REQUIRE( Strip::trailingChars( input, nullptr ) == input );
        Strip::removeTrailingChars( nullptr, ";." );
        Strip::removeTrailingChars( input, nullptr );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );

        const char* input2 = "xyz";
        ptr                = Strip::trailingChars( input2, input2 );
        REQUIRE( ptr == input2 );
        const char* input3 = "";
        ptr                = Strip::trailingChars( input3, input2 );
        REQUIRE( ptr == input3 );

        memcpy( input, "xyz", 3 );
        input[3] = '\0';  // Make it a null-terminated string
        REQUIRE( *input == 'x' );
        Strip::removeTrailingChars( input, "zxy" );
        REQUIRE( *input == '\0' );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
