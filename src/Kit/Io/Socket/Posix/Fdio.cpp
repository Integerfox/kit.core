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
#include "Kit/System/Assert.h"
#include "Kit/System/Api.h"
#include "Kit/Text/FString.h"
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace Socket {
namespace Posix {


void Fdio::close( int& fd ) noexcept
{
    if ( fd != INVALID_FD )
    {
        ::shutdown( fd, SHUT_RDWR );
        ::close( fd );
        fd = INVALID_FD;
    }
}

bool Fdio::write( int fd, bool& eosFlag, const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept
{
    KIT_SYSTEM_ASSERT( buffer != nullptr );

    // Throw an error if the socket had already been closed
    if ( fd == INVALID_FD )
    {
        return false;
    }

    // Ignore write requests of ZERO bytes
    if ( maxBytes == 0 )
    {
        bytesWritten = 0;
        return true;
    }

    // perform the write
    ByteCount_T result = send( fd, (char*)buffer, maxBytes, MSG_NOSIGNAL );
    if ( result < 0 )
    {
        bytesWritten = 0;
        eosFlag      = true;
        return false;
    }
    if ( result == 0 )
    {
        bytesWritten = 0;
        eosFlag      = true;
        return false;
    }
    bytesWritten = result;
    eosFlag      = false;
    return true;
}

void Fdio::flush( int fd ) noexcept
{
    // Do not know how to implement using only Posix  (jtt 2-14-2015)
}

bool Fdio::isOpened( int fd ) noexcept
{
    return fd != INVALID_FD;
}

bool Fdio::read( int fd, bool& eosFlag, void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept
{
    KIT_SYSTEM_ASSERT( buffer != nullptr );

    // Throw an error if the socket had already been closed
    if ( fd == INVALID_FD )
    {
        return false;
    }

    // Ignore read requests of ZERO bytes
    if ( numBytes == 0 )
    {
        bytesRead = 0;
        return true;
    }

    // perform the read
    ByteCount_T result = recv( fd, (char*)buffer, numBytes, 0 );
    if ( result < 0 )
    {
        bytesRead = 0;
        eosFlag   = true;
        return false;
    }

    // Connection closed gracefully
    if ( result == 0 )
    {
        bytesRead = 0;
        eosFlag   = true;
        return false;
    }

    // Success!
    bytesRead = result;
    eosFlag   = false;
    return true;
}

bool Fdio::available( int fd ) noexcept
{
    // Trap that the stream has been CLOSED!
    if ( fd == INVALID_FD )
    {
        return false;
    }

    int nbytes;
    ioctl( fd, FIONREAD, &nbytes );
    return nbytes > 0;
}

int Fdio::createListeningSocket( int      port,
                                 unsigned numRetries,
                                 uint32_t delayBetweenRetriesMs ) noexcept
{
    struct sockaddr_in local;
    int                one = 1;
    int                result;

    // Create the Socket to listen with
    int fd = socket( AF_INET, SOCK_STREAM, 0 );
    if ( fd < 0 )
    {
        return fd;
    }

    // Set Options on the socket
    setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof( one ) );

    // Set the "address" of the socket
    unsigned retry = numRetries;
    while ( retry )
    {
        memset( &local, 0, sizeof( local ) );
        local.sin_family      = AF_INET;
        local.sin_addr.s_addr = htonl( INADDR_ANY );
        local.sin_port        = htons( port );
        if ( ( result = bind( fd, (struct sockaddr*)&local, sizeof( local ) ) ) >= 0 )
        {
            break;
        }

        // Delay between retry attempts
        Kit::System::sleep( delayBetweenRetriesMs );
        if ( --retry == 0 )
        {
            return result;
        }
    }

    // Create a queue to hold connection requests
    if ( ( result = ::listen( fd, SOMAXCONN ) ) < 0 )
    {
        close( fd );
        return result;
    }

    // Success!
    return fd;
}

int Fdio::acceptConnection( int listeningFd, struct sockaddr_in& client_addr ) noexcept
{
    // Wait on the 'accept'
    socklen_t client_len = sizeof( client_addr );
    return accept( listeningFd, (struct sockaddr*)&client_addr, &client_len );
}

int Fdio::enableKeepAlive( int fd ) noexcept
{
    int bOptVal = 1;
    int bOptLen = sizeof( int );
    return setsockopt( fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&bOptVal, bOptLen );
}

bool Fdio::resolveAddress( const char* remoteHostName, int portNumToConnectTo, struct addrinfo*& results ) noexcept
{
    Kit::Text::FString<5> port( portNumToConnectTo );
    struct addrinfo       hints;
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    return getaddrinfo( remoteHostName, port, &hints, &results ) == 0;
}

void Fdio::freeAddresses( struct addrinfo* results ) noexcept
{
    freeaddrinfo( results );
}

int Fdio::createSocket( struct addrinfo* addr ) noexcept
{
    return socket( addr->ai_family, addr->ai_socktype, addr->ai_protocol );
}

}  // end namespace
}
}
}
//------------------------------------------------------------------------------