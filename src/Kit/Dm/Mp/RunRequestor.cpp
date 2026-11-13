/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "RunRequestor.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Dm {
namespace Mp {

///////////////////////////////////////////////////////////////////////////////
RunRequestor::RunRequestor( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName, RunControl& dataMp )
    : PseudoMpBase<RunControl_T, RunControl>( myModelBase, symbolicName, dataMp )
{
}

RunRequestStatus RunRequestor::request( RunRequest request, uint16_t& retRequestId, uint16_t* retSeqNum ) noexcept
{
    return m_dataMp.issueRequest_( request, retRequestId, retSeqNum );
}

bool RunRequestor::readStatus( RunRequestStatus& retStatus, RunState& retActualState, uint16_t& retRequestId, uint16_t* retSeqNum ) const noexcept
{
    RunControl_T data;
    if ( !m_dataMp.read( data, retSeqNum ) )
    {
        return false;
    }

    retStatus      = data.status;
    retActualState = data.actual;
    retRequestId   = data.requestId;
    return true;
}

bool RunRequestor::readStatusAndSync( RunRequestStatus& retStatus, RunState& retActualState, uint16_t& retRequestId, IObserver& observerToSync, uint16_t* retSeqNum ) noexcept
{
    RunControl_T data;
    uint16_t     seqNum = SEQUENCE_NUMBER_UNKNOWN;
    bool         valid  = m_dataMp.readAndSync( data, seqNum, observerToSync );
    if ( retSeqNum )
    {
        *retSeqNum = seqNum;
    }
    if ( !valid )
    {
        return false;
    }

    retStatus      = data.status;
    retActualState = data.actual;
    retRequestId   = data.requestId;
    return true;
}

bool RunRequestor::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::IString* errorMsg ) noexcept
{
    // Get the request
    const char* text = src;
    if ( text == nullptr )
    {
        if ( errorMsg )
        {
            *errorMsg = "Invalid syntax for the 'val' key/value pair";
        }
        return false;
    }
    auto maybeRequest = RunRequest::_from_string_nothrow( text );
    if ( !maybeRequest )
    {
        if ( errorMsg )
        {
            errorMsg->format( "Invalid request (%s)", text );
        }
        return false;
    }

    // Make the request
    uint16_t         requestId = 0;
    RunRequestStatus result    = request( *maybeRequest, requestId, &retSequenceNumber );
    if ( result != +RunRequestStatus::PENDING )
    {
        if ( errorMsg )
        {
            errorMsg->format( "Request not accepted (%s)", result._to_string() );
        }
        return false;
    }

    return true;
}


}  // end namespace
}
}
//------------------------------------------------------------------------------
