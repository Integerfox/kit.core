#ifndef DM_TRANSACTION_SERVER_H_
#define DM_TRANSACTION_SERVER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Dm/Transaction/MpFoo.h"
#include "Kit/Dm/ObserverCallback.h"
#include "Kit/Itc/OpenCloseSync.h"
#include "Kit/EventQueue/IQueue.h"
#include "Kit/System/Trace.h"
#include "Kit/Bsp/Api.h"
#include <inttypes.h>

///
namespace Dm {
///
namespace Transaction {

/** This concrete class is a 'server' that responds to requests from a 'client'
    to generate.  See the README.md file for more details
 */
class Server : public Kit::Itc::OpenCloseSync
{
public:
    /// Constructor
    Server( Kit::EventQueue::IQueue& myMbox,
            MpFoo&                   mpFoo )
        : Kit::Itc::OpenCloseSync( myMbox )
        , m_mpFoo( mpFoo )
        , m_obFoo( myMbox )
        , m_opened( false )
    {
        m_obFoo.setCallback<Server, &Server::triggerChanged>( this );
    }

public:
    /// See Kit::Itc::OpenCloseSync
    void request( OpenMsg& msg ) noexcept override
    {
        // Only 'open' when not already opened
        if ( !m_opened )
        {
            // Housekeeping
            m_opened = true;

            // Subscribe for the 'trigger' MP change notification.
            m_mpFoo.attach( m_obFoo );
        }

        // Return the message to the sender since we are done processing the request
        msg.returnToSender();
    }

    /// See Kit::Itc::OpenCloseSync
    void request( CloseMsg& msg ) noexcept override
    {
        // Only 'close' when already opened
        if ( m_opened )
        {
            // Housekeeping
            m_opened = false;

            // Make sure the all of the potential callbacks get cancelled
            m_mpFoo.detach( m_obFoo );
        }

        // Return the message to the sender since we are done processing the request
        msg.returnToSender();
    }


protected:
    /// Callback when the 'trigger' MP changes
    void triggerChanged( MpFoo& mpThatChanged, Kit::Dm::IObserver& observer ) noexcept
    {
        Foo_T data;
        if ( mpThatChanged.readAndSync( data, observer ) )
        {
            // 'Alive' feedback when running on hardware
            Bsp_toggle_debug1();

            // Generate a Random number between the upper and lower limits and write to the MP
            // TODO: Need a Random Number interface
            data.randomNumber = ( ( data.upperLimit - data.lowerLimit ) / 2 ) + data.lowerLimit;
            data.isValid      = true;  // For this example - the result is always valid (because I am lazy)
            KIT_SYSTEM_TRACE_MSG( "main",
                                  "SERVER: request.  upper=%" PRId32 ", lower=%" PRId32 ", result=%" PRId32 ", isValid=%s",
                                  data.upperLimit,
                                  data.lowerLimit,
                                  data.randomNumber,
                                  data.isValid ? "true" : "false" );

            // Update the MP with response AND make sure that a change notification
            // gets generated (i.e. the 'true' argument).  Why is the 'force'
            // required? It's because change notifications are only generated when
            // the MP value is changed.  It is theoretically possible that the
            // server's random-number/valid-invalid state values do NOT change
            // the MP actual value.  Hence, the 'force' argument is used to
            // make sure that the Client gets notified of the response.
            mpThatChanged.detach( m_obFoo );  // Detach so we don't get callback for WRITE
            uint16_t seqNum = mpThatChanged.write( data, true );
            mpThatChanged.attach( m_obFoo, seqNum );  // Re-attach for the next request
        }
    }

protected:
    /// MP for the Client/Server transaction
    MpFoo& m_mpFoo;

    /// Observer callback for the MP change notification
    Kit::Dm::ObserverCallback<MpFoo> m_obFoo;

    /// Track my open/closed
    bool m_opened;
};

}  // end namespaces
}
#endif  // end header latch
