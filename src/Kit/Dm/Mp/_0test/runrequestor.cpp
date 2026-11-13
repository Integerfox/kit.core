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
#include "Kit/Dm/ObserverCallback.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/Dm/Mp/RunRequestor.h"
#include <string.h>

#define STRCMP( s1, s2 ) ( strcmp( s1, s2 ) == 0 )

using namespace Kit::Dm;
using namespace Kit::Dm::Mp;

////////////////////////////////////////////////////////////////////////////////

static ModelDatabase modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );
static RunControl    mp_data_( modelDb_, "RUN_DATA" );
static RunRequestor  mp_requestor_( modelDb_, "RUN_REQUESTOR", mp_data_ );

// Event queue for the observer (note: the queue is never run since the test does not generate change notifications)
static Kit::EventQueue::Server eventQ_;

////////////////////////////////////////////////////////////////////////////////
// Note: The 'widget' side of the interface is simulated by directly calling the
//       RunControl's package scope methods
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "RunRequestor" )
{
    Kit::Text::FString<128> errorMsg;
    RunRequestStatus        status = RunRequestStatus::FAILED;
    RunState                state  = RunState::PAUSED;
    uint16_t                id     = 0;
    uint16_t                seqNum = 0;
    mp_data_.setInvalid();
    mp_data_.setLockState( LockRequest_T::eUNLOCK );

    SECTION( "type" )
    {
        REQUIRE( STRCMP( mp_requestor_.getTypeAsText(), "Kit::Dm::Mp::RunRequestor" ) );

        // Via the generic interface (e.g. how the database/TShell access the MP)
        IModelPoint* mp = modelDb_.lookupModelPoint( "RUN_REQUESTOR" );
        REQUIRE( mp == &mp_requestor_ );
        REQUIRE( STRCMP( mp->getTypeAsText(), "Kit::Dm::Mp::RunRequestor" ) );
    }

    SECTION( "requests are not accepted until the widget's state is published" )
    {
        REQUIRE( mp_requestor_.start( id ) == +RunRequestStatus::NOT_ACCEPTED );
        REQUIRE( id == 0 );
        REQUIRE( mp_data_.updateState_( RunState::STOPPED ) );
        REQUIRE( mp_requestor_.start( id ) == +RunRequestStatus::PENDING );
    }

    SECTION( "request" )
    {
        REQUIRE( mp_data_.updateState_( RunState::STOPPED ) );

        // Each convenience method makes its request
        RunControl_T data;
        REQUIRE( mp_requestor_.start( id, &seqNum ) == +RunRequestStatus::PENDING );
        REQUIRE( id == 1 );
        REQUIRE( seqNum == mp_data_.getSequenceNumber() );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.requested == +RunRequest::START );
        REQUIRE( data.status == +RunRequestStatus::PENDING );
        REQUIRE( mp_data_.respondToRequest_( id, RunRequestStatus::COMPLETED, RunState::RUNNING ) );

        REQUIRE( mp_requestor_.pause( id ) == +RunRequestStatus::PENDING );
        REQUIRE( id == 2 );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.requested == +RunRequest::PAUSE );
        REQUIRE( mp_data_.respondToRequest_( id, RunRequestStatus::COMPLETED, RunState::PAUSED ) );

        REQUIRE( mp_requestor_.resume( id ) == +RunRequestStatus::PENDING );
        REQUIRE( id == 3 );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.requested == +RunRequest::RESUME );
        REQUIRE( mp_data_.respondToRequest_( id, RunRequestStatus::COMPLETED, RunState::RUNNING ) );

        REQUIRE( mp_requestor_.stop( id ) == +RunRequestStatus::PENDING );
        REQUIRE( id == 4 );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.requested == +RunRequest::STOP );
        REQUIRE( mp_data_.respondToRequest_( id, RunRequestStatus::COMPLETED, RunState::STOPPED ) );

        // Generic request
        REQUIRE( mp_requestor_.request( RunRequest::START, id ) == +RunRequestStatus::PENDING );
        REQUIRE( id == 5 );
    }

    SECTION( "one request at a time" )
    {
        REQUIRE( mp_data_.updateState_( RunState::STOPPED ) );
        REQUIRE( mp_requestor_.start( id ) == +RunRequestStatus::PENDING );
        uint16_t firstId = id;
        uint16_t prev    = mp_data_.getSequenceNumber();

        // The id is not updated and the sequence number is still returned
        id = 0;
        REQUIRE( mp_requestor_.stop( id, &seqNum ) == +RunRequestStatus::ERR_REQUEST_IN_PROGRESS );
        REQUIRE( id == 0 );
        REQUIRE( seqNum == prev );

        // Can make another request once the first completes
        REQUIRE( mp_data_.respondToRequest_( firstId, RunRequestStatus::COMPLETED, RunState::RUNNING ) );
        REQUIRE( mp_requestor_.stop( id ) == +RunRequestStatus::PENDING );
        REQUIRE( id != firstId );
    }

    SECTION( "requests are not accepted when ERRORED" )
    {
        REQUIRE( mp_data_.updateState_( RunState::RUNNING ) );
        REQUIRE( mp_requestor_.stop( id ) == +RunRequestStatus::PENDING );
        REQUIRE( mp_data_.respondToRequest_( id, RunRequestStatus::FAILED, RunState::ERRORED ) );
        REQUIRE( mp_requestor_.start( id, &seqNum ) == +RunRequestStatus::NOT_ACCEPTED );
        REQUIRE( seqNum == mp_data_.getSequenceNumber() );
    }

    SECTION( "readStatus" )
    {
        // Invalid: outputs not updated
        REQUIRE( mp_requestor_.readStatus( status, state, id, &seqNum ) == false );
        REQUIRE( status == +RunRequestStatus::FAILED );
        REQUIRE( state == +RunState::PAUSED );
        REQUIRE( id == 0 );

        REQUIRE( mp_data_.updateState_( RunState::STOPPED ) );
        REQUIRE( mp_requestor_.readStatus( status, state, id ) );
        REQUIRE( status == +RunRequestStatus::COMPLETED );
        REQUIRE( state == +RunState::STOPPED );
        REQUIRE( id == 0 );

        uint16_t reqId = 0;
        REQUIRE( mp_requestor_.start( reqId ) == +RunRequestStatus::PENDING );
        REQUIRE( mp_requestor_.readStatus( status, state, id, &seqNum ) );
        REQUIRE( status == +RunRequestStatus::PENDING );
        REQUIRE( state == +RunState::STOPPED );
        REQUIRE( id == reqId );
        REQUIRE( seqNum == mp_data_.getSequenceNumber() );

        REQUIRE( mp_data_.respondToRequest_( reqId, RunRequestStatus::COMPLETED, RunState::RUNNING ) );
        REQUIRE( mp_requestor_.readStatus( status, state, id ) );
        REQUIRE( status == +RunRequestStatus::COMPLETED );
        REQUIRE( state == +RunState::RUNNING );
        REQUIRE( id == reqId );
    }

    SECTION( "readStatusAndSync" )
    {
        ObserverCallback<RunControl> observer( eventQ_ );

        // Invalid MP: outputs not updated, but the observer is still synchronized
        REQUIRE( mp_requestor_.readStatusAndSync( status, state, id, observer, &seqNum ) == false );
        REQUIRE( status == +RunRequestStatus::FAILED );
        REQUIRE( state == +RunState::PAUSED );
        REQUIRE( seqNum == mp_data_.getSequenceNumber() );
        REQUIRE( observer.getModelPoint_() == &mp_data_ );
        mp_data_.detach( observer );

        REQUIRE( mp_data_.updateState_( RunState::STOPPED ) );
        uint16_t reqId = 0;
        REQUIRE( mp_requestor_.start( reqId ) == +RunRequestStatus::PENDING );
        REQUIRE( mp_requestor_.readStatusAndSync( status, state, id, observer ) );
        REQUIRE( status == +RunRequestStatus::PENDING );
        REQUIRE( state == +RunState::STOPPED );
        REQUIRE( id == reqId );
        REQUIRE( observer.getModelPoint_() == &mp_data_ );
        mp_data_.detach( observer );
    }

    SECTION( "fromJSON" )
    {
        RunControl_T data;
        REQUIRE( mp_data_.updateState_( RunState::STOPPED ) );

        // Make a request
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_REQUESTOR\", val:\"START\"}", &errorMsg, nullptr, &seqNum ) );
        REQUIRE( seqNum == mp_data_.getSequenceNumber() );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.status == +RunRequestStatus::PENDING );
        REQUIRE( data.requested == +RunRequest::START );

        // Errors
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_REQUESTOR\", val:\"STOP\"}", &errorMsg ) == false );
        REQUIRE( errorMsg == "Request not accepted (ERR_REQUEST_IN_PROGRESS)" );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_REQUESTOR\", val:\"STOP\"}" ) == false );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_REQUESTOR\", val:\"bogus\"}", &errorMsg ) == false );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_REQUESTOR\", val:\"bogus\"}" ) == false );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_REQUESTOR\", val:{}}", &errorMsg ) == false );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_REQUESTOR\", val:{}}" ) == false );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.requested == +RunRequest::START );
    }

    // Cleanup for a (potential) subsequent test
    mp_data_.setLockState( LockRequest_T::eUNLOCK );
    mp_data_.setInvalid();
}
