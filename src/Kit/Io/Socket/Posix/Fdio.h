#ifndef KIT_IO_SOCKET_POSIX_FDIO_H_
#define KIT_IO_SOCKET_POSIX_FDIO_H_
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
#include "Kit/System/Assert.h"
#include "Kit/System/Api.h"
#include "Kit/Text/FString.h"
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>


///
namespace Kit {
///
namespace Io {
///
namespace Socket {
///
namespace Posix {


//////////////////////
/** This static class provides a collection of functions for operating on POSIX
    file descriptors for BSD sockets.
 */
class Fdio
{
public:
    /// Used to indicate an invalid file descriptor
    static constexpr int INVALID_FD = -1;

    /** Closes the file descriptor 'fd'. If 'fd' is already closed (i.e. INVALID_FD),
        the method is a NOP.  Upon a successful close, 'fd' is set to INVALID_FD.
     */
    inline static void close( int& fd ) noexcept
    {
        if ( fd != INVALID_FD )
        {
            ::shutdown( fd, SHUT_RDWR );
            ::close( fd );
            fd = INVALID_FD;
        }
    }

public:
    /** Attempts to write up to 'maxBytes' from 'buffer' to the file descriptor
        'fd'. The actual number of bytes written is returned in 'bytesWritten'.

        The 'eosFlag' is set to true if End-of-Stream was encountered, else it is
        cleared (i.e. set to false)

        The method blocks until at least one byte is written to the file descriptor.

        Returns true if successful; else false on error.
     */
    static bool write( int fd, bool& eosFlag, const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept
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
        bytesWritten = send( fd, (char*)buffer, maxBytes, MSG_NOSIGNAL );
        eosFlag      = bytesWritten <= 0;
        return !eosFlag;
    }

    /// Flushes the file descriptor 'fd'
    inline static void flush( int fd ) noexcept
    {
        // Do not know how to implement using only Posix  (jtt 2-14-2015)
    }

    /** Returns true if the file descriptor is in the open state
     */
    inline static bool isOpened( int fd ) noexcept
    {
        return fd != INVALID_FD;
    }

public:
    /** The method attempts to read up to 'numBytes' from the file descriptor.  The
        actual number of bytes read is returned via 'bytesRead'

        The 'eosFlag' is set to true if End-of-Stream was encountered, else it is
        cleared (i.e. set to false)

        The method blocks until at least one byte is read from the file descriptor.

        Returns true if successful; else false on error.
     */

    static bool read( int fd, bool& eosFlag, void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept
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
        bytesRead = recv( fd, (char*)buffer, numBytes, 0 );
        eosFlag   = bytesRead <= 0;
        return !eosFlag;
    }


    /** Returns true if there is data available to be read from the file descriptor
     */
    inline static bool available( int fd ) noexcept
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

public:
    /** Creates the "listening" socket.  It will attempt N times to binding the
        listening port before giving up.  Upon success a valid file descriptor
        is returned (>=0).  On error a negative value is returned
     */
    static int createListeningSocket( int      port,
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
            return result;
        }

        // Success!
        return fd;
    }

    /** This method is to accept an incoming connection request.  Returns
        a valid file descriptor for the accepted connection, or < 0 if an error
        occurred.  When successful, the 'client_addr' structure is populated
        with information about the remote Host.
     */
    inline static int acceptConnection( int listeningFd, struct sockaddr_in& client_addr ) noexcept
    {
        // Wait on the 'accept'
        socklen_t client_len = sizeof( client_addr );
        return accept( listeningFd, (struct sockaddr*)&client_addr, &client_len );
    }

    /** This method enables the SO_KEEPALIVE option on the specified socket.
        Returns >=0 if successful, else on error < 0 is returned
     */
    inline static int enableKeepAlive( int fd ) noexcept
    {
        int bOptVal = 1;
        int bOptLen = sizeof( int );
        return setsockopt( fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&bOptVal, bOptLen );
    }

public:
    /** This resolved the specific network address string and port number (IPv4
        or IPv6) to a socket address info structure. The method returns true if
        successful, else false is returned.

        When successful, the method returns one or MORE address info structure
        candidates via the 'results' argument. In addition on success, the caller
        is required to freeAddresses() method to free the memory associated
        with the 'results' argument.
     */
    inline static bool resolveAddress( const char* remoteHostName, int portNumToConnectTo, struct addrinfo*& results ) noexcept
    {
        Kit::Text::FString<5> port( portNumToConnectTo );
        struct addrinfo       hints;
        memset( &hints, 0, sizeof( hints ) );
        hints.ai_family   = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        return getaddrinfo( remoteHostName, port, &hints, &results ) == 0;
    }

    /** This method frees the memory associated with the 'results' argument
        that was populated by a previous call to the resolveAddress() method.
     */
    inline static void freeAddresses( struct addrinfo* results ) noexcept
    {
        freeaddrinfo( results );
    }

    /** Create a file descriptor for the specified socket address info.
        Returns a valid file descriptor if successful, else < 0 is returned.
     */
    inline static int createSocket( struct addrinfo* addr ) noexcept
    {
        return socket( addr->ai_family, addr->ai_socktype, addr->ai_protocol );
    }
};

}  // end namespaces
}
}
}
#endif  // end header latch