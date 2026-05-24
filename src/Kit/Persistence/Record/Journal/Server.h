#ifndef KIT_PERSISTENCE_RECORD_JOURNAL_SERVER_H
#define KIT_PERSISTENCE_RECORD_JOURNAL_SERVER_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Dm/IModelPoint.h"
#include "Kit/Itc/OpenCloseSync.h"
#include "Kit/Persistence/Record/Journal/IEntry.h"
#include "Kit/Persistence/Record/Journal/IReaderRequest.h"
#include "Kit/Persistence/Record/Journal/ReaderSync.h"
#include "Kit/Persistence/Record/Journal/ResetSync.h"
#include "Kit/Container/RingBufferMP.h"
#include "Kit/Dm/ObserverCallback.h"

/** Maximum number of entries that can be written as the result of single
    change notification
 */
#ifndef OPTION_KIT_PERSISTENCE_JOURNAL_SERVER_MAX_BATCH_WRITE
#define OPTION_KIT_PERSISTENCE_JOURNAL_SERVER_MAX_BATCH_WRITE 4
#endif

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Journal {

/** This concrete template class implements the ITC messaging and Model Point
    monitoring needed to provide a thread-safe/asynchronous interface for
    Journal Records. A single Server instance manages a single EntryRecord
    instance. Multiple EntryRecords can be managed by creating multiple Server
    instances

    NOTE: The EntryRecords and HeadRecords are NOT managed by the Kit::Persistence::Record::Server.
          They are wholly managed by this class (Kit::Persistence::Record::Journal::Server)

    Template Args:
        ENTRY:=   The Application specific class that defines an 'entry'

 */
template <class ENTRY>
class Server : public Kit::Itc::OpenCloseSync,
               public ReaderSync,
               public ResetSync
{
public:
    /** Constructor.  The argument 'timingTickInMsec' specifies the timing
        resolution that will be used for Kit::Timer::Local Timers.

        NOTE: 'recordList' is variable length array where the last entry in the
              array MUST BE a nullptr.
     */
    Server( Kit::EventQueue::IQueue&             myEventQueue,
            IEntry&                              entryRecord,
            Kit::Container::RingBufferMP<ENTRY>& incomingEntriesBuffer ) noexcept
        : OpenCloseSync( myEventQueue )
        , ReaderSync( myEventQueue, entryRecord )
        , ResetSync( myEventQueue )
        , m_record( entryRecord )
        , m_obBuffer( myEventQueue )
        , m_buffer( incomingEntriesBuffer )
        , m_opened( false )
    {
        m_obBuffer.setCallback<Server, &Server::bufferElementCountChanged>( this );
    }

public:
    /// See Kit::Itc::IOpenRequest
    void request( OpenMsg& msg ) noexcept override
    {
        if ( !m_opened )
        {
            m_opened = true;
            m_buffer.m_mpElementCount.attach( m_obBuffer );
            m_record.start( m_eventQueue );
        }

        msg.returnToSender();
    }

    /// See Kit::Itc::ICloseRequest
    void request( CloseMsg& msg ) noexcept override
    {
        if ( m_opened )
        {
            m_opened = false;
            m_buffer.m_mpElementCount.detach( m_obBuffer );
            m_record.stop();
        }

        msg.returnToSender();
    }

public:
    /// See Kit::Persistence::Journal::RetrieveLatestRequest
    void request( RetrieveLatestMsg& msg ) noexcept override
    {
        RetrieveLatestRequest::Payload& payload = msg.getPayload();
        //
        payload.m_success = m_record.getLatest( payload.m_entryDst, payload.m_markerEntryRetrieved );
        msg.returnToSender();
    }

    /// See Kit::Persistence::Journal::RetrieveNextRequest
    void request( RetrieveNextMsg& msg ) noexcept override
    {
        RetrieveNextRequest::Payload& payload = msg.getPayload();
        //
        payload.m_success = m_record.getNext( payload.m_newerThan, payload.m_beginHereMarker, payload.m_entryDst, payload.m_markerEntryRetrieved );
        msg.returnToSender();
    }


    /// See Kit::Persistence::Journal::RetrievePreviousRequest
    void request( RetrievePreviousMsg& msg ) noexcept override
    {
        RetrievePreviousRequest::Payload& payload = msg.getPayload();
        //
        payload.m_success = m_record.getPrevious( payload.m_olderThan, payload.m_beginHereMarker, payload.m_entryDst, payload.m_markerEntryRetrieved );
        msg.returnToSender();
    }


    /// See Kit::Persistence::Journal::RetrieveByEntryIndexRequest
    void request( RetrieveByEntryIndexMsg& msg ) noexcept override
    {
        RetrieveByEntryIndexRequest::Payload& payload = msg.getPayload();
        //
        payload.m_success = m_record.getByEntryIndex( payload.m_index, payload.m_entryDst, payload.m_markerEntryRetrieved );
        msg.returnToSender();
    }

    /// See Kit::Persistence::Journal::IEntry
    Size_T maxIndex() const noexcept override
    {
        return m_record.getMaxIndex();  // Note: NO Critical section is required since this is a 'constant' value
    }

    /// See Kit::Persistence::Journal::LogicalResetRequest
    void request( LogicalResetMsg& msg ) noexcept override
    {
        LogicalResetRequest::Payload& payload = msg.getPayload();
        m_record.resetHead();
        payload.m_success = true;  // Note: resetHead() is void, so I assume it always succeeds
        msg.returnToSender();
    }

protected:
    /// Callback when the 'trigger' MP changes
    void bufferElementCountChanged( Kit::Dm::Mp::Uint32& mp, Kit::Dm::IObserver& observer ) noexcept
    {
        uint32_t count;
        uint16_t seqNum = 0;
        if ( mp.readAndSync( count, seqNum , observer ) && count > 0 )
        {
            // Temporarily disable callbacks to avoid un-necessary callbacks
            mp.detach( m_obBuffer );

            // Drain the buffer (but limit how many adds at one time) and write the entries to persistent storage
            unsigned iterations = 0;
            ENTRY    entry;
            while ( iterations < OPTION_KIT_PERSISTENCE_JOURNAL_SERVER_MAX_BATCH_WRITE && m_buffer.remove( entry) )
            {
                hookAddingEntry( entry );
                m_record.addEntry( entry );
                iterations++;
                count--;
            }

            // Re-enable callbacks to be notified of new (or still pending) entries
            if ( count > 0 )
            {
                // If there are still pending entries, use SEQUENCE_NUMBER_UNKNOWN to get an immediate callback
                seqNum = Kit::Dm::IModelPoint::SEQUENCE_NUMBER_UNKNOWN;
            }
            mp.attach( m_obBuffer, seqNum );
        }
    }

protected:
    /// Hook function called for each entry BEFORE it is added to the record.
    virtual void hookAddingEntry( const ENTRY& entry ) noexcept
    {
        // Default does nothing, but child class can override to add functionality
    }

protected:
    /// Indexed Entry Record that handles the actual work to read/write the data
    IEntry& m_record;

    /// Observer for change notification (to the RingBuffer)
    Kit::Dm::ObserverCallback<Dm::Mp::Uint32> m_obBuffer;

    /// The incoming entries RingBuffer
    Kit::Container::RingBufferMP<ENTRY>& m_buffer;

    /// Track my open state
    bool m_opened;
};


}  // end namespaces
}
}
}
#endif  // end header latch
