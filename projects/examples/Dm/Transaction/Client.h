#ifndef DM_TRANSACTION_CLIENT_H_
#define DM_TRANSACTION_CLIENT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "Dm/Transaction/MpFoo.h"
#include "Kit/Dm/IObserver.h"
#include "Kit/Itc/OpenCloseSync.h"
#include "Kit/Dm/ObserverCallback.h"
#include "Kit/EventQueue/IQueue.h"
#include "Kit/System/Timer.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Shutdown.h"
#include <inttypes.h>

/// Periodic Time interval, in milliseconds, for the Client to issue a request to the Server
#ifndef OPTION_DM_TRANSACTION_CLIENT_REQUEST_INTERVAL_MS
#define OPTION_DM_TRANSACTION_CLIENT_REQUEST_INTERVAL_MS 1000  // 1sec
#endif

/// Difference between the upper and lower limits
#ifndef OPTION_DM_TRANSACTION_CLIENT_LIMIT_RANGE
#define OPTION_DM_TRANSACTION_CLIENT_LIMIT_RANGE 10
#endif

/// Delta change - between transactions - for the Client's request limits (upper and lower)
#ifndef OPTION_DM_TRANSACTION_CLIENT_TRANSACTION_DELTA
#define OPTION_DM_TRANSACTION_CLIENT_TRANSACTION_DELTA 100
#endif

/// Number of interations to run the Client/Server transaction before terminating the application
#ifndef OPTION_DM_TRANSACTION_CLIENT_NUM_ITERATIONS
#define OPTION_DM_TRANSACTION_CLIENT_NUM_ITERATIONS 100
#endif


///
namespace Dm {
///
namespace Transaction {

/** This concrete class is a 'client' to request/trigger the 'server' to generate
    a random number.  See the README.md file for more details
 */
class Client : public Kit::Itc::OpenCloseSync, public Kit::System::Timer
{
public:
    /// Constructor
    Client( Kit::EventQueue::IQueue& myMbox,
            MpFoo&                   mpFoo )
        : Kit::Itc::OpenCloseSync( myMbox )
        , Kit::System::Timer( myMbox )
        , m_mpFoo( mpFoo )
        , m_obFoo( myMbox )
        , m_opened( false )
    {
        m_obFoo.setCallback<Client, &Client::triggerChanged>( this );
    }

public:
    /// See Kit::Itc::OpenCloseSync
    void request( OpenMsg& msg ) noexcept override
    {
        // Only 'open' when not already opened
        if ( !m_opened )
        {
            // Housekeeping
            m_opened         = true;
            m_offset         = 0;
            m_iterationCount = 0;

            // Send the first server request
            expired();
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
            Timer::stop();
            m_mpFoo.detach( m_obFoo );
        }

        // Return the message to the sender since we are done processing the request
        msg.returnToSender();
    }


protected:
    /// Callback for when the software timer expires
    void expired() noexcept override
    {
        // Check for the Application-Exit condition
        if ( m_iterationCount >= OPTION_DM_TRANSACTION_CLIENT_NUM_ITERATIONS )
        {
            Kit::System::Shutdown::success();
            return;
        }

        // Trigger the server request and subscribe for the server's response
        m_iterationCount++;
        int32_t  lowerLimit = m_offset;
        int32_t  upperLimit = m_offset + OPTION_DM_TRANSACTION_CLIENT_LIMIT_RANGE;
        uint16_t seqNum     = m_mpFoo.triggerRequest( upperLimit, lowerLimit );
        m_mpFoo.attach( m_obFoo, seqNum );
    }

    /// Callback when the 'trigger' MP changes
    void triggerChanged( MpFoo& mpThatChanged, Kit::Dm::IObserver& observer ) noexcept
    {
        Foo_T data;
        if ( mpThatChanged.readAndSync( data, observer ) )
        {
            KIT_SYSTEM_TRACE_MSG( "main",
                                  "client: RESPONSE. upper=%" PRId32 ", lower=%" PRId32 ", result=%" PRId32 ", isValid=%s",
                                  data.upperLimit,
                                  data.lowerLimit,
                                  data.randomNumber,
                                  data.isValid ? "true" : "false" );

            // Cancel the callback subscription since we got the response from the server
            mpThatChanged.detach( m_obFoo );

            // "reset" the MP for next transaction
            mpThatChanged.setInvalid();

            // Start the timer for the next transaction.
            m_offset += OPTION_DM_TRANSACTION_CLIENT_TRANSACTION_DELTA;
            Timer::start( OPTION_DM_TRANSACTION_CLIENT_REQUEST_INTERVAL_MS );
        }
    }

protected:
    /// MP for the Client/Server transaction
    MpFoo& m_mpFoo;

    /// Observer callback for the MP change notification
    Kit::Dm::ObserverCallback<MpFoo> m_obFoo;

    /// Offset for upper/lower limit for the next transaction
    int32_t m_offset;

    /// Iteration count for the Client/Server transaction
    unsigned m_iterationCount;

    /// Track my open/closed
    bool m_opened;
};

}  // end namespaces
}
#endif  // end header latch
