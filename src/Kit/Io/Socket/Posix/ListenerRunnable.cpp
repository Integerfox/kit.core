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
#include "Kit/Io/Socket/ListenerRunnable.h"

#define SECT_ "Cpl::Io::Socket::Posix"

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace Socket {


///////////////////////////////
void ListenerRunnable::stopListener() noexcept
{
    Posix::Fdio::close( m_listeningSocket );
}


void ListenerRunnable::listen() noexcept
{
    // Create the listening socket
    m_listeningSocket = Posix::Fdio::createListeningSocket( m_port, USE_KIT_IO_SOCKET_LISTENER_BIND_RETRIES, USE_KIT_IO_SOCKET_LISTENER_BIND_RETRY_WAIT_MS );
    if ( m_listeningSocket == Posix::Fdio::INVALID_FD )
    {
        // TODO: Add logging/tracing
        return;
    }

    // Start listening for connections
    for ( ;; )
    {
        struct sockaddr_in client_addr;

        // Wait on the 'accept'
        int newfd;
        if ( ( newfd = Posix::Fdio::acceptConnection( m_listeningSocket, client_addr ) ) < 0 )
        {
            // TODO: Add logging/tracing
            break;
        }

        // Enable SO_KEEPALIVE so we know when the client terminated the TCP session
        int bOptVal = 1;
        int bOptLen = sizeof( int );
        if ( setsockopt( newfd, SOL_SOCKET, SO_KEEPALIVE, (char*)&bOptVal, bOptLen ) < 0 )
        {
            // CPL_SYSTEM_TRACE_ALLOCATE( int, sockerr, errno );
            // CPL_SYSTEM_TRACE_MSG( SECT_, ( "Cpl::Io::Socket::Posix::Listener:: start() - Failed enable SO_KEEPALIVE. errCode=(%X) %s.", sockerr, strerror( sockerr ) ) );
        }

        // Create a Descriptor for the accepted connection and pass it to the client
        Cpl::Io::Descriptor streamFd( newfd );
        if ( !m_clientPtr->newConnection( streamFd, inet_ntoa( client_addr.sin_addr ) ) )
        {
            ::shutdown( newfd, 2 );  // 2==SHUT_RDWR
            ::close( newfd );
            continue;
        }
    }


    // Clean-up
    stopListener();
}

}  // end namespace
}
}
//------------------------------------------------------------------------------