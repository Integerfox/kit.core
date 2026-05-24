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
#include "IReaderRequest.h"
#include "Kit/Itc/SyncReturnHandler.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Persistence {
namespace Record {
namespace Journal {

bool ReaderSync::retrieveLatest( IPayload& dst, IEntry::Marker_T& entryMarker ) noexcept
{
    RetrieveLatestRequest::Payload           msgPayload( dst );
    Kit::Itc::SyncReturnHandler              srh;
    RetrieveLatestRequest::RetrieveLatestMsg msg( *this, msgPayload, srh );
    m_myEventQueue.postSync( msg );

    entryMarker = msgPayload.m_markerEntryRetrieved;
    return msgPayload.m_success;
}

bool ReaderSync::retrieveNext( uint64_t               newerThanTimestamp,
                               const IEntry::Marker_T beginHereMarker,
                               IPayload&              dst,
                               IEntry::Marker_T&      entryMarker ) noexcept
{
    IEntry::Marker_T                     beginHere = beginHereMarker;
    RetrieveNextRequest::Payload         msgPayload( dst, beginHere, newerThanTimestamp );
    Kit::Itc::SyncReturnHandler          srh;
    RetrieveNextRequest::RetrieveNextMsg msg( *this, msgPayload, srh );
    m_myEventQueue.postSync( msg );

    entryMarker = msgPayload.m_markerEntryRetrieved;
    return msgPayload.m_success;
}


bool ReaderSync::retrievePrevious( uint64_t               olderThanTimestamp,
                                   const IEntry::Marker_T beginHereMarker,
                                   IPayload&              dst,
                                   IEntry::Marker_T&      entryMarker ) noexcept
{
    IEntry::Marker_T                             beginHere = beginHereMarker;
    RetrievePreviousRequest::Payload             msgPayload( dst, beginHere, olderThanTimestamp );
    Kit::Itc::SyncReturnHandler                  srh;
    RetrievePreviousRequest::RetrievePreviousMsg msg( *this, msgPayload, srh );
    m_myEventQueue.postSync( msg );

    entryMarker = msgPayload.m_markerEntryRetrieved;
    return msgPayload.m_success;
}

bool ReaderSync::retrieveByEntryIndex( Size_T            entryIndex,
                                       IPayload&         dst,
                                       IEntry::Marker_T& entryMarker ) noexcept
{
    RetrieveByEntryIndexRequest::Payload                  msgPayload( dst, entryIndex );
    Kit::Itc::SyncReturnHandler                           srh;
    RetrieveByEntryIndexRequest::RetrieveByEntryIndexMsg msg( *this, msgPayload, srh );
    m_myEventQueue.postSync( msg );

    entryMarker = msgPayload.m_markerEntryRetrieved;
    return msgPayload.m_success;
}

Size_T ReaderSync::maxIndex() const noexcept
{
    // NOTE: No ITC message is needed to get the max index since the max index
    //       is const data and is set when the Entry Record is constructed
    return m_entry.getMaxIndex();
}


}  // end namespace
}
}
}
//------------------------------------------------------------------------------