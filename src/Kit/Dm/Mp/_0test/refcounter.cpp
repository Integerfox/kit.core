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
#include "Kit/Text/FString.h"
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Dm/Mp/RefCounter.h"
#include "Kit/EventQueue/Server.h"
#include "common.h"
#include <cstdint>
#include <string.h>

#define STRCMP( s1, s2 ) ( strcmp( s1, s2 ) == 0 )
#define MAX_STR_LENG     1024
#define SECT_            "_0test"

#define INITIAL_VALUE    32

using namespace Kit::Dm;
using namespace Kit::Dm::Mp;


////////////////////////////////////////////////////////////////////////////////

// Allocate/create my Model Database
static ModelDatabase modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );

// Allocate my Model Points
static RefCounter mp_apple_( modelDb_, "APPLE" );
static RefCounter mp_orange_( modelDb_, "ORANGE", INITIAL_VALUE );

// Don't let the Runnable object go out of scope before its thread has actually terminated!
static Kit::EventQueue::Server t1Mbox_;

template <>
void Viewer<RefCounter, uint32_t>::displayElement( const char* label, uint32_t& elem )
{
    KIT_SYSTEM_TRACE_MSG( SECT_, "%s:%" PRIu32, label, elem );
}

////////////////////////////////////////////////////////////////////////////////

//
// Note: The bare minimum I need to test code that is 'new' to the concrete MP type
//
TEST_CASE( "RefCounter" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();


    Kit::Text::FString<MAX_STR_LENG> errorMsg = "noerror";
    char                             string[MAX_STR_LENG + 1];
    bool                             truncated;
    bool                             valid;
    uint32_t                         value;
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
        REQUIRE( strcmp( mpType, "Kit::Dm::Mp::RefCounter" ) == 0 );
    }

    SECTION( "read/writes" )
    {
        uint16_t seqNum  = mp_apple_.getSequenceNumber();
        uint16_t seqNum2 = mp_apple_.increment( 2 );
        REQUIRE( seqNum + 1 == seqNum2 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 2 );  // By design the invalid MP has a 'data value' of zero

        seqNum = mp_apple_.reset( 10 );
        REQUIRE( seqNum == seqNum2 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 10 );

        seqNum2 = mp_apple_.decrement( 9 );
        REQUIRE( seqNum == seqNum2 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 1 );

        seqNum = mp_apple_.decrement();
        REQUIRE( seqNum == seqNum2 + 1 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 0 );

        seqNum2 = mp_apple_.increment();
        REQUIRE( seqNum + 1 == seqNum2 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 1 );

        seqNum = mp_apple_.increment( 0xFFFFFFFF );
        REQUIRE( seqNum == seqNum2 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 0xFFFFFFFF );

        mp_apple_.reset( 1 );
        seqNum2 = mp_apple_.decrement( 100 );
        REQUIRE( seqNum + 1 == seqNum2 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 0 );

        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == INITIAL_VALUE );
    }

    SECTION( "copy" )
    {
        mp_apple_.reset( 10 );
        mp_orange_.copyFrom( mp_apple_ );
        valid = mp_orange_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 10 );

        mp_apple_.setInvalid();
        mp_orange_.copyFrom( mp_apple_ );
        REQUIRE( mp_orange_.isNotValid() );
    }

    SECTION( "observer" )
    {
        KIT_SYSTEM_TRACE_SCOPE( SECT_, "observer test" );
        uint32_t                         expectedValue = 11;
        Viewer<Mp::RefCounter, uint32_t> viewer_apple1( t1Mbox_, Kit::System::Thread::getCurrent(), mp_apple_, expectedValue );
        Kit::System::Thread*             t1 = Kit::System::Thread::create( t1Mbox_, "T1" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Created Viewer thread (%p)", t1 );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        viewer_apple1.open();
        mp_apple_.reset( 11 );
        Kit::System::Thread::wait();
        viewer_apple1.close();

        // Shutdown threads
        t1Mbox_.pleaseStop();
        WAIT_FOR_THREAD_TO_STOP( t1 );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Destroying Viewer thread (%p)...", t1 );
        Kit::System::Thread::destroy( *t1 );
    }

    SECTION( "toJSON-pretty" )
    {
        mp_apple_.reset( 127 );
        mp_apple_.toJSON( string, MAX_STR_LENG, truncated, true, true );
        KIT_SYSTEM_TRACE_MSG( SECT_, "toJSON: [%s]", string );

        StaticJsonDocument<1024> doc;
        DeserializationError     err = deserializeJson( doc, string );
        REQUIRE( err == DeserializationError::Ok );
        REQUIRE( doc["locked"] == false );
        REQUIRE( doc["valid"] == true );
        REQUIRE( doc["val"] == 127 );
    }

    SECTION( "fromJSON" )
    {
        const char* json   = "{name:\"APPLE\", val:1234}";
        bool        result = modelDb_.fromJSON( json );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 1234 );

        json   = "{name:\"APPLE\", val:12.34}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        json   = "{name:\"APPLE\", val:12.34}";
        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
    }

    SECTION( "fromJSON-actions" )
    {
        const char* json   = "{name:\"APPLE\", val:\"+3\"}";
        bool        result = modelDb_.fromJSON( json );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 3 );

        json   = "{name:\"APPLE\", val:\"-1\"}";
        result = modelDb_.fromJSON( json );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 2 );

        json   = "{name:\"APPLE\", val:\"bob\"}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        json   = "{name:\"APPLE\", val:\"bob\"}";
        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );

        json   = "{name:\"APPLE\", val:\"+bob\"}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        json   = "{name:\"APPLE\", val:\"+bob\"}";
        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );

        json   = "{name:\"APPLE\", val:\"-bob\"}";
        result = modelDb_.fromJSON( json, &errorMsg );
        REQUIRE( result == false );
        REQUIRE( errorMsg != "noerror" );

        json   = "{name:\"APPLE\", val:\"-bob\"}";
        result = modelDb_.fromJSON( json );
        REQUIRE( result == false );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
