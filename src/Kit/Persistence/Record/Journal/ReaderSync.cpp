/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "ReaderSync.h"
#include "Kit/Itc/SyncReturnHandler.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Persistence {
namespace Record {
namespace Journal {

bool ReaderSync::retrieveLatest( IPayload& dst, IEntry::Marker_T& entryMarker ) noexcept
{
    ReadRequest::LatestPayload     msgPayload( dst );
    Kit::Itc::SyncReturnHandler    srh;
    ReadRequest::RetrieveLatestMsg msg( *this, msgPayload, srh );
    m_myEventQueue.postSync( msg );

    entryMarker = msgPayload.m_markerEntryRetrieved;
    return msgPayload.m_success;
}

bool ReaderSync::retrieveNext( uint64_t               newerThanTimestamp,
                               const IEntry::Marker_T beginHereMarker,
                               IPayload&              dst,
                               IEntry::Marker_T&      entryMarker ) noexcept
{
    IEntry::Marker_T             beginHere = beginHereMarker;
    ReadRequest::NextPayload     msgPayload( dst, beginHere, newerThanTimestamp );
    Kit::Itc::SyncReturnHandler  srh;
    ReadRequest::RetrieveNextMsg msg( *this, msgPayload, srh );
    m_myEventQueue.postSync( msg );

    entryMarker = msgPayload.m_markerEntryRetrieved;
    return msgPayload.m_success;
}


bool ReaderSync::retrievePrevious( uint64_t               olderThanTimestamp,
                                   const IEntry::Marker_T beginHereMarker,
                                   IPayload&              dst,
                                   IEntry::Marker_T&      entryMarker ) noexcept
{
    IEntry::Marker_T                 beginHere = beginHereMarker;
    ReadRequest::PreviousPayload     msgPayload( dst, beginHere, olderThanTimestamp );
    Kit::Itc::SyncReturnHandler      srh;
    ReadRequest::RetrievePreviousMsg msg( *this, msgPayload, srh );
    m_myEventQueue.postSync( msg );

    entryMarker = msgPayload.m_markerEntryRetrieved;
    return msgPayload.m_success;
}

bool ReaderSync::retrieveByEntryIndex( Size_T            entryIndex,
                                       IPayload&         dst,
                                       IEntry::Marker_T& entryMarker ) noexcept
{
    ReadRequest::ByEntryIndexPayload     msgPayload( dst, entryIndex );
    Kit::Itc::SyncReturnHandler          srh;
    ReadRequest::RetrieveByEntryIndexMsg msg( *this, msgPayload, srh );
    m_myEventQueue.postSync( msg );

    entryMarker = msgPayload.m_markerEntryRetrieved;
    return msgPayload.m_success;
}


}  // end namespace
}
}
}
//------------------------------------------------------------------------------