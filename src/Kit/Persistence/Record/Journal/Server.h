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

#include "Kit/Itc/OpenCloseSync.h"
#include "Kit/Persistence/Record/Journal/IEntry.h"
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

/** This concrete template class implements the ITC messaging (and Model Point
    monitoring needed to provide a thread-safe/asynchronous interface for
    Journal Records. A single Server instance manages a single EntryRecord
    instance. Multiple EntryRecords can be managed by creating multiple Server
    instances

    NOTE: EntryRecords are NOT managed by the Kit::Persistence::Record::Server.
          They managed by this class (Kit::Persistence::Record::Journal::Server)

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
        }

        msg.returnToSender();
    }

protected:
    /// Callback when the 'trigger' MP changes
    void bufferElementCountChanged( Kit::Dm::Mp::Uint32& mp, Kit::Dm::IObserver& observer ) noexcept
    {
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
