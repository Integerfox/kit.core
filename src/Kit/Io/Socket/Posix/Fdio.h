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
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

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
    static void close( int& fd ) noexcept
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
    static void flush( int fd ) noexcept
    {
        // Do not know how to implement using only Posix  (jtt 2-14-2015)
    }

    /** Returns true if the file descriptor is in the open state
     */
    static bool isOpened( int fd ) noexcept
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
    static bool available( int fd ) noexcept
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
};

}  // end namespaces
}
}
}
#endif  // end header latch