/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "RunManager.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Dm {
namespace Mp {

///////////////////////////////////////////////////////////////////////////////
RunManager::RunManager( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, RunControl& dataMp )
    : PseudoMpBase<RunControl_T, RunControl>( myModelBase, symbolicName, dataMp )
{
}

bool RunManager::getPendingRequest( RunRequest& retRequest, uint16_t& retRequestId ) const noexcept
{
    RunControl_T data;
    if ( m_dataMp.read( data ) && data.status == +RunRequestStatus::PENDING )
    {
        retRequest   = data.requested;
        retRequestId = data.requestId;
        return true;
    }
    return false;
}

bool RunManager::getPendingRequestAndSync( RunRequest& retRequest, uint16_t& retRequestId, IObserver& observerToSync ) noexcept
{
    RunControl_T data;
    bool         valid = m_dataMp.readAndSync( data, observerToSync );
    if ( valid && data.status == +RunRequestStatus::PENDING )
    {
        retRequest   = data.requested;
        retRequestId = data.requestId;
        return true;
    }
    return false;
}

bool RunManager::respond( uint16_t requestId, RunRequestStatus result, RunState actualState, uint16_t* retSeqNum ) noexcept
{
    return m_dataMp.respondToRequest_( requestId, result, actualState, retSeqNum );
}

bool RunManager::setState( RunState actualState, uint16_t* retSeqNum ) noexcept
{
    return m_dataMp.updateState_( actualState, retSeqNum );
}

bool RunManager::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::IString* errorMsg ) noexcept
{
    if ( src.is<JsonObject>() == false )
    {
        if ( errorMsg )
        {
            *errorMsg = "'val' key/value pair is NOT an JSON object";
        }
        return false;
    }

    // The 'setState' and 'response' actions use different keys, i.e. they cannot be mixed
    bool isSetState = !src["state"].isNull();
    bool isResponse = !src["id"].isNull() || !src["status"].isNull() || !src["newState"].isNull();
    if ( isSetState == isResponse )
    {
        if ( errorMsg )
        {
            *errorMsg = "Specify either 'state' (set state) or 'id', 'status', and 'newState' (respond)";
        }
        return false;
    }

    bool result;

    // Set the state (no response)
    if ( isSetState )
    {
        const char* stateText  = src["state"] | "";
        auto        maybeState = RunState::_from_string_nothrow( stateText );
        if ( !maybeState )
        {
            if ( errorMsg )
            {
                *errorMsg = "Invalid 'state' key/value pair";
            }
            return false;
        }
        result = setState( *maybeState, &retSequenceNumber );
    }

    // Respond to a request
    else
    {
        const char* statusText   = src["status"] | "";
        const char* newStateText = src["newState"] | "";
        auto        maybeStatus  = RunRequestStatus::_from_string_nothrow( statusText );
        auto        maybeState   = RunState::_from_string_nothrow( newStateText );
        if ( !maybeStatus || !maybeState || !src["id"].is<uint16_t>() )
        {
            if ( errorMsg )
            {
                *errorMsg = "Missing/invalid 'id', 'status', and/or 'newState' key/value pair";
            }
            return false;
        }
        result = respond( src["id"].as<uint16_t>(), *maybeStatus, *maybeState, &retSequenceNumber );
    }

    if ( !result )
    {
        if ( errorMsg )
        {
            *errorMsg = "Operation not allowed in the current state";
        }
        return false;
    }

    return true;
}


}  // end namespace
}
}
//------------------------------------------------------------------------------
