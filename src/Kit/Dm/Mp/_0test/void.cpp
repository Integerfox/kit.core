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
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Dm/Mp/Void.h"
#include <string.h>

#define STRCMP( s1, s2 ) ( strcmp( s1, s2 ) == 0 )
#define MAX_STR_LENG     1024
#define SECT_            "_0test"

#define INITIAL_VALUE    ((void*)42)

////////////////////////////////////////////////////////////////////////////////
using namespace Kit::Dm;

// Allocate/create my Model Database
static ModelDatabase modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Mp::Void mp_apple_( modelDb_, "APPLE" );
static Mp::Void mp_orange_( modelDb_, "ORANGE", INITIAL_VALUE );

////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "Void" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    char    string[MAX_STR_LENG + 1];
    bool    truncated;
    bool    valid;
    void*   value;
    mp_apple_.setInvalid();

    SECTION( "gets" )
    {
        // Gets...
        const char* name = mp_apple_.getName();
        REQUIRE( strcmp( name, "APPLE" ) == 0 );

        size_t s = mp_apple_.getSize();
        REQUIRE( s == sizeof( value ) );

        s = mp_apple_.getExternalSize();
        REQUIRE( s == sizeof( value ) + sizeof( bool ) );

        const char* mpType = mp_apple_.getTypeAsText();
        KIT_SYSTEM_TRACE_MSG( SECT_, "typeText: [%s]", mpType );
        REQUIRE( strcmp( mpType, "Kit::Dm::Mp::Void*" ) == 0 );
    }


    SECTION( "read/writes" )
    {
        mp_apple_.write( (void*)2 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == (void*)2 ); 
        mp_apple_.write( (void*)1 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == (void*)1 );

        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == INITIAL_VALUE );
    }

    SECTION( "toJSON-pretty" )
    {
        mp_apple_.write( (void*)0x7F00 );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        KIT_SYSTEM_TRACE_MSG( SECT_, "toJSON: [%s]", string );

        StaticJsonDocument<1024> doc;
        DeserializationError     err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( STRCMP( doc["val"], "0x7F00" ) );
    }

    SECTION( "fromJSON" )
    {
        // Start with MP in the invalid state
        mp_apple_.setInvalid();

        const char* json   = "{name:\"APPLE\", val:\"0xA234\"}";
        bool        result = modelDb_.fromJSON( json );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == (void*)0xA234 );
    }

        SECTION( "fromJSON2" )
    {
        // Start with MP in the invalid state
        mp_apple_.setInvalid();

        const char* json   = "{name:\"APPLE\", val:234}";
        bool        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
