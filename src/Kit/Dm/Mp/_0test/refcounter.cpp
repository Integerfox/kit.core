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
#include "common.h"
#include "Kit/_support/testing/ManualEventQueue.h"
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
    mp_apple_.setInvalid( ChangeNotice_T::eNORMAL, LockRequest_T::eUNLOCK );

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

    SECTION( "forceNotification" )
    {
        // Reset/increment/decrement that do NOT cross a zero boundary are 'silent' by default
        uint16_t seqNum  = mp_apple_.reset( 10 );
        uint16_t seqNum2 = mp_apple_.reset( 20 );
        REQUIRE( seqNum2 == seqNum );
        seqNum2 = mp_apple_.increment( 1 );
        REQUIRE( seqNum2 == seqNum );
        seqNum2 = mp_apple_.decrement( 1 );
        REQUIRE( seqNum2 == seqNum );

        // ...but eFORCE always triggers a change notification
        seqNum2 = mp_apple_.reset( 20, ChangeNotice_T::eFORCE );
        REQUIRE( seqNum2 == seqNum + 1 );
        seqNum = mp_apple_.increment( 1, ChangeNotice_T::eFORCE );
        REQUIRE( seqNum == seqNum2 + 1 );
        seqNum2 = mp_apple_.decrement( 1, ChangeNotice_T::eFORCE );
        REQUIRE( seqNum2 == seqNum + 1 );

        // eFORCE does NOT override the locked state
        mp_apple_.applyLock();
        seqNum  = mp_apple_.getSequenceNumber();
        seqNum2 = mp_apple_.reset( 99, ChangeNotice_T::eFORCE );
        REQUIRE( seqNum2 == seqNum );
        seqNum2 = mp_apple_.increment( 99, ChangeNotice_T::eFORCE );
        REQUIRE( seqNum2 == seqNum );
        seqNum2 = mp_apple_.decrement( 99, ChangeNotice_T::eFORCE );
        REQUIRE( seqNum2 == seqNum );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 20 );

        // ...but an explicit lock request still writes through (and notifies)
        seqNum2 = mp_apple_.reset( 99, ChangeNotice_T::eFORCE, LockRequest_T::eLOCK );
        REQUIRE( seqNum2 == seqNum + 1 );
        valid = mp_apple_.read( value );
        REQUIRE( valid );
        REQUIRE( value == 99 );
        REQUIRE( mp_apple_.isLocked() );
        mp_apple_.removeLock();
    }

    SECTION( "observer" )
    {
        KIT_SYSTEM_TRACE_SCOPE( SECT_, "observer test" );
        ManualEventQueue                 eventQueue;  // No thread: the test thread runs the queue
        Viewer<Mp::RefCounter, uint32_t> viewer( eventQueue, mp_apple_ );

        // NOTE: The MP MUST be in the INVALID state at the start of this test
        // Attaching (with an unknown sequence number) generates a notification of the current state
        REQUIRE( eventQueue.requestOpen( viewer ) );
        REQUIRE( viewer.m_notificationCount == 1 );
        REQUIRE( viewer.m_lastValid == false );

        // Invalid -->valid
        mp_apple_.reset( 11 );
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( viewer.m_notificationCount == 2 );
        REQUIRE( viewer.m_lastValid );
        REQUIRE( viewer.m_lastValue == 11 );

        // Non-zero -->non-zero transitions do NOT generate change notifications
        mp_apple_.reset( 11 );
        mp_apple_.increment();
        mp_apple_.decrement( 5 );
        mp_apple_.reset( UINT32_MAX );
        mp_apple_.increment();  // Clamped
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( viewer.m_notificationCount == 2 );
        uint32_t value = 0;
        REQUIRE( mp_apple_.read( value ) );
        REQUIRE( value == UINT32_MAX );

        // ...unless forced
        mp_apple_.increment( 1, Kit::Dm::ChangeNotice_T::eFORCE );
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( viewer.m_notificationCount == 3 );
        REQUIRE( viewer.m_lastValue == UINT32_MAX );

        // Transition to zero
        mp_apple_.decrement( UINT32_MAX );
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( viewer.m_notificationCount == 4 );
        REQUIRE( viewer.m_lastValue == 0 );

        // Zero -->zero (clamped) is not a transition
        mp_apple_.decrement();
        mp_apple_.reset( 0 );
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( viewer.m_notificationCount == 4 );

        // Transition from zero
        mp_apple_.increment();
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( viewer.m_notificationCount == 5 );
        REQUIRE( viewer.m_lastValue == 1 );

        // Valid -->invalid is a change, invalid -->invalid is not
        mp_apple_.setInvalid();
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( viewer.m_notificationCount == 6 );
        REQUIRE( viewer.m_lastValid == false );
        mp_apple_.setInvalid();
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( viewer.m_notificationCount == 6 );

        // Detached -->no change notification
        REQUIRE( eventQueue.requestClose( viewer ) );
        mp_apple_.reset( 11 );
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( viewer.m_notificationCount == 6 );
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

        json   = "{name:\"APPLE\", val:\"+5\", locked:true}";
        result = modelDb_.fromJSON( json );
        REQUIRE( result == true );
        valid = mp_apple_.read( value );
        REQUIRE( valid == true );
        REQUIRE( value == 7 );
        REQUIRE( mp_apple_.isLocked() == true );

        // Avoid leaking lock state across sections (static MP instance)
        mp_apple_.removeLock();
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
