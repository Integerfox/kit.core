#ifndef KIT_IO_SOCKET_LISTENER_CLIENT_SYNC_H_
#define KIT_IO_SOCKET_LISTENER_CLIENT_SYNC_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/Socket/IListenerClient.h"
#include "Kit/Io/Socket/IListenerClientRequest.h"
#include "Kit/EventQueue/IQueue.h"
#include "Kit/Io/Types.h"


///
namespace Kit {
///
namespace Io {
///
namespace Socket {


/** This partially concrete class implements the synchronous ITC interface
    for the Listener Client.  A child class is still required to implement the ITC
    request() method.
 */
class ListenerClientSync : public IListenerClient,
    public IListenerClientRequest
{
protected:
    /// Mailbox (of the server implementing the ITC request() method)
    Kit::EventQueue::IQueue& m_eventQueue;


public:
    /// Constructor
    ListenerClientSync( Kit::EventQueue::IQueue& myEventQueue ) noexcept;


public:
    /// See Kit::Io::Socket::Listener::Client
    bool newConnection( KitIoSocketHandle_T newFd, const char* rawConnectionInfo ) noexcept override;

};



}      // end namespaces
}
}
#endif  // end header latch
