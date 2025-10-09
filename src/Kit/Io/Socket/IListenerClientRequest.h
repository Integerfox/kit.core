#ifndef KIT_IO_SOCKET_ILISTENER_CLIENT_REQUEST_H_
#define KIT_IO_SOCKET_ILISTENER_CLIENT_REQUEST_H_
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
#include "Kit/Itc/RequestMessage.h"
#include "Kit/Itc/SAP.h"


///
namespace Kit {
///
namespace Io {
///
namespace Socket {

/** This abstract class define message types and payloads for a set of
    ITC services. The request() method(s) are to be implemented by the
    'server'
 */

class IListenerClientRequest
{
public:
    /// SAP for this API
    typedef Kit::Itc::SAP<IListenerClientRequest> SAP;


public:
    /// Payload for Message: newConnection
    struct Payload_T
    {
        /// IN: Raw Information about connection (passed to client)
        const char* m_rawConnectionInfo;

        /// IN: Accepted socket descriptor (passed to client)
        KitIoSocketHandle_T m_acceptedFd;

        /// OUT: Client's acceptance flag/status (returned to the listener)
        bool m_accepted;

        /// Constructor
        Payload_T( KitIoSocketHandle_T newfd, const char* rawConnectionInfo )
            : m_rawConnectionInfo( rawConnectionInfo )
            , m_acceptedFd( newfd )
            , m_accepted( false )
        {
        }
    };


    /// Message Type: newConnection
    typedef Kit::Itc::RequestMessage<IListenerClientRequest, Payload_T> NewConnectionMsg;

public:
    /// Request: NewConnection
    virtual void request( NewConnectionMsg& msg ) = 0;


public:
    ///
    virtual ~IListenerClientRequest() = default;
};


}  // end namespaces
}
}
#endif  // end header latch
