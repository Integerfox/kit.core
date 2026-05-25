#ifndef KIT_PERSISTENCE_RECORD_SERVER_H
#define KIT_PERSISTENCE_RECORD_SERVER_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/EventQueue/IQueue.h"
#include "Kit/Persistence/Record/IRecord.h"
#include "Kit/Itc/OpenCloseSync.h"
#include "Kit/System/Assert.h"

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {

/** This concrete class is responsible for starting and stopping a list
    of IRecords objects, i.e. provides a thread-safe execution model
    for the IRecords.
 */
class Server : public Kit::Itc::OpenCloseSync
{
public:
    /** Constructor.  The argument 'timingTickInMsec' specifies the timing
        resolution that will be used for Cpl::Timer::Local Timers.

        NOTE: 'recordList' is variable length array where the last entry in the
              array MUST BE a nullptr.
     */
    Server( Kit::EventQueue::IQueue& myEventQueue,
            IRecord*                 recordList[],
            unsigned                 numRecords ) noexcept
        : OpenCloseSync( myEventQueue )
        , m_records( recordList )
        , m_numRecords( numRecords )
        , m_opened( false )
    {
        KIT_SYSTEM_ASSERT( recordList );
    }

public:
    /// See Kit::Itc::IOpenRequest
    void request( OpenMsg& msg ) noexcept override
    {
        if ( !m_opened )
        {
            m_opened = true;

            // Start each record
            for ( unsigned i = 0; i < m_numRecords && m_opened; i++ )
            {
                m_opened &= m_records[i]->start( m_eventQueue );
            }
        }

        msg.returnToSender();
    }

    /// See Kit::Itc::ICloseRequest
    void request( CloseMsg& msg ) noexcept override
    {
        if ( m_opened )
        {
            m_opened = false;

            // Stop each record
            for ( unsigned i = 0; i < m_numRecords; i++ )
            {
                m_records[i]->stop();
            }
        }
        msg.returnToSender();
    }

protected:
    /// Variable length list of Records to manage.  T
    IRecord** m_records;

    /// Number of elements in the record list
    unsigned m_numRecords;

    /// Track my open state
    bool m_opened;
};


}  // end namespaces
}
}
#endif  // end header latch