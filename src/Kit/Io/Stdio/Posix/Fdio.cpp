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
#include <unistd.h>
#include <sys/ioctl.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace Stdio {
namespace Posix {

bool Fdio::write( int fd, bool& eosFlag, const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept
{
    KIT_SYSTEM_ASSERT( buffer != nullptr );

    // Trap that the stream has been CLOSED!
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
    bytesWritten = ::write( fd, buffer, maxBytes );
    eosFlag      = bytesWritten == 0 ? true : false;
    return bytesWritten > 0;
}

void Fdio::flush( int fd ) noexcept
{
    // Ignore if the stream has been CLOSED!
    if ( fd != INVALID_FD )
    {
        fsync( fd );
    }
}

bool Fdio::isOpened( int fd ) noexcept
{
    return fd != INVALID_FD;
}

void Fdio::close( int& fd ) noexcept
{
    if ( fd != INVALID_FD )
    {
        ::close( fd );
        fd = INVALID_FD;
    }
}

bool Fdio::read( int fd, bool& eosFlag, void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept
{
    KIT_SYSTEM_ASSERT( buffer != nullptr );

    // Trap that the stream has been CLOSED!
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
    bytesRead = (int)::read( fd, buffer, numBytes );
    eosFlag   = bytesRead == 0 ? true : false;
    return bytesRead > 0;
}

bool Fdio::available( int fd ) noexcept
{
    // Trap that the stream has been CLOSED!
    if ( fd == INVALID_FD )
    {
        return false;
    }

    int nbytes;
    int result = ioctl( fd, FIONREAD, &nbytes );

    // Check for errors
    if ( result < 0 )
    {
        return false;
    }

    return nbytes > 0;
}

}  // end namespace
}
}
}
//------------------------------------------------------------------------------