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
#include "Kit/System/Trace.h"
#include <cerrno>

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
        KIT_SYSTEM_TRACE_MSG( SECT_,
                              "listen(): Failed to creating listening socket. Port=%d, result=%d, errno=(%d) %s.",
                              m_port,
                              m_listeningSocket,
                              errno,
                              strerror( errno ) );
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
            KIT_SYSTEM_TRACE_MSG( SECT_,
                                  "listen(): acceptConnection() failed. Port=%d, result=%d, errno=(%d) %s.",
                                  m_port,
                                  newfd,
                                  errno,
                                  strerror( errno ) );
            // Abort listening
            break;
        }

        int result;
        if ( ( result = Posix::Fdio::enableKeepAlive( newfd ) ) < 0 )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_,
                                  "listen(): failed to enable keepAlive. Port=%d, result=%d, errno=(%d) %s.",
                                  m_port,
                                  result,
                                  errno,
                                  strerror( errno ) );
        }

        // Call the client back to see if it wants to accept the connection
        if ( !m_clientPtr->newConnection( newfd, inet_ntoa( client_addr.sin_addr ) ) )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_,
                                  "listen(): client declined connection request. Port=%d, remote=%s.",
                                  m_port,
                                  inet_ntoa( client_addr.sin_addr ) );
            Posix::Fdio::close( newfd );
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