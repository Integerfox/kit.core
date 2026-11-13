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
#include "Kit/Text/FString.h"
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Dm/Mp/RunControl.h"
#include <string.h>

#define STRCMP( s1, s2 ) ( strcmp( s1, s2 ) == 0 )
#define MAX_STR_LENG     1024

using namespace Kit::Dm;
using namespace Kit::Dm::Mp;

////////////////////////////////////////////////////////////////////////////////

static ModelDatabase modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );
static RunControl    mp_data_( modelDb_, "RUN_DATA" );

////////////////////////////////////////////////////////////////////////////////
// Note: The package scope methods (that the RunRequestor and RunManager call) are
//       called directly.
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "RunControl_T" )
{
    RunControl_T a( 3, RunRequest::STOP, RunState::PAUSED, RunRequestStatus::FAILED );
    RunControl_T b( a );
    RunControl_T c;

    // The default constructor zeros the struct
    REQUIRE( c.requestId == 0 );
    REQUIRE( c.requested == +RunRequest::NOP );
    REQUIRE( c.actual == +RunState::INVALID );
    REQUIRE( c.status == +RunRequestStatus::NOP );

    REQUIRE( a == b );
    REQUIRE( !( a == c ) );
    c = a;
    REQUIRE( c == a );
    REQUIRE( c.requestId == 3 );
    REQUIRE( c.requested == +RunRequest::STOP );
    REQUIRE( c.actual == +RunState::PAUSED );
    REQUIRE( c.status == +RunRequestStatus::FAILED );
}

TEST_CASE( "RunControl" )
{
    RunControl_T data;
    uint16_t     id     = 0;
    uint16_t     seqNum = 0;
    mp_data_.setInvalid();
    mp_data_.setLockState( LockRequest_T::eUNLOCK );

    SECTION( "initial state" )
    {
        REQUIRE( mp_data_.isNotValid() );
        REQUIRE( mp_data_.read( data ) == false );
        REQUIRE( STRCMP( mp_data_.getTypeAsText(), "Kit::Dm::Mp::RunControl" ) );
    }

    SECTION( "updateState_" )
    {
        // Becoming valid: no request made
        REQUIRE( mp_data_.updateState_( RunState::STOPPED, &seqNum ) );
        REQUIRE( seqNum == mp_data_.getSequenceNumber() );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.actual == +RunState::STOPPED );
        REQUIRE( data.status == +RunRequestStatus::COMPLETED );
        REQUIRE( data.requested == +RunRequest::NOP );
        REQUIRE( data.requestId == 0 );

        // No change/no change notification when the state is the same
        uint16_t prev = seqNum;
        REQUIRE( mp_data_.updateState_( RunState::STOPPED, &seqNum ) );
        REQUIRE( seqNum == prev );

        REQUIRE( mp_data_.updateState_( RunState::RUNNING, &seqNum ) );
        REQUIRE( seqNum != prev );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.actual == +RunState::RUNNING );

        // Does not affect an in-flight request
        REQUIRE( mp_data_.issueRequest_( RunRequest::PAUSE, id ) == +RunRequestStatus::PENDING );
        REQUIRE( mp_data_.updateState_( RunState::STOPPED ) );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.actual == +RunState::STOPPED );
        REQUIRE( data.status == +RunRequestStatus::PENDING );
        REQUIRE( data.requestId == id );

        // Locked
        mp_data_.setLockState( LockRequest_T::eLOCK );
        prev = mp_data_.getSequenceNumber();
        REQUIRE( mp_data_.updateState_( RunState::RUNNING, &seqNum ) == false );
        REQUIRE( seqNum == prev );
    }

    SECTION( "ERRORED is terminal" )
    {
        REQUIRE( mp_data_.updateState_( RunState::RUNNING ) );
        REQUIRE( mp_data_.updateState_( RunState::ERRORED ) );
        REQUIRE( mp_data_.updateState_( RunState::RUNNING ) == false );
        REQUIRE( mp_data_.updateState_( RunState::STOPPED ) == false );
        REQUIRE( mp_data_.updateState_( RunState::ERRORED ) );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.actual == +RunState::ERRORED );

        // Invalidating the MP is the only way out
        mp_data_.setInvalid();
        REQUIRE( mp_data_.updateState_( RunState::RUNNING ) );
    }

    SECTION( "issueRequest_" )
    {
        // Not accepted: invalid MP, INVALID state, NOP request
        REQUIRE( mp_data_.issueRequest_( RunRequest::START, id ) == +RunRequestStatus::NOT_ACCEPTED );
        REQUIRE( mp_data_.updateState_( RunState::INVALID ) );
        REQUIRE( mp_data_.issueRequest_( RunRequest::START, id ) == +RunRequestStatus::NOT_ACCEPTED );
        REQUIRE( mp_data_.updateState_( RunState::STOPPED ) );
        REQUIRE( mp_data_.issueRequest_( RunRequest::NOP, id ) == +RunRequestStatus::NOT_ACCEPTED );

        // Accepted
        uint16_t prev = mp_data_.getSequenceNumber();
        REQUIRE( mp_data_.issueRequest_( RunRequest::START, id, &seqNum ) == +RunRequestStatus::PENDING );
        REQUIRE( id == 1 );
        REQUIRE( seqNum == mp_data_.getSequenceNumber() );
        REQUIRE( seqNum != prev );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.requested == +RunRequest::START );
        REQUIRE( data.status == +RunRequestStatus::PENDING );
        REQUIRE( data.requestId == 1 );

        // Only one request at a time (nothing changes when refused)
        uint16_t id2 = 0;
        REQUIRE( mp_data_.issueRequest_( RunRequest::STOP, id2, &seqNum ) == +RunRequestStatus::ERR_REQUEST_IN_PROGRESS );
        REQUIRE( id2 == 0 );
        REQUIRE( seqNum == mp_data_.getSequenceNumber() );
        REQUIRE( seqNum != prev );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.requested == +RunRequest::START );

        // Next request gets a new identifier
        REQUIRE( mp_data_.respondToRequest_( id, RunRequestStatus::COMPLETED, RunState::RUNNING ) );
        REQUIRE( mp_data_.issueRequest_( RunRequest::STOP, id ) == +RunRequestStatus::PENDING );
        REQUIRE( id == 2 );

        // ERRORED: not accepted
        REQUIRE( mp_data_.respondToRequest_( id, RunRequestStatus::FAILED, RunState::ERRORED ) );
        REQUIRE( mp_data_.issueRequest_( RunRequest::START, id ) == +RunRequestStatus::NOT_ACCEPTED );

        // Locked: not accepted
        mp_data_.setInvalid();
        REQUIRE( mp_data_.updateState_( RunState::STOPPED ) );
        mp_data_.setLockState( LockRequest_T::eLOCK );
        prev = mp_data_.getSequenceNumber();
        REQUIRE( mp_data_.issueRequest_( RunRequest::START, id, &seqNum ) == +RunRequestStatus::NOT_ACCEPTED );
        REQUIRE( seqNum == prev );
    }

    SECTION( "request id skips zero when rolling over" )
    {
        REQUIRE( mp_data_.updateState_( RunState::STOPPED ) );
        for ( uint32_t i = 0; i < 65536; i++ )
        {
            REQUIRE( mp_data_.issueRequest_( RunRequest::START, id ) == +RunRequestStatus::PENDING );
            REQUIRE( id != 0 );
            REQUIRE( mp_data_.respondToRequest_( id, RunRequestStatus::COMPLETED, RunState::RUNNING ) );
        }
        REQUIRE( id == 1 );
    }

    SECTION( "respondToRequest_" )
    {
        // No request in-flight
        REQUIRE( mp_data_.respondToRequest_( 1, RunRequestStatus::COMPLETED, RunState::RUNNING ) == false );
        REQUIRE( mp_data_.updateState_( RunState::STOPPED ) );
        REQUIRE( mp_data_.respondToRequest_( 0, RunRequestStatus::COMPLETED, RunState::RUNNING ) == false );

        REQUIRE( mp_data_.issueRequest_( RunRequest::START, id ) == +RunRequestStatus::PENDING );
        uint16_t prev = mp_data_.getSequenceNumber();

        // Bad responses (nothing changes, sequence number is still returned)
        REQUIRE( mp_data_.respondToRequest_( id + 1, RunRequestStatus::COMPLETED, RunState::RUNNING, &seqNum ) == false );
        REQUIRE( seqNum == prev );
        REQUIRE( mp_data_.respondToRequest_( id, RunRequestStatus::NOP, RunState::RUNNING ) == false );
        REQUIRE( mp_data_.respondToRequest_( id, RunRequestStatus::PENDING, RunState::RUNNING ) == false );
        REQUIRE( mp_data_.respondToRequest_( id, RunRequestStatus::ERR_REQUEST_IN_PROGRESS, RunState::RUNNING ) == false );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.status == +RunRequestStatus::PENDING );
        REQUIRE( data.actual == +RunState::STOPPED );

        // Good response
        REQUIRE( mp_data_.respondToRequest_( id, RunRequestStatus::NOT_SUPPORTED, RunState::PAUSED, &seqNum ) );
        REQUIRE( seqNum == mp_data_.getSequenceNumber() );
        REQUIRE( seqNum != prev );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.status == +RunRequestStatus::NOT_SUPPORTED );
        REQUIRE( data.actual == +RunState::PAUSED );

        // Only one response per request
        REQUIRE( mp_data_.respondToRequest_( id, RunRequestStatus::COMPLETED, RunState::RUNNING ) == false );
    }

    SECTION( "respondToRequest_ - ERRORED" )
    {
        REQUIRE( mp_data_.updateState_( RunState::RUNNING ) );
        REQUIRE( mp_data_.issueRequest_( RunRequest::STOP, id ) == +RunRequestStatus::PENDING );

        // The state can become ERRORED while the request is in-flight, i.e. the response cannot recover
        REQUIRE( mp_data_.updateState_( RunState::ERRORED ) );
        REQUIRE( mp_data_.respondToRequest_( id, RunRequestStatus::COMPLETED, RunState::STOPPED ) == false );
        REQUIRE( mp_data_.respondToRequest_( id, RunRequestStatus::FAILED, RunState::ERRORED ) );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.actual == +RunState::ERRORED );
        REQUIRE( data.status == +RunRequestStatus::FAILED );
    }

    SECTION( "respondToRequest_ - locked" )
    {
        REQUIRE( mp_data_.updateState_( RunState::STOPPED ) );
        REQUIRE( mp_data_.issueRequest_( RunRequest::START, id ) == +RunRequestStatus::PENDING );
        mp_data_.setLockState( LockRequest_T::eLOCK );
        uint16_t prev = mp_data_.getSequenceNumber();
        REQUIRE( mp_data_.respondToRequest_( id, RunRequestStatus::COMPLETED, RunState::RUNNING, &seqNum ) == false );
        REQUIRE( seqNum == prev );
    }

    SECTION( "toJSON" )
    {
        char buffer[MAX_STR_LENG];
        bool truncated;

        REQUIRE( mp_data_.updateState_( RunState::PAUSED ) );
        REQUIRE( mp_data_.issueRequest_( RunRequest::RESUME, id ) == +RunRequestStatus::PENDING );
        mp_data_.toJSON( buffer, sizeof( buffer ), truncated );

        StaticJsonDocument<1024> doc;
        REQUIRE( deserializeJson( doc, buffer ) == DeserializationError::Ok );
        REQUIRE( doc["valid"] == true );
        REQUIRE( STRCMP( doc["type"], "Kit::Dm::Mp::RunControl" ) );
        REQUIRE( STRCMP( doc["val"]["requested"], "RESUME" ) );
        REQUIRE( STRCMP( doc["val"]["actual"], "PAUSED" ) );
        REQUIRE( STRCMP( doc["val"]["status"], "PENDING" ) );
        REQUIRE( doc["val"]["requestId"] == id );

        // Invalid
        mp_data_.setInvalid();
        mp_data_.toJSON( buffer, sizeof( buffer ), truncated );
        REQUIRE( deserializeJson( doc, buffer ) == DeserializationError::Ok );
        REQUIRE( doc["valid"] == false );
    }

    SECTION( "fromJSON is not supported" )
    {
        Kit::Text::FString<128> errorMsg;
        REQUIRE( mp_data_.updateState_( RunState::STOPPED ) );

        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_DATA\", val:{actual:\"RUNNING\"}}", &errorMsg ) == false );
        REQUIRE( errorMsg.isEmpty() == false );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_DATA\", val:{actual:\"RUNNING\"}}" ) == false );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.actual == +RunState::STOPPED );
    }

    // Cleanup for a (potential) subsequent test
    mp_data_.setLockState( LockRequest_T::eUNLOCK );
    mp_data_.setInvalid();
}
