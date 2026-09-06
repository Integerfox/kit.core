#ifndef EXAMPLES_ITC_ASYNC_CANCEL_CLIENT_H_
#define EXAMPLES_ITC_ASYNC_CANCEL_CLIENT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Examples/Itc/AsyncCancel/IFlashRequest.h"
#include "Kit/Itc/OpenCloseSync.h"
#include "Kit/System/Assert.h"
#include "Kit/System/Semaphore.h"
#include "Kit/System/Trace.h"
#include <inttypes.h>

///
namespace Examples {
///
namespace Itc {
///
namespace AsyncCancel {


/** This concrete class is a asynchronous ITC client that cycles through sending
    different flash duty cycle requests to the Server that controls the LED
    flashing
 */
class Client : public IFlashResponse, public Kit::Itc::OpenCloseSync
{
public:
    /// Constructor
    Client( Kit::EventQueue::IQueue& clientsEventQueue,
            IFlashRequest::SAP&      serverSAP,
            FlashPattern_T           flashPatterns[],
            unsigned                 numFlashPatterns,
            Kit::System::Semaphore&  shutdownSignal )
        : Kit::Itc::OpenCloseSync( clientsEventQueue )
        , m_shutdownSignal( shutdownSignal )
        , m_serverSAP( serverSAP )
        , m_flashResponseMsg( *this, clientsEventQueue, serverSAP, m_flashPayload )
        , m_flashCancelResponseMsg( *this, clientsEventQueue, serverSAP, m_flashCancelPayload )
        , m_flashPatterns( flashPatterns )
        , m_numFlashPatterns( numFlashPatterns )
        , m_maxNumberOfRequests( 0 )
        , m_opened( false )
    {
        KIT_SYSTEM_ASSERT( m_flashPatterns != nullptr );
    }

public:
    /// See Kit::Itc::OpenCloseSync
    void request( OpenMsg& msg ) noexcept override
    {
        // Only 'open' when not already opened
        if ( !m_opened )
        {
            // Housekeeping
            m_opened            = true;
            m_flashPatternIndex = 0;
            m_closedMsg         = nullptr;

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

            // Cancel any pending flash request before closing
            KIT_SYSTEM_TRACE_MSG( "main", "Cancelling any pending flash request before closing." );
            m_closedMsg                           = &msg;
            m_flashCancelPayload.flashMsgToCancel = &m_flashResponseMsg.getRequestMsg();
            m_serverSAP.post( m_flashCancelResponseMsg.getRequestMsg() );
            return;  // The Close message will be returned once the cancel response is received
        }

        // Return the message to the sender since we are done processing the request
        msg.returnToSender();
    }

public:
    /// Flash Response
    void response( FlashMsg& msg ) noexcept override
    {
        // Ignore the response if the client is in the process of closing
        if ( m_closedMsg )
        {
            KIT_SYSTEM_TRACE_MSG( "main", "Ignoring flash response since client is closing." );
            return;
        }

        // Process the Response
        auto& payload = msg.getPayload();
        if ( payload.success )
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

        // End the application on error
        else
        {
            KIT_SYSTEM_TRACE_MSG( "main", "Flash Request FAILED. (idx=%" PRIu32 ")", m_flashPatternIndex );
            m_shutdownSignal.signal();
        }
    }

    /// Cancel Response
    void response( FlashCancelMsg& msg ) noexcept override
    {
        // Process the Cancel Response
        auto& payload = msg.getPayload();
        if ( payload.canceled )
        {
            KIT_SYSTEM_TRACE_MSG( "main", "Flash Cancel Request: Request cancelled." );
        }
        else
        {
            KIT_SYSTEM_TRACE_MSG( "main", "Flash Cancel Request: No pending request to cancel." );
        }

        // End the close() process by returning the Close msg
        if ( m_closedMsg )
        {
            KIT_SYSTEM_TRACE_MSG( "main", "Flash Client Closed." );
            m_closedMsg->returnToSender();
        }
    }

protected:
    /// Helper method: Send the rate request to the server
    void sendRequest() noexcept
    {
        // Buid the Request payload
        auto& payload        = m_flashResponseMsg.getPayload();
        payload.flashPattern = m_flashPatterns[m_flashPatternIndex];
        KIT_SYSTEM_TRACE_MSG( "main",
                              "Sending Rate Request (on=%" PRIu32 " ms, off=%" PRIu32 " ms, n=%" PRIu32 ")",
                              payload.flashPattern.onTimeMs,
                              payload.flashPattern.offTimeMs,
                              payload.flashPattern.repeatCount );

        // Increment the flash rate index for the next request
        incrementFlashRateIndex();

        // Send the request to the server
        m_serverSAP.post( m_flashResponseMsg.getRequestMsg() );
    }

    /// Helper
    void incrementFlashRateIndex() noexcept
    {
        m_flashPatternIndex = ( m_flashPatternIndex + 1 ) % m_numFlashPatterns;
    }

protected:
    /// Used to signal the main thread that the client has finished its work.
    Kit::System::Semaphore& m_shutdownSignal;

    /// Server Widget's SAP (aka its fully qualified 'handle')
    IFlashRequest::SAP& m_serverSAP;

    /// Flash Msg: Payload
    IFlashRequest::FlashPayload_T m_flashPayload;

    /// Flash Msg: Response message (which contains the Request message)
    IFlashResponse::FlashMsg m_flashResponseMsg;

    /// CANCEL Flash Msg: Payload
    IFlashRequest::FlashCancelPayload_T m_flashCancelPayload;

    /// CANCEL Flash Msg: Response message (which contains the Request message)
    IFlashResponse::FlashCancelMsg m_flashCancelResponseMsg;

    /// Cached closed msg
    CloseMsg* m_closedMsg;

    /// Flash rate table
    FlashPattern_T* m_flashPatterns;

    /// Index of the current flash rate
    uint32_t m_flashPatternIndex;

    /// Flash rate table count
    unsigned m_numFlashPatterns;

    /// Maximum number of requests to send
    int m_maxNumberOfRequests;

    /// Track my open/closed
    bool m_opened;
};

}  // end namespaces
}
}
#endif  // end header latch
