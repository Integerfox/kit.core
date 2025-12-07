/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "catch2/catch_test_macros.hpp"
#include "Kit/Type/Guid.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"


/// 
using namespace Kit::Type;
using namespace Kit::Text;
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "guid" )
{
    ShutdownUnitTesting::clearAndUseCounter();

    Kit::Text::FString<KIT_TYPE_GUID_MAX_FORMATTED_WITH_BRACES_LENGTH> stringBuffer;
    Guid_T guid ={{{ 0x12, 0x3e, 0x45, 0x67, 0xe8, 0x9b, 0x12, 0xd3, 0xa4, 0x56, 0x42, 0x66, 0x14, 0x17, 0x40, 0x00 }}};
    bool result = guid.toString( stringBuffer );
    REQUIRE( result == true );
    REQUIRE( stringBuffer == "123e4567-e89b-12d3-a456-426614174000" );
    result = guid.toString( stringBuffer, true );
    REQUIRE( result == true );
    REQUIRE( stringBuffer == "{123e4567-e89b-12d3-a456-426614174000}" );

    Kit::Text::FString<KIT_TYPE_GUID_MAX_FORMATTED_LENGTH - 1> tooSmall;
    result = guid.toString( tooSmall );
    REQUIRE( result == false );
    result = guid.toString( tooSmall, true );
    REQUIRE( result == false );

    Guid_T guidOut;
    result = guidOut.fromString( "123e4567-e89b-12d3-a456-426614174000" );
    REQUIRE( result == true );
    REQUIRE( guid == guidOut );
    result = guidOut.fromString( "{123e4567-e89b-12d3-a456-426614174000}" );
    REQUIRE( result == true );
    REQUIRE( guid == guidOut );

    result = guidOut.fromString( "123e4567e89b-12d3-a456-426614174000}" );
    REQUIRE( result == false );
    result = guidOut.fromString( "{123e4567-e89b-12d3-a456-426614174000" );
    REQUIRE( result == false );
    result = guidOut.fromString( "123e4567e89b-12d3-a456-426614174000}" );
    REQUIRE( result == false );
    result = guidOut.fromString( 0 );
    REQUIRE( result == false );

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
