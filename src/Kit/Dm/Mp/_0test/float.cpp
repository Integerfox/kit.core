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
#include "Kit/System/Api.h"
#include "Kit/Text/FString.h"
#include "Kit/Text/DString.h"
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Dm/Mp/Float.h"
#include <string.h>

#define STRCMP(s1,s2)       (strcmp(s1,s2)==0)
#define MAX_STR_LENG        1024
#define SECT_               "_0test"

#define MAGIC_VALUE         3.14F
#define MAGIC_INC           0.0001F


////////////////////////////////////////////////////////////////////////////////
using namespace Kit::Dm;
using namespace Kit::Dm::Mp;

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Float       mp_apple_( modelDb_, "APPLE" );
static Float       mp_orange_( modelDb_, "ORANGE", MAGIC_VALUE );

////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "Float" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    char    string[MAX_STR_LENG + 1];
    bool    truncated;
    bool    valid;
    float value;

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
        REQUIRE( strcmp( mpType, "Kit::Dm::Mp::Float" ) == 0 );
    }


    SECTION( "read/writes/compare" )
    {
        // Start with MP in the invalid state
        mp_apple_.setInvalid();

        mp_apple_.increment( 2 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 2 );  // By design the invalid MP has a 'data value' of zero
        uint16_t seqNum = mp_apple_.write( MAGIC_VALUE );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( Kit::Math::areFloatsEqual( value, MAGIC_VALUE) );
        uint16_t seqNum2 = mp_apple_.increment( MAGIC_INC );
        mp_apple_.read( value );
        REQUIRE( Kit::Math::areFloatsEqual( value, MAGIC_VALUE+ MAGIC_INC ) );
        REQUIRE( seqNum + 1 == seqNum2 );

        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( Kit::Math::areFloatsEqual( value, MAGIC_VALUE ) );
    }

    SECTION( "toJSON-pretty" )
    {
        mp_apple_.write( 127.03F );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        KIT_SYSTEM_TRACE_MSG( SECT_, "toJSON: [%s]", string );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( Kit::Math::areFloatsEqual( doc["val"], 127.03F ) );
    }

    SECTION( "fromJSON" )
    {
        // Start with MP in the invalid state
        mp_apple_.setInvalid();

        const char* json = "{name:\"APPLE\", val:1234.042}";
        bool result = modelDb_.fromJSON( json );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( Kit::Math::areFloatsEqual( value, 1234.042F ) );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
