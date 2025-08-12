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

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
