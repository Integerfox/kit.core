#ifndef KIT_PERSISTENCE_RECORD_JOURNAL_READER_SYNC_H
#define KIT_PERSISTENCE_RECORD_JOURNAL_READER_SYNC_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Record/Journal/IReader.h"
#include "Kit/Persistence/Record/Journal/IReaderRequest.h"

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Journal {

/** This concrete class provides synchronous ITC wrapper functions for the
    IReader interface.  A child class is required to implement the actual ITC
    message request methods.
*/
class ReaderSync : public IReader,
                   public ReadRequest
{
public:
    /// See Kit::Persistence::Record::Journal::IReader
    bool retrieveLatest( IPayload& dst, IEntry::Marker_T& entryMarker ) noexcept override;

    /// See Kit::Persistence::Record::Journal::IReader
    bool retrieveNext( uint64_t               newerThanTimestamp,
                       const IEntry::Marker_T beginHereMarker,
                       IPayload&              dst,
                       IEntry::Marker_T&      entryMarker ) noexcept override;

    /// See Kit::Persistence::Record::Journal::IReader
    bool retrievePrevious( uint64_t               olderThanTimestamp,
                           const IEntry::Marker_T beginHereMarker,
                           IPayload&              dst,
                           IEntry::Marker_T&      entryMarker ) noexcept override;

    /// See Kit::Persistence::Record::Journal::IReader
    bool retrieveByEntryIndex( Size_T            entryIndex,
                               IPayload&         dst,
                               IEntry::Marker_T& entryMarker ) noexcept override;

protected:
    /// Constructor.
    ReaderSync( Kit::EventQueue::IQueue& eventQueue ) noexcept
        : m_myEventQueue( eventQueue )
    {
    }

protected:
    /// IEventQueue of the child class implementing the IReaderRequest methods
    Kit::EventQueue::IQueue& m_myEventQueue;
};


}  // end namespaces
}
}
}
#endif  // end header latch
