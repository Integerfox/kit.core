#ifndef KIT_IO_SOCKET_ILISTENER_H_
#define KIT_IO_SOCKET_ILISTENER_H_
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

///
namespace Kit {
///
namespace Io {
///
namespace Socket {


/** This abstract class defines the interface for a SIMPLE socket listener. A
    socket listener 'listens' for potential TCP/IP socket connections.  When a
    request for a connection comes in, the listener notifies the client.  It is
    the client's responsibility to determine if the connection request is
    accepted as well as provide memory for the accepted connection. The client
    is also responsible for reclaiming the memory once the socket connection has
    been terminated.
 */
class IListener 
{
public:
    /// Starts the Listener listening.
    virtual void startListening( IListenerClient& client, int portNumToListenOn ) noexcept = 0;

    /// Shuts down the Listener.  
    virtual void terminate() noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IListener() {}
};


}      // end namespaces
}
}
#endif  // end header latch
