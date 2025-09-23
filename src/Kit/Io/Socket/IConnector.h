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
namespace Cpl {
///
namespace Io {
///
namespace Socket {


/** This abstract class defines the interface for establishing/requesting
    a SIMPLE socket connection, i.e. make a "client connection".  A single
    instance can be used to create many connections.
 */
class IConnector
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
    virtual Result_T establish( const char* remoteHostName, int portNumToConnectTo, KitIoSocketHandle_T& fdOut ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IConnector() = default;
};


}      // end namespaces
}
}
#endif  // end header latch
