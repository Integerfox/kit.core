#ifndef KIT_IO_SOCKET_WIN32_FDIO_H_
#define KIT_IO_SOCKET_WIN32_FDIO_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

/// Disable able 'deprecated warnings' for use of inet_ntoa() function
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "Kit/System/Assert.h"
#include "Kit/System/Api.h"
#include "Kit/Io/Types.h"
#include "Kit/Text/FString.h"

///
namespace Kit {
///
namespace Io {
///
namespace Socket {
///
namespace Win32 {


//////////////////////
/** This static class provides a collection of functions for operating on Win32
    file descriptors for BSD sockets.
 */
class Fdio
{
public:
    /** Closes the file descriptor 'fd'. If 'fd' is already closed (i.e. INVALID_SOCKET),
        the method is a NOP.  Upon a successful close, 'fd' is set to INVALID_SOCKET.
     */
    static void close( SOCKET& fd ) noexcept;

public:
    /** Attempts to write up to 'maxBytes' from 'buffer' to the file descriptor
        'fd'. The actual number of bytes written is returned in 'bytesWritten'.

        The 'eosFlag' is set to true if End-of-Stream was encountered, else it is
        cleared (i.e. set to false)

        The method blocks until at least one byte is written to the file descriptor.

        Returns true if successful; else false on error.
     */
    static bool write( SOCKET fd, bool& eosFlag, const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept
    {
        KIT_SYSTEM_ASSERT( buffer != nullptr );

        // Throw an error if the socket had already been closed
        if ( fd == INVALID_SOCKET )
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
        int result = send( fd, (char*)buffer, maxBytes, 0 );
        if ( result == SOCKET_ERROR || result == 0 )  // Error or connection closed gracefully
        {
            bytesWritten = 0;
            eosFlag      = true;
            return false;
        }
        
        // Success!
        bytesWritten = static_cast<ByteCount_T>(result);
        eosFlag      = false;
        return true;
    }

    /// Flushes the file descriptor 'fd'
    inline static void flush( SOCKET fd ) noexcept
    {
        // I could use WSAIoctl() here with SIO_FLUSH - but according
        // to the Microsoft documentation - WSAIoctrl w/SIO_FLUSH could
        // block (unless using overlapped IO) - which is not the designed
        // behavior for this call -->so we will skip it for now (jtt 2-14-2015)
    }

    /** Returns true if the file descriptor is in the open state
     */
    inline static bool isOpened( SOCKET fd ) noexcept
    {
        return fd != INVALID_SOCKET;
    }

public:
    /** The method attempts to read up to 'numBytes' from the file descriptor.  The
        actual number of bytes read is returned via 'bytesRead'

        The 'eosFlag' is set to true if End-of-Stream was encountered, else it is
        cleared (i.e. set to false)

        The method blocks until at least one byte is read from the file descriptor.

        Returns true if successful; else false on error.
     */

    static bool read( SOCKET fd, bool& eosFlag, void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept
    {
        KIT_SYSTEM_ASSERT( buffer != nullptr );

        // Throw an error if the socket had already been closed
        if ( fd == INVALID_SOCKET )
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
        int result = recv( fd, (char*)buffer, numBytes, 0 );
        if ( result == SOCKET_ERROR || result == 0 )  // Error or connection closed gracefully
        {
            bytesRead = 0;
            eosFlag   = true;
            return false;
        }

        // Success!
        bytesRead = static_cast<ByteCount_T>(result);
        eosFlag   = false;
        return true;
    }


    /** Returns true if there is data available to be read from the file descriptor
     */
    inline static bool available( SOCKET fd ) noexcept
    {
        unsigned long nbytes = 1;  // NOTE: If there is error -->then I will return true
        ioctlsocket( fd, FIONREAD, &nbytes );
        return nbytes > 0 ? true : false;
    }

public:
    /** Creates the "listening" socket.  It will attempt N times to binding the
        listening port before giving up.  Upon success a valid socket descriptor
        is returned.  On error INVALID_SOCKET is returned
     */
    static SOCKET createListeningSocket( int      port,
                                         unsigned numRetries,
                                         uint32_t delayBetweenRetriesMs ) noexcept
    {
        struct sockaddr_in local;
        int                one = 1;

        // Create the Socket to listen with
        SOCKET fd = socket( AF_INET, SOCK_STREAM, 0 );
        if ( fd == INVALID_SOCKET )
        {
            return INVALID_SOCKET;
        }

        // Set Options on the socket
        setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, (char*)&one, sizeof( one ) );

        // Set the "address" of the socket
        unsigned retry = numRetries;
        while ( retry )
        {
            memset( &local, 0, sizeof( local ) );
            local.sin_family      = AF_INET;
            local.sin_addr.s_addr = htonl( INADDR_ANY );
            local.sin_port        = htons( port );
            if ( bind( fd, (struct sockaddr*)&local, sizeof( local ) ) != SOCKET_ERROR )
            {
                break;
            }

            // Delay between retry attempts
            Kit::System::sleep( delayBetweenRetriesMs );
            if ( --retry == 0 )
            {
                return INVALID_SOCKET;
            }
        }

        // Create a queue to hold connection requests
        if ( ::listen( fd, SOMAXCONN ) == SOCKET_ERROR )
        {
            return INVALID_SOCKET;
        }

        // Success!
        return fd;
    }

    /** This method is to accept an incoming connection request.  Returns
        a valid socket descriptor for the accepted connection, or INVALID_SOCKET
        if an error occurred.  When successful, the 'client_addr' structure is
        populated with information about the remote Host.
     */
    inline static SOCKET acceptConnection( SOCKET listeningFd, struct sockaddr_in& client_addr ) noexcept
    {
        // Wait on the 'accept'
        socklen_t client_len = sizeof( client_addr );
        return accept( listeningFd, (struct sockaddr*)&client_addr, &client_len );
    }

    /** This method enables the SO_KEEPALIVE option on the specified socket.
        Returns 0 if successful, else on error SOCKET_ERROR is returned
     */
    inline static int enableKeepAlive( SOCKET fd ) noexcept
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
        Returns a valid socket descriptor if successful, else INVALID_SOCKET is returned.
     */
    inline static SOCKET createSocket( struct addrinfo* addr ) noexcept
    {
        return socket( addr->ai_family, addr->ai_socktype, addr->ai_protocol );
    }
};

}  // end namespaces
}
}
}
#endif  // end header latch