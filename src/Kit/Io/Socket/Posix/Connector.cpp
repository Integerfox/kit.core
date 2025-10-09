/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Fdio.h"
#include "Kit/Io/Socket/Connector.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace Socket {

///////////////////////////////
Connector::Result_T Connector::establish( const char* remoteHostName, int portNumToConnectTo, KitIoSocketHandle_T& fdOut ) noexcept
{
    // Get a list of address candidates for the remote Host
    struct addrinfo* adapters = nullptr;
    if ( !Posix::Fdio::resolveAddress( remoteHostName, portNumToConnectTo, adapters ) )
    {
        return eERROR;
    }

    // Walk the list of addresses until a connection succeeds
    struct addrinfo* ptr;
    bool             failedCreateSocket = true;
    fdOut                               = Posix::Fdio::INVALID_FD;
    for ( ptr = adapters; ptr != nullptr; ptr = ptr->ai_next )
    {
        // Create a SOCKET for connecting to server
        fdOut = Posix::Fdio::createSocket( ptr );
        if ( fdOut < 0 )
        {
            continue;  // Try the next address
        }

        // If got here and still exited the loop with invalid file descriptor, it mostly likely means that the connect request was refused
        failedCreateSocket = false;

        // Attempt to connect to the remote host
        if ( connect( fdOut, ptr->ai_addr, ptr->ai_addrlen ) < 0 )
        {
            Posix::Fdio::close( fdOut );
            continue;  // Try the next address
        }

        // If I get here, I successfully connected to the remote Host
        break;
    }

    // Housekeeping
    Posix::Fdio::freeAddresses( adapters );

    // Check if a connection was made
    if ( fdOut == Posix::Fdio::INVALID_FD )
    {
        return failedCreateSocket ? eERROR : eREFUSED;
    }

    // If I get here, I have successful connection to the remote Host
    return eSUCCESS;
}

}  // end namespace
}
}
//------------------------------------------------------------------------------