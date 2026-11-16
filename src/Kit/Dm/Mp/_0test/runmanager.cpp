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
#include "Kit/Dm/Mp/RunManager.h"
#include <string.h>

#define STRCMP( s1, s2 ) ( strcmp( s1, s2 ) == 0 )

using namespace Kit::Dm;
using namespace Kit::Dm::Mp;

////////////////////////////////////////////////////////////////////////////////

static ModelDatabase modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );
static RunControl    mp_data_( modelDb_, "RUN_DATA" );
static RunManager    mp_manager_( modelDb_, "RUN_MANAGER", mp_data_ );

// Event queue for the observer (note: the queue is never run since the test does not generate change notifications)
static Kit::EventQueue::Server eventQ_;

////////////////////////////////////////////////////////////////////////////////
// Note: The 'client' side of the interface is simulated by directly calling the
//       RunControl's package scope methods
////////////////////////////////////////////////////////////////////////////////

TEST_CASE( "RunManager" )
{
    Kit::Text::FString<128> errorMsg;
    RunControl_T            data;
    RunRequest              req    = RunRequest::NOP;
    uint16_t                id     = 0;
    uint16_t                seqNum = 0;
    mp_data_.setInvalid();
    mp_data_.setLockState( LockRequest_T::eUNLOCK );

    SECTION( "type" )
    {
        REQUIRE( STRCMP( mp_manager_.getTypeAsText(), "Kit::Dm::Mp::RunManager" ) );

        // Via the generic interface (e.g. how the database/TShell access the MP)
        IModelPoint* mp = modelDb_.lookupModelPoint( "RUN_MANAGER" );
        REQUIRE( mp == &mp_manager_ );
        REQUIRE( STRCMP( mp->getTypeAsText(), "Kit::Dm::Mp::RunManager" ) );
    }

    SECTION( "setState" )
    {
        // Publishes the initial state
        REQUIRE( mp_manager_.isNotValid() );
        REQUIRE( mp_manager_.setState( RunState::STOPPED, &seqNum ) );
        REQUIRE( seqNum == mp_data_.getSequenceNumber() );
        REQUIRE( mp_manager_.isNotValid() == false );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.actual == +RunState::STOPPED );
        REQUIRE( data.status == +RunRequestStatus::COMPLETED );

        // Unrequested state changes do not affect an in-flight request
        uint16_t reqId = 0;
        REQUIRE( mp_data_.issueRequest_( RunRequest::START, reqId ) == +RunRequestStatus::PENDING );
        REQUIRE( mp_manager_.setState( RunState::PAUSED ) );
        REQUIRE( mp_manager_.getPendingRequest( req, id ) );
        REQUIRE( id == reqId );
        REQUIRE( mp_manager_.respond( reqId, RunRequestStatus::COMPLETED, RunState::RUNNING ) );

        // ERRORED is terminal (nothing changes, sequence number is still returned)
        REQUIRE( mp_manager_.setState( RunState::ERRORED ) );
        uint16_t prev = mp_data_.getSequenceNumber();
        REQUIRE( mp_manager_.setState( RunState::RUNNING, &seqNum ) == false );
        REQUIRE( seqNum == prev );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.actual == +RunState::ERRORED );
    }

    SECTION( "getPendingRequest" )
    {
        // Invalid MP and no request
        REQUIRE( mp_manager_.getPendingRequest( req, id ) == false );
        REQUIRE( mp_manager_.setState( RunState::STOPPED ) );
        REQUIRE( mp_manager_.getPendingRequest( req, id ) == false );
        REQUIRE( req == +RunRequest::NOP );
        REQUIRE( id == 0 );

        uint16_t reqId = 0;
        REQUIRE( mp_data_.issueRequest_( RunRequest::PAUSE, reqId ) == +RunRequestStatus::PENDING );
        REQUIRE( mp_manager_.getPendingRequest( req, id ) );
        REQUIRE( req == +RunRequest::PAUSE );
        REQUIRE( id == reqId );

        // Completed
        REQUIRE( mp_manager_.respond( reqId, RunRequestStatus::COMPLETED, RunState::PAUSED ) );
        REQUIRE( mp_manager_.getPendingRequest( req, id ) == false );
    }

    SECTION( "getPendingRequestAndSync" )
    {
        ObserverCallback<RunControl> observer( eventQ_ );

        // Invalid MP: no request, but the observer is still synchronized
        REQUIRE( mp_manager_.getPendingRequestAndSync( req, id, observer ) == false );
        REQUIRE( observer.getModelPoint_() == &mp_data_ );
        mp_data_.detach( observer );

        REQUIRE( mp_manager_.setState( RunState::STOPPED ) );
        REQUIRE( mp_manager_.getPendingRequestAndSync( req, id, observer ) == false );
        REQUIRE( observer.getModelPoint_() == &mp_data_ );
        mp_data_.detach( observer );

        uint16_t reqId = 0;
        REQUIRE( mp_data_.issueRequest_( RunRequest::START, reqId ) == +RunRequestStatus::PENDING );
        REQUIRE( mp_manager_.getPendingRequestAndSync( req, id, observer ) );
        REQUIRE( req == +RunRequest::START );
        REQUIRE( id == reqId );
        REQUIRE( observer.getModelPoint_() == &mp_data_ );
        mp_data_.detach( observer );
    }

    SECTION( "respond" )
    {
        REQUIRE( mp_manager_.setState( RunState::STOPPED ) );

        // No request in-flight
        REQUIRE( mp_manager_.respond( 1, RunRequestStatus::COMPLETED, RunState::RUNNING ) == false );

        uint16_t reqId = 0;
        REQUIRE( mp_data_.issueRequest_( RunRequest::START, reqId ) == +RunRequestStatus::PENDING );
        uint16_t prev = mp_data_.getSequenceNumber();

        // Bad responses (nothing changes, sequence number is still returned)
        REQUIRE( mp_manager_.respond( reqId + 1, RunRequestStatus::COMPLETED, RunState::RUNNING, &seqNum ) == false );
        REQUIRE( seqNum == prev );
        REQUIRE( mp_manager_.respond( reqId, RunRequestStatus::NOP, RunState::RUNNING ) == false );
        REQUIRE( mp_manager_.respond( reqId, RunRequestStatus::PENDING, RunState::RUNNING ) == false );
        REQUIRE( mp_manager_.respond( reqId, RunRequestStatus::ERR_REQUEST_IN_PROGRESS, RunState::RUNNING ) == false );
        REQUIRE( mp_manager_.getPendingRequest( req, id ) );

        // Each (final) status can be returned
        REQUIRE( mp_manager_.respond( reqId, RunRequestStatus::NOT_SUPPORTED, RunState::STOPPED, &seqNum ) );
        REQUIRE( seqNum == mp_data_.getSequenceNumber() );
        REQUIRE( seqNum != prev );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.status == +RunRequestStatus::NOT_SUPPORTED );
        REQUIRE( data.actual == +RunState::STOPPED );

        REQUIRE( mp_data_.issueRequest_( RunRequest::START, reqId ) == +RunRequestStatus::PENDING );
        REQUIRE( mp_manager_.respond( reqId, RunRequestStatus::NOT_ACCEPTED, RunState::STOPPED ) );
        REQUIRE( mp_data_.issueRequest_( RunRequest::START, reqId ) == +RunRequestStatus::PENDING );
        REQUIRE( mp_manager_.respond( reqId, RunRequestStatus::FAILED, RunState::ERRORED ) );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.status == +RunRequestStatus::FAILED );
        REQUIRE( data.actual == +RunState::ERRORED );

        // Only one response per request
        REQUIRE( mp_manager_.respond( reqId, RunRequestStatus::COMPLETED, RunState::ERRORED ) == false );
    }

    SECTION( "respond - cannot recover from ERRORED" )
    {
        REQUIRE( mp_manager_.setState( RunState::RUNNING ) );
        REQUIRE( mp_data_.issueRequest_( RunRequest::STOP, id ) == +RunRequestStatus::PENDING );
        REQUIRE( mp_manager_.setState( RunState::ERRORED ) );
        REQUIRE( mp_manager_.respond( id, RunRequestStatus::COMPLETED, RunState::STOPPED ) == false );
        REQUIRE( mp_manager_.respond( id, RunRequestStatus::FAILED, RunState::ERRORED ) );
    }

    SECTION( "fromJSON - setState" )
    {
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:{state:\"STOPPED\"}}", &errorMsg, nullptr, &seqNum ) );
        REQUIRE( seqNum == mp_data_.getSequenceNumber() );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.actual == +RunState::STOPPED );

        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:{state:\"bogus\"}}", &errorMsg ) == false );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:{state:\"bogus\"}}" ) == false );

        // Not allowed in the current state
        REQUIRE( mp_manager_.setState( RunState::ERRORED ) );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:{state:\"RUNNING\"}}", &errorMsg ) == false );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:{state:\"RUNNING\"}}" ) == false );
    }

    SECTION( "fromJSON - respond" )
    {
        REQUIRE( mp_manager_.setState( RunState::STOPPED ) );
        uint16_t reqId = 0;
        REQUIRE( mp_data_.issueRequest_( RunRequest::START, reqId ) == +RunRequestStatus::PENDING );

        // Errors: missing/bad keys (nothing changes)
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:{id:1, newState:\"RUNNING\"}}", &errorMsg ) == false );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:{id:1, status:\"COMPLETED\"}}", &errorMsg ) == false );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:{status:\"COMPLETED\", newState:\"RUNNING\"}}", &errorMsg ) == false );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:{id:1, status:\"bogus\", newState:\"RUNNING\"}}", &errorMsg ) == false );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:{id:1, status:\"COMPLETED\", newState:\"bogus\"}}", &errorMsg ) == false );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:{id:1, status:\"COMPLETED\", newState:\"bogus\"}}" ) == false );
        REQUIRE( mp_manager_.getPendingRequest( req, id ) );

        // Wrong request id
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:{id:99, status:\"COMPLETED\", newState:\"RUNNING\"}}", &errorMsg ) == false );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:{id:99, status:\"COMPLETED\", newState:\"RUNNING\"}}" ) == false );
        REQUIRE( mp_manager_.getPendingRequest( req, id ) );

        // Good response
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:{id:1, status:\"COMPLETED\", newState:\"RUNNING\"}}", &errorMsg, nullptr, &seqNum ) );
        REQUIRE( seqNum == mp_data_.getSequenceNumber() );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.status == +RunRequestStatus::COMPLETED );
        REQUIRE( data.actual == +RunState::RUNNING );
    }

    SECTION( "fromJSON - setState and respond keys cannot be mixed" )
    {
        REQUIRE( mp_manager_.setState( RunState::STOPPED ) );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:{id:1, status:\"COMPLETED\", state:\"RUNNING\"}}", &errorMsg ) == false );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:{state:\"PAUSED\", newState:\"RUNNING\"}}", &errorMsg ) == false );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:{}}", &errorMsg ) == false );
        REQUIRE( modelDb_.fromJSON( "{name:\"RUN_MANAGER\", val:\"RUNNING\"}", &errorMsg ) == false );
        REQUIRE( mp_data_.read( data ) );
        REQUIRE( data.actual == +RunState::STOPPED );
    }

    // Cleanup for a (potential) subsequent test
    mp_data_.setLockState( LockRequest_T::eUNLOCK );
    mp_data_.setInvalid();
}
