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
#include <unistd.h>
#include <netinet/in.h>
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
    static void close( int& fd ) noexcept;

public:
    /** Attempts to write up to 'maxBytes' from 'buffer' to the file descriptor
        'fd'. The actual number of bytes written is returned in 'bytesWritten'.

        The 'eosFlag' is set to true if End-of-Stream was encountered, else it is
        cleared (i.e. set to false)

        The method blocks until at least one byte is written to the file descriptor.

        Returns true if successful; else false on error.
     */
    static bool write( int fd, bool& eosFlag, const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept;

    /// Flushes the file descriptor 'fd'
    static void flush( int fd ) noexcept;

    /// Returns true if the file descriptor is in the open state
    static bool isOpened( int fd ) noexcept;

public:
    /** The method attempts to read up to 'numBytes' from the file descriptor.  The
        actual number of bytes read is returned via 'bytesRead'

        The 'eosFlag' is set to true if End-of-Stream was encountered, else it is
        cleared (i.e. set to false)

        The method blocks until at least one byte is read from the file descriptor.

        Returns true if successful; else false on error.
     */
    static bool read( int fd, bool& eosFlag, void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept;


    /// Returns true if there is data available to be read from the file descriptor
    static bool available( int fd ) noexcept;

public:
    /** Creates the "listening" socket.  It will attempt N times to binding the
        listening port before giving up.  Upon success a valid file descriptor
        is returned (>=0).  On error a negative value is returned
     */
    static int createListeningSocket( int      port,
                                      unsigned numRetries,
                                      uint32_t delayBetweenRetriesMs ) noexcept;

    /** This method is to accept an incoming connection request.  Returns
        a valid file descriptor for the accepted connection, or < 0 if an error
        occurred.  When successful, the 'client_addr' structure is populated
        with information about the remote Host.
     */
    static int acceptConnection( int listeningFd, struct sockaddr_in& client_addr ) noexcept;

    /** This method enables the SO_KEEPALIVE option on the specified socket.
        Returns >=0 if successful, else on error < 0 is returned
     */
    static int enableKeepAlive( int fd ) noexcept;

public:
    /** This resolved the specific network address string and port number (IPv4
        or IPv6) to a socket address info structure. The method returns true if
        successful, else false is returned.

        When successful, the method returns one or MORE address info structure
        candidates via the 'results' argument. In addition on success, the caller
        is required to freeAddresses() method to free the memory associated
        with the 'results' argument.
     */
    static bool resolveAddress( const char* remoteHostName, int portNumToConnectTo, struct addrinfo*& results ) noexcept;

    /** This method frees the memory associated with the 'results' argument
        that was populated by a previous call to the resolveAddress() method.
     */
    static void freeAddresses( struct addrinfo* results ) noexcept;

    /** Create a file descriptor for the specified socket address info.
        Returns a valid file descriptor if successful, else < 0 is returned.
     */
    static int createSocket( struct addrinfo* addr ) noexcept;
};

}  // end namespaces
}
}
}
#endif  // end header latch