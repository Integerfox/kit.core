/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "RunControl.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Dm {
namespace Mp {

///////////////////////////////////////////////////////////////////////////////
RunControl::RunControl( Kit::Dm::IModelDatabase& myModelBase, const char* symbolicName )
    : Scalar<RunControl_T, RunControl>( myModelBase, symbolicName )
{
}

///////////////////////////////////////////////////////////////////////////////
RunRequestStatus RunControl::issueRequest_( RunRequest request, uint16_t& newRequestId, uint16_t* retSeqNum ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );

    RunRequestStatus result = RunRequestStatus::PENDING;
    if ( !m_valid || request == +RunRequest::NOP || m_data.actual == +RunState::INVALID ||
         m_data.actual == +RunState::ERRORED || !testAndUpdateLock( LockRequest_T::eNO_REQUEST ) )
    {
        result = RunRequestStatus::NOT_ACCEPTED;
    }
    else if ( m_data.status == +RunRequestStatus::PENDING )
    {
        result = RunRequestStatus::ERR_REQUEST_IN_PROGRESS;
    }
    else
    {
        // Request id zero is reserved for 'no request has been made'
        m_data.requestId++;
        if ( m_data.requestId == 0 )
        {
            m_data.requestId = 1;
        }
        m_data.requested = request;
        m_data.status    = RunRequestStatus::PENDING;
        newRequestId     = m_data.requestId;
        processDataUpdated();
    }

    if ( retSeqNum )
    {
        *retSeqNum = m_seqNum;
    }
    return result;
}

bool RunControl::respondToRequest_( uint16_t requestId, RunRequestStatus result, RunState newState, uint16_t* retSeqNum ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );

    bool updated = false;
    if ( m_valid && m_data.status == +RunRequestStatus::PENDING && m_data.requestId == requestId &&
         result != +RunRequestStatus::NOP && result != +RunRequestStatus::PENDING && result != +RunRequestStatus::ERR_REQUEST_IN_PROGRESS &&
         ( m_data.actual != +RunState::ERRORED || newState == +RunState::ERRORED ) &&
         testAndUpdateLock( LockRequest_T::eNO_REQUEST ) )
    {
        m_data.status = result;
        m_data.actual = newState;
        processDataUpdated();
        updated = true;
    }

    if ( retSeqNum )
    {
        *retSeqNum = m_seqNum;
    }
    return updated;
}

bool RunControl::updateState_( RunState newState, uint16_t* retSeqNum ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( Kit::Dm::ModelPointBase::m_modelDatabase.getMutex_() );

    bool updated = false;
    if ( testAndUpdateLock( LockRequest_T::eNO_REQUEST ) && ( !m_valid || m_data.actual != +RunState::ERRORED || newState == +RunState::ERRORED ) )
    {
        updated = true;

        // Becoming valid: no request has been made (or is in-flight)
        if ( !m_valid )
        {
            m_data.requested = RunRequest::NOP;
            m_data.actual    = newState;
            m_data.status    = RunRequestStatus::COMPLETED;
            processDataUpdated();
        }
        else if ( m_data.actual != newState )
        {
            m_data.actual = newState;
            processDataUpdated();
        }
    }

    if ( retSeqNum )
    {
        *retSeqNum = m_seqNum;
    }
    return updated;
}

///////////////////////////////////////////////////////////////////////////////
bool RunControl::setJSONVal( JsonDocument& doc ) noexcept
{
    JsonObject valObj   = doc.createNestedObject( "val" );
    valObj["requested"] = (char*)m_data.requested._to_string();
    valObj["actual"]    = (char*)m_data.actual._to_string();
    valObj["status"]    = (char*)m_data.status._to_string();
    valObj["requestId"] = m_data.requestId;
    return true;
}

bool RunControl::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Kit::Text::IString* errorMsg ) noexcept
{
    if ( errorMsg )
    {
        *errorMsg = "The Model Point cannot be modified via JSON";
    }
    return false;
}


}  // end namespace
}
}
//------------------------------------------------------------------------------
