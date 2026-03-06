/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/EventQueue/Server.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "common.h"
#include "Kit/Text/FString.h"
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Dm/Mp/String.h"
#include <string.h>

#define STRCMP( s1, s2 ) ( strcmp( s1, s2 ) == 0 )
#define MAX_STR_LENG     1024
#define SECT_            "_0test"

#define MY_UUT_DATA_SIZE 8

#define INITIAL_VALUE    "Hello World"

using namespace Kit::Dm;
using namespace Kit::Dm::Mp;


////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static ModelDatabase modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static StringAllocate<MY_UUT_DATA_SIZE> mp_apple_( modelDb_, "APPLE" );
static StringAllocate<MY_UUT_DATA_SIZE> mp_orange_( modelDb_, "ORANGE", INITIAL_VALUE );

// Don't let the Runnable object go out of scope before its thread has actually terminated!
static Kit::EventQueue::Server t1Mbox_;


template <>
void Viewer<String, Kit::Text::FString<MY_UUT_DATA_SIZE>>::displayElement( const char* label, Kit::Text::FString<MY_UUT_DATA_SIZE>& elem )
{
    KIT_SYSTEM_TRACE_MSG( SECT_, "%s:%s", label, elem.getString() );
}

////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "String" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::Text::FString<MAX_STR_LENG>     errorMsg = "noerror";
    Kit::Text::FString<MY_UUT_DATA_SIZE> valStr;
    char                                 string[MAX_STR_LENG + 1];
    bool                                 truncated;
    bool                                 valid;
    mp_apple_.setInvalid();

    SECTION( "gets" )
    {
        // Gets...
        const char* name = mp_apple_.getName();
        REQUIRE( strcmp( name, "APPLE" ) == 0 );

        size_t s = mp_apple_.getSize();
        REQUIRE( s == MY_UUT_DATA_SIZE + 1 );

        s = mp_apple_.getExternalSize();
        REQUIRE( s == MY_UUT_DATA_SIZE + 1 + sizeof( bool ) );

        const char* mpType = mp_apple_.getTypeAsText();
        KIT_SYSTEM_TRACE_MSG( SECT_, "typeText: [%s]", mpType );
        REQUIRE( strcmp( mpType, "Kit::Dm::Mp::String" ) == 0 );
    }


    SECTION( "read/writes" )
    {
        mp_apple_.write( "12345678" );
        valid = mp_apple_.read( valStr );
        REQUIRE( valid );
        REQUIRE( valStr == "12345678" );
        mp_apple_.write( "bob" );
        valid = mp_apple_.read( valStr );
        REQUIRE( valid );
        REQUIRE( valStr == "bob" );

        valid = mp_orange_.read( valStr );
        REQUIRE( valid );
        REQUIRE( valStr == "Hello Wo" );

        mp_apple_.write( "Very Long String" );
        valid = mp_apple_.read( string, sizeof( string ) );
        REQUIRE( valid == true );
        REQUIRE( STRCMP( string, "Very Lon" ) );

        mp_apple_.read( string, 4 );
        REQUIRE( STRCMP( string, "Ver" ) );

        uint16_t seqNum  = mp_apple_.getSequenceNumber();
        uint16_t seqNum2 = mp_apple_.write( nullptr, 10, false );
        REQUIRE( seqNum == seqNum2 );
        mp_apple_.read( valStr );
        REQUIRE( valStr == "Very Lon" );
    }

    SECTION( "copy" )
    {
        mp_apple_.write( "charlie" );
        mp_orange_.copyFrom( mp_apple_ );
        valid = mp_orange_.read( valStr );
        REQUIRE( valid );
        REQUIRE( valStr == "charlie" );

        mp_apple_.setInvalid();
        mp_orange_.copyFrom( mp_apple_ );
        REQUIRE( mp_orange_.isNotValid() );
    }

    SECTION( "toJSON-pretty" )
    {
        mp_apple_.write( "Hi Bob" );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        KIT_SYSTEM_TRACE_MSG( SECT_, "toJSON: [%s]", string );

        StaticJsonDocument<1024> doc;
        DeserializationError     err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( doc["val"]["maxLen"] == MY_UUT_DATA_SIZE );
        REQUIRE( STRCMP( doc["val"]["text"], "Hi Bob" ) );
    }

    SECTION( "fromJSON" )
    {
        const char* json   = "{name:\"APPLE\", val:{text:\"good bye\"}}";
        bool        result = modelDb_.fromJSON( json );
        REQUIRE( result == true );
        valid = mp_apple_.read( valStr );
        REQUIRE( valid );
        REQUIRE( valStr == "good bye" );

        json   = "{name:\"APPLE\", val:true}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
    }

    SECTION( "observer" )
    {
        KIT_SYSTEM_TRACE_SCOPE( SECT_, "observer test" );
        Kit::Text::FString<MY_UUT_DATA_SIZE>                 expectedVal = "bob";
        Viewer<String, Kit::Text::FString<MY_UUT_DATA_SIZE>> viewer_apple1( t1Mbox_, Kit::System::Thread::getCurrent(), mp_apple_, expectedVal );
        Kit::System::Thread*                                 t1 = Kit::System::Thread::create( t1Mbox_, "T1" );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        viewer_apple1.open();
        mp_apple_.write( expectedVal );
        Kit::System::Thread::wait();
        viewer_apple1.close();

        // Shutdown threads
        t1Mbox_.pleaseStop();
        Kit::System::sleep( 100 );  // allow time for threads to stop
        REQUIRE( t1->isActive() == false );
        WAIT_FOR_THREAD_TO_STOP( t1 );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
