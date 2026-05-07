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
#include "Kit/System/FatalError.h"
#include "Kit/System/Shutdown.h"

/** Maximum number of supported Records the Manager supports. This value is used
    for error-checking only, i.e. to ensure the variable list recordList array
    is properly terminated with a nullptr.  This value is NOT used to limit the
    number of Records the Server can manage, i.e. the Server can handle an 
    unlimited number of Records.
 */
#ifndef OPTION_KIT_PERSISTENCE_RECORD_SERVER_MAX_RECORDS
#define OPTION_KIT_PERSISTENCE_RECORD_SERVER_MAX_RECORDS 64
#endif

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
            IRecord*                 recordList[] ) noexcept
        : OpenCloseSync( myEventQueue )
        , m_records( recordList )
        , m_opened( false )
    {
        // Ensure the record list is properly terminated with a nullptr
        for ( unsigned i = 0; i < OPTION_KIT_PERSISTENCE_RECORD_SERVER_MAX_RECORDS; i++ )
        {
            if ( m_records[i] == nullptr )
            {
                return;
            }
        }

        Kit::System::FatalError::log( Kit::System::Shutdown::eASSERT, "Kit::Persistence::Record::Server was given an invalid record list" );
    }

public:
    /// See Kit::Itc::IOpenRequest
    void request( OpenMsg& msg ) noexcept override
    {
        if ( !m_opened )
        {
            m_opened = true;

            // Start each record
            for ( unsigned i = 0; m_records[i] != nullptr && m_opened; i++ )
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
            for ( unsigned i = 0; m_records[i] != nullptr; i++ )
            {
                m_records[i]->stop();
            }
        }
        msg.returnToSender();
    }

protected:
    /** Variable length list of Records to manage.  The last item in the list
        must be ZERO to indicate the end-of-the list
     */
    IRecord** m_records;

    /// Track my open state
    bool m_opened;
};


}  // end namespaces
}
}
#endif  // end header latch