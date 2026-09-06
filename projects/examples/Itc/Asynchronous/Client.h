#ifndef ITC_ASYNCHRONOUS_CLIENT_H_
#define ITC_ASYNCHRONOUS_CLIENT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Itc/Asynchronous/IRateResponse.h"
#include "Kit/Itc/OpenCloseSync.h"
#include "Kit/System/Assert.h"
#include "Kit/System/Semaphore.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Timer.h"
#include <inttypes.h>

///
namespace Itc {
///
namespace Asynchronous {

/** This concrete class is a asynchronous ITC client that sets the server's
    widgets LED flash rate
 */
class Client : public IRateResponse, public Kit::Itc::OpenCloseSync, public Kit::System::Timer
{
public:
    /// Constructor
    Client( Kit::EventQueue::IQueue&             clientsEventQueue,
            Itc::Synchronous::IRateRequest::SAP& serverSAP,
            uint32_t                             flashRatesMs[],
            unsigned                             flashRatesCount,
            Kit::System::Semaphore&              shutdownSignal )
        : Kit::Itc::OpenCloseSync( clientsEventQueue )
        , Kit::System::Timer( clientsEventQueue )
        , m_shutdownSignal( shutdownSignal )
        , m_serverSAP( serverSAP )
        , m_rateResponseMsg( *this, clientsEventQueue, serverSAP, m_ratePayload )
        , m_flashRatesMs( flashRatesMs )
        , m_flashRatesCount( flashRatesCount )
        , m_maxNumberOfRequests( 0 )
        , m_opened( false )
    {
        KIT_SYSTEM_ASSERT( m_flashRatesMs != nullptr );
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
            m_flashRateIndex = 0;

            // Get the maximum number of requests from the message payload
            m_maxNumberOfRequests = static_cast<int>( reinterpret_cast<size_t>( msg.getPayload().args ) );

            // Send the first server request
            sendRequest();
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
            Timer::stop();

            // NOTE: The implementation is assuming that the Class is opened and
            //       closed ONLY once with respect to ITC message - but nothing
            //       prevents the application abusing these semantics. A more
            //       robust shutdown would ensure that any pending requests have
            //       been returned.  See the 'Itc/AsyncCancel' example for how to
            //       properly handle this scenario.
            m_rateResponseMsg.abandon();
        }

        // Return the message to the sender since we are done processing the request
        msg.returnToSender();
    }

public:
    /// Rate Response
    void response( RateMsg& msg ) noexcept override
    {
        // Process the Response
        auto& payload = msg.getPayload();
        if ( payload.success )
        {
            // Run each flash rate for 10 full on/off cycles
            Timer::start( payload.flashRateMs * 2 * 10 );
        }

        // End the application on error
        else
        {
            KIT_SYSTEM_TRACE_MSG( "main", "Rate Request FAILED. (delay=%" PRIu32 " ms)", payload.flashRateMs );
            m_shutdownSignal.signal();
        }
    }

protected:
    /// Timer callback
    void expired() noexcept override
    {
        // Limit the number of requests
        if ( --m_maxNumberOfRequests > 0 )
        {
            sendRequest();
        }

        // Shutdown the application when the maximum number of requests has been reached
        else
        {
            KIT_SYSTEM_TRACE_MSG( "main", "Maximum number of requests reached." );
            m_shutdownSignal.signal();
        }
    }

    /// Helper method: Send the rate request to the server
    void sendRequest() noexcept
    {
        // Buid the Request payload
        auto& payload       = m_rateResponseMsg.getPayload();
        payload.flashRateMs = m_flashRatesMs[m_flashRateIndex];
        KIT_SYSTEM_TRACE_MSG( "main", "Sending Rate Request (flashRate=%" PRIu32 " ms, idx=%" PRIu32 ")", payload.flashRateMs, m_flashRateIndex );

        // Increment the flash rate index for the next request
        incrementFlashRateIndex();

        // Send the request to the server
        m_serverSAP.post( m_rateResponseMsg.getRequestMsg() );
    }

    /// Helper
    void incrementFlashRateIndex() noexcept
    {
        m_flashRateIndex = ( m_flashRateIndex + 1 ) % m_flashRatesCount;
    }

protected:
    /// Used to signal the main thread that the client has finished its work.
    Kit::System::Semaphore& m_shutdownSignal;

    /// Server Widget's SAP (aka its fully qualified 'handle')
    Itc::Synchronous::IRateRequest::SAP& m_serverSAP;

    /// Rate Msg: Payload
    Itc::Synchronous::IRateRequest::RatePayload_T m_ratePayload;

    /// Rate Msg: Response message (which contains the Request message)
    IRateResponse::RateMsg m_rateResponseMsg;

    /// Flash rate table
    uint32_t* m_flashRatesMs;

    /// Index of the current flash rate
    uint32_t m_flashRateIndex;

    /// Flash rate table count
    unsigned m_flashRatesCount;

    /// Maximum number of requests to send
    int m_maxNumberOfRequests;

    /// Track my open/closed
    bool m_opened;
};

}  // end namespaces
}
#endif  // end header latch
