/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Dm/Transaction/FooT.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/Dm/Mp/_0test/common.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Text/FString.h"
#include "Dm/Transaction/MpFoo.h"
#include <string.h>

using namespace Dm::Transaction;

#define STRCMP( s1, s2 ) ( strcmp( s1, s2 ) == 0 )
#define MAX_STR_LENG     1024
#define SECT_            "_0test"

static const Foo_T INITIAL_VALUE = { 12, 34, 56, true };


////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static Kit::Dm::ModelDatabase modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static MpFoo mp_apple_( modelDb_, "APPLE" );
static MpFoo mp_orange_( modelDb_, "ORANGE", INITIAL_VALUE );


// Don't let the Runnable object go out of scope before its thread has actually terminated!
static Kit::EventQueue::Server t1Mbox_;

template <>
void Viewer<MpFoo, Foo_T>::displayElement( const char* label, Foo_T& elem )
{
    KIT_SYSTEM_TRACE_MSG( SECT_, "%s:u=%d, l=%d, r=%d, v=%d", label, elem.upperLimit, elem.lowerLimit, elem.randomNumber, elem.isValid );
}

////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to concrete MP type
//
TEST_CASE( "MpFoo" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::Text::FString<MAX_STR_LENG> errorMsg = "noerror";
    char                             string[MAX_STR_LENG + 1];
    bool                             truncated;
    bool                             valid;
    Foo_T                            value;
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
        REQUIRE( strcmp( mpType, "Dm::Transaction::Mp::MpFoo" ) == 0 );
    }

    SECTION( "read/writes" )
    {
        Foo_T expected = { 12, 34, 56, true };
        mp_apple_.write( expected );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == expected );
        expected.isValid = false;
        mp_apple_.write( expected );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == expected );

        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == INITIAL_VALUE );
    }

    SECTION( "copy" )
    {
        Foo_T expected = { 112, 134, 156, false };
        mp_apple_.write( expected );
        mp_orange_.copyFrom( mp_apple_ );
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == expected );

        mp_apple_.setInvalid();
        mp_orange_.copyFrom( mp_apple_ );
        REQUIRE( mp_orange_.isNotValid() );
    }

    SECTION( "toJSON-pretty" )
    {
        Foo_T expected = { 12, 34, 56, true };
        mp_apple_.write( expected );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        KIT_SYSTEM_TRACE_MSG( SECT_, "toJSON: [%s]", string );

        StaticJsonDocument<1024> doc;
        DeserializationError     err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( doc["val"]["isValid"] == true );
        REQUIRE( doc["val"]["upper"] == 12 );
        REQUIRE( doc["val"]["lower"] == 34 );
        REQUIRE( doc["val"]["result"] == 56 );
    }

    SECTION( "fromJSON" )
    {
        Foo_T expected = { 12, 34, 56, true };
        const char* json   = "{name:\"APPLE\", val:{upper:12, lower:34, result:56, isValid:true}}";
        bool        result = modelDb_.fromJSON( json );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == expected );

        json   = "{name:\"APPLE\", val:\"true\"}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == true );  // Does not throw an error by design - but the MP was not updated
        REQUIRE( errorMsg == "noerror" );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == expected );

        json   = "{name:\"APPLE\", val:{upper:\"bob\"}}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == true );  // Does not throw an error by design - but the MP was not updated
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == expected );
    }

    SECTION( "observer" )
    {
        KIT_SYSTEM_TRACE_SCOPE( SECT_, "observer test" );
        Foo_T expectedVal = { 212, 234, 256, true };
        Viewer<MpFoo, Foo_T> viewer_apple1( t1Mbox_, Kit::System::Thread::getCurrent(), mp_apple_, expectedVal );
        Kit::System::Thread*   t1 = Kit::System::Thread::create( t1Mbox_, "T1" );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        viewer_apple1.open();
        mp_apple_.write( expectedVal );
        Kit::System::Thread::wait();
        viewer_apple1.close();

        // Shutdown threads
        t1Mbox_.pleaseStop();
        WAIT_FOR_THREAD_TO_STOP( t1 );
        Kit::System::Thread::destroy( *t1 );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
