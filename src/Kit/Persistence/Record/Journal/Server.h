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

        NOTE: When the RingBufferMP 'incomingEntriesBuffer' is created it MUST
              to be created with the 'initializeMemory' constructor flags set to
              false - which is NOT the default behavior. This is because ENTRY
              is not struct - but a class with a vtable pointer.
     */
    Server( Kit::EventQueue::IQueue&             myEventQueue,
            IEntry&                              entryRecord,
            Kit::Container::RingBufferMP<ENTRY>& incomingEntriesBuffer ) noexcept
        : OpenCloseSync( myEventQueue )
        , ReaderSync( myEventQueue )
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
            m_opened = m_record.start( m_eventQueue );
            if ( m_opened )
            {
                m_buffer.m_mpElementCount.attach( m_obBuffer );
            }
        }

        // Success reflects whether the Record started successfully
        msg.getPayload().success = m_opened;
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

        // The close request never fails
        msg.getPayload().success = true;
        msg.returnToSender();
    }

public:
    /// See Kit::Persistence::Journal::RetrieveLatestRequest
    void request( RetrieveLatestMsg& msg ) noexcept override
    {
        ReadRequest::LatestPayload& payload = msg.getPayload();
        //
        payload.m_success = m_record.getLatest( payload.m_entryDst,
                                                payload.m_markerEntryRetrieved );
        msg.returnToSender();
    }

    /// See Kit::Persistence::Journal::RetrieveNextRequest
    void request( RetrieveNextMsg& msg ) noexcept override
    {
        ReadRequest::NextPayload& payload = msg.getPayload();
        //
        payload.m_success = m_record.getNext( payload.m_newerThan,
                                              payload.m_beginHereMarker,
                                              payload.m_entryDst,
                                              payload.m_markerEntryRetrieved );
        msg.returnToSender();
    }


    /// See Kit::Persistence::Journal::RetrievePreviousRequest
    void request( RetrievePreviousMsg& msg ) noexcept override
    {
        ReadRequest::PreviousPayload& payload = msg.getPayload();
        //
        payload.m_success = m_record.getPrevious( payload.m_olderThan,
                                                  payload.m_beginHereMarker,
                                                  payload.m_entryDst,
                                                  payload.m_markerEntryRetrieved );
        msg.returnToSender();
    }


    /// See Kit::Persistence::Journal::RetrieveByEntryIndexRequest
    void request( RetrieveByEntryIndexMsg& msg ) noexcept override
    {
        ReadRequest::ByEntryIndexPayload& payload = msg.getPayload();
        //
        payload.m_success = m_record.getByEntryIndex( payload.m_index,
                                                      payload.m_entryDst,
                                                      payload.m_markerEntryRetrieved );
        msg.returnToSender();
    }

    /// See Kit::Persistence::Journal::IReader
    Size_T maxIndex() const noexcept override
    {
        return m_record.getMaxIndex();  // Note: NO Critical section is required since this is a 'constant' value
    }

    /// See Kit::Persistence::Journal::LogicalResetRequest
    void request( LogicalResetMsg& msg ) noexcept override
    {
        LogicalResetRequest::Payload& payload = msg.getPayload();
        payload.m_success                     = m_record.resetHead();
        msg.returnToSender();
    }

protected:
    /// Callback when the 'trigger' MP changes
    void bufferElementCountChanged( Kit::Dm::Mp::Uint32& mp, Kit::Dm::IObserver& observer ) noexcept
    {
        uint32_t count;
        uint16_t seqNum = 0;
        if ( mp.readAndSync( count, seqNum, observer ) && count > 0 )
        {
            // Temporarily disable callbacks to avoid un-necessary callbacks
            mp.detach( m_obBuffer );

            // Drain the buffer (but limit how many adds at one time) and write the entries to persistent storage
            unsigned iterations = 0;
            ENTRY    entry;
            while ( iterations < OPTION_KIT_PERSISTENCE_JOURNAL_SERVER_MAX_BATCH_WRITE && hookRemovingEntry( entry ) )
            {
                if ( hookAddingEntry( entry ) )
                {
                    m_record.addEntry( entry );
                }
                iterations++;
                count--;
            }

            // Re-enable callbacks to be notified of new (or still pending) entries
            if ( iterations >= OPTION_KIT_PERSISTENCE_JOURNAL_SERVER_MAX_BATCH_WRITE && count > 0 )
            {
                // The batch limit was reached with entries still pending: use SEQUENCE_NUMBER_UNKNOWN to get an immediate
                // callback.  When hookRemovingEntry() stopped the drain, the still pending entries are NOT forced (which
                // would spin) - they are processed on the next change notification
                seqNum = Kit::Dm::IModelPoint::SEQUENCE_NUMBER_UNKNOWN;
            }
            mp.attach( m_obBuffer, seqNum );
        }
    }

protected:
    /** Hook function called to remove the next entry from the incoming
        buffer.  Draining the buffer stops when the method returns false.

        A child class can return false - WITHOUT removing an entry - to defer
        writing entries, e.g. to pause updates to persistent storage.  The
        deferred entries stay in the incoming buffer and are processed on the
        next change notification of the buffer, i.e. when the next entry is
        added to the buffer.

        @param entry Destination for the removed entry
        @return true when an entry was removed into 'entry'; else false (the
                buffer is empty or draining is deferred)
     */
    virtual bool hookRemovingEntry( ENTRY& entry ) noexcept
    {
        return m_buffer.remove( entry );
    }

    /** Hook function called for each entry BEFORE it is added to the record.
        A child class can modify the entry before it is written, or filter it
        out.  A filtered entry has already been removed from the incoming
        buffer, i.e. it is discarded (use hookRemovingEntry() to defer writing
        entries without discarding them).

        @param entry The entry that is about to be added
        @return true to add the entry to the record, false to filter it out
     */
    virtual bool hookAddingEntry( ENTRY& entry ) noexcept
    {
        // Default adds every entry unchanged
        return true;
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
