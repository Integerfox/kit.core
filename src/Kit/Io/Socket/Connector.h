#ifndef KIT_IO_SOCKET_CONNECTOR_H_
#define KIT_IO_SOCKET_CONNECTOR_H_
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


/** This static class defines a platform independent interface for 
    establishing/requesting a SIMPLE socket connection, i.e. makes a "client
    connection" to a remote Host.

    Supports IPv4 and IPv6 connections.
 */
class Connector
{
public:
    /// Possible return codes when attempting to establish a connection
    enum Result_T {
        eSUCCESS=0,     /// Connection was successful
        eERROR,         /// Error occurred
        eREFUSED,       /// Connection request was refused by the remote Host
    };


public:
    /** Requests a client connection to the specified remote Host.  Upon
        success an opened 'socket handle' is returned (via 'fdOut') for the
        connection.
     */
    static Result_T establish( const char* remoteHostName, int portNumToConnectTo, KitIoSocketHandle_T& fdOut ) noexcept;
};


}      // end namespaces
}
}
#endif  // end header latch
