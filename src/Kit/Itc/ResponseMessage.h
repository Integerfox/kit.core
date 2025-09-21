#ifndef KIT_ITC_RESPONSE_MESSAGE_H_
#define KIT_ITC_RESPONSE_MESSAGE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Itc/AsyncReturnHandler.h"
#include "Kit/Itc/RequestMessage.h"


///
namespace Kit {
///
namespace Itc {


/** This template class represents a client response message, which is posted
    to the client's mailbox after the corresponding server request message is
    returned to the client via the server message returnToSender interface. The
    purpose of this class is to provide a standard model for confirmed
    asynchronous messages. The SERVICE template argument is the type of the
    server interface that is used by the corresponding server request message.
    The CLIENT class type is the interface which is invoked by the response
    message when the client thread invokes the process routine inherited from
    the Message class.The CLIENT interface must contain a function named
    "response", which returns nothing (void) and takes a single argument, which
    is a reference to this template class type.
 */
template <class CLIENT, class SERVICE, class PAYLOAD>
class ResponseMessage : public IMessage
{
public:
    /// Constructor
    ResponseMessage( CLIENT& client, EventQueue::IQueue& clientsEventQueue, SERVICE& server, PAYLOAD& payload )
        : m_client( client ), m_rh( clientsEventQueue, *this ), m_request( server, payload, m_rh )
    {
    }

    /// Constructor
    ResponseMessage( CLIENT& client, EventQueue::IQueue& clientsEventQueue, SAP<SERVICE>& serverSap, PAYLOAD& payload )
        : m_client( client ), m_rh( clientsEventQueue, *this ), m_request( serverSap, payload, m_rh )
    {
    }

public:
    /// See Message
    void process() noexcept override
    {
        m_client.response( *this );
    }


public:
    /// Returns a reference to the contained server-request-message
    RequestMessage<SERVICE, PAYLOAD>& getRequestMsg()
    {
        return m_request;
    }

    /// Returns a reference the payload associated with this request/response
    PAYLOAD& getPayload()
    {
        return m_request.getPayload();
    }

protected:
    /// Reference to the client interface whose "response" method will be called
    CLIENT& m_client;

    /// Return handler used to deliver the response
    AsyncReturnHandler m_rh;

    /// I contain the actual instantiated server request message!
    RequestMessage<SERVICE, PAYLOAD> m_request;
};


}  // end namespaces
}
#endif  // end header latch
