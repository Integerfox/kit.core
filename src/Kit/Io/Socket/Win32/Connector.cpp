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
    if ( !Win32::Fdio::resolveAddress( remoteHostName, portNumToConnectTo, adapters ) )
    {
        return eERROR;
    }

    // Walk the list of addresses until a connection succeeds
    struct addrinfo* ptr;
    bool             createdSocket      = false;
    bool             sawRefused         = false;
    bool             sawConnectError    = false;
    fdOut                               = INVALID_SOCKET;
    for ( ptr = adapters; ptr != nullptr; ptr = ptr->ai_next )
    {
        // Create a SOCKET for connecting to server
        fdOut = Win32::Fdio::createSocket( ptr );
        if ( fdOut == INVALID_SOCKET )
        {
            continue;  // Try the next address
        }

        // Track that we successfully created at least one socket.
        createdSocket = true;

        // Attempt to connect to the remote host
        if ( connect( fdOut, ptr->ai_addr, ptr->ai_addrlen ) < 0 )
        {
            int err = WSAGetLastError();
            if ( err == WSAECONNREFUSED )
            {
                sawRefused = true;
            }
            else
            {
                sawConnectError = true;
            }
            Win32::Fdio::close( fdOut );
            continue;  // Try the next address
        }

        // If I get here, I successfully connected to the remote Host
        break;
    }

    // Housekeeping
    Win32::Fdio::freeAddresses( adapters );

    // Check if a connection was made
    if ( fdOut == INVALID_SOCKET )
    {
        if ( !createdSocket || sawConnectError )
        {
            return eERROR;
        }
        return sawRefused ? eREFUSED : eERROR;
    }

    // If I get here, I have successful connection to the remote Host
    return eSUCCESS;
}

}  // end namespace
}
}
//------------------------------------------------------------------------------