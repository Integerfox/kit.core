#ifndef KIT_IO_SOCKET_ILISTENER_CLIENT_H_
#define KIT_IO_SOCKET_ILISTENER_CLIENT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/Types.h"


///
namespace Kit {
///
namespace Io {
///
namespace Socket {


/** This abstract class defines the callback mechanism used for accepting incoming
    socket connections.

    This is interface is NOT thread safe.

    In-thread Client:
        It is okay for the client to be running in the same thread as the
        listener.  However, the client should NOT return from the newConnection()
        method until the client is 'done' with the socket.  

    Out-of-thread Client:
        For this use case, the client should NOT inherit/implement this class
        directly because it is NOT thread safe. Instead, the client should inherit
        from the ListenerClientSync class and then implement the request(msg)
        method.
 */
class IListenerClient
{
public:
    /** This method is a callback method that is called when the listener
        has  accepted in incoming socket request.  It is up the client to
        determine if the application will accept or reject the socket.  If
        the client rejects the socket, it needs to return false, else
        returns true.
     */
    virtual bool newConnection( KitIoSocketHandle_T& newFd, const char* rawConnectionInfo ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IListenerClient() = default;
};


}  // end namespaces
}
}
#endif  // end header latch
