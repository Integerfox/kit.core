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
#include "common.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/Text/FString.h"
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Dm/Mp/Bool.h"
#include <string.h>

#define STRCMP(s1,s2)       (strcmp(s1,s2)==0)
#define MAX_STR_LENG        1024
#define SECT_               "_0test"

#define INITIAL_VALUE       true


////////////////////////////////////////////////////////////////////////////////
using namespace Kit::Dm;

// Allocate/create my Model Database
static ModelDatabase    modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static Mp::Bool       mp_apple_( modelDb_, "APPLE" );
static Mp::Bool       mp_orange_( modelDb_, "ORANGE", INITIAL_VALUE );


// Don't let the Runnable object go out of scope before its thread has actually terminated!
static Kit::EventQueue::Server  t1Mbox_;


////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "Bool" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::Text::FString<MAX_STR_LENG> errorMsg = "noerror";
    char string[MAX_STR_LENG + 1];
    bool truncated;
    bool valid;
    bool value;
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
        KIT_SYSTEM_TRACE_MSG( SECT_, "typeText: [%s]", mpType) ;
        REQUIRE( strcmp( mpType, "Kit::Dm::Mp::Bool" ) == 0 );
    }

    SECTION( "read/writes" )
    {
        mp_apple_.write( true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == true );
        mp_apple_.write( false );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == false );

        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == INITIAL_VALUE );
    }

    SECTION( "copy" )
    {
        mp_apple_.write( true );
        mp_orange_.copyFrom( mp_apple_ );
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == true );

        mp_apple_.setInvalid();
        mp_orange_.copyFrom( mp_apple_ );
        REQUIRE( mp_orange_.isNotValid() );
    }

    SECTION( "toJSON-pretty" )
    {
        mp_apple_.write( true );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        KIT_SYSTEM_TRACE_MSG( SECT_, "toJSON: [%s]", string );

        StaticJsonDocument<1024> doc;
        DeserializationError err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( doc["val"] == true );
    }

    SECTION( "fromJSON" )
    {
        const char* json = "{name:\"APPLE\", val:false}";
        bool result = modelDb_.fromJSON( json );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == false );

        json = "{name:\"APPLE\", val:\"true\"}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
    }

    SECTION( "observer" )
    {
        KIT_SYSTEM_TRACE_SCOPE( SECT_, "observer test" );
        bool expectedVal = true;
        Viewer<Mp::Bool,bool>     viewer_apple1( t1Mbox_, Kit::System::Thread::getCurrent(), mp_apple_, expectedVal );
        Kit::System::Thread* t1 = Kit::System::Thread::create( t1Mbox_, "T1" );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        viewer_apple1.open();
        mp_apple_.write( true );
        Kit::System::Thread::wait();
        viewer_apple1.close();

        // Shutdown threads
        t1Mbox_.pleaseStop();
        WAIT_FOR_THREAD_TO_STOP( t1 );
        Kit::System::Thread::destroy( *t1 );
    }


    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
