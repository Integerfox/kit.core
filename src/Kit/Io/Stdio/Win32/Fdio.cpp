/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "Fdio.h"
#include "Kit/System/Assert.h"
#include <conio.h>

/** Enable/disables the work-around for STDIN (see comments on availableStdin()
    method).
    NOTE: The hack is defaulted OFF.  This is because any unit tests that use
          STDIN (and there are several) uses STDIN as a piped stream not as
          a live console.
 */
#ifdef USE_KIT_IO_STDIO_WIN32_STDIN_CONSOLE_HACK
#define ISAVAILBLE( fd ) return _kbhit()
#else
#define ISAVAILBLE( fd )                           \
    DWORD signaled = WaitForSingleObject( fd, 0 ); \
    return signaled == WAIT_OBJECT_0
#endif

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace Stdio {
namespace Win32 {


bool Fdio::write( HANDLE fd, bool& eosFlag, const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept
{
    KIT_SYSTEM_ASSERT( buffer != nullptr );

    // Trap that the stream has been CLOSED!
    if ( fd == INVALID_HANDLE_VALUE )
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
    unsigned long work;
    BOOL          result = WriteFile( fd, buffer, maxBytes, &work, 0 );
    bytesWritten         = static_cast<ByteCount_T>( work );
    DWORD lastError      = GetLastError();
    eosFlag              = ( result != 0 || bytesWritten > 0 )                                                    ? false
                           : lastError == ERROR_HANDLE_EOF || lastError == ERROR_BROKEN_PIPE || bytesWritten == 0 ? true
                                                                                                                  : false;
    return result != 0 && bytesWritten > 0;
}

void Fdio::flush( HANDLE fd ) noexcept
{
    // Ignore if the stream has been CLOSED!
    if ( fd != INVALID_HANDLE_VALUE )
    {
        FlushFileBuffers( fd );
    }
}

bool Fdio::isOpened( HANDLE fd ) noexcept
{
    return fd != INVALID_HANDLE_VALUE;
}

void Fdio::close( HANDLE& fd ) noexcept
{
    if ( fd != INVALID_HANDLE_VALUE )
    {
        CloseHandle( fd );
        fd = INVALID_HANDLE_VALUE;
    }
}


bool Fdio::read( HANDLE fd, bool& eosFlag, void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept
{
    KIT_SYSTEM_ASSERT( buffer != nullptr );

    // Trap that the stream has been CLOSED!
    if ( fd == INVALID_HANDLE_VALUE )
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
    unsigned long work;
    BOOL          result = ReadFile( fd, buffer, numBytes, &work, 0 );
    bytesRead            = static_cast<ByteCount_T>( work );
    DWORD lastError      = GetLastError();
    eosFlag              = ( result != 0 && bytesRead > 0 )                                                    ? false
                           : lastError == ERROR_HANDLE_EOF || lastError == ERROR_BROKEN_PIPE || bytesRead == 0 ? true
                                                                                                               : false;
    return result != 0 && bytesRead > 0;
}

bool Fdio::availableStdin( HANDLE fd ) noexcept
{
    // Trap that the stream has been CLOSED!
    if ( fd == INVALID_HANDLE_VALUE )
    {
        return false;
    }
    ISAVAILBLE( fd );
}

bool Fdio::available( HANDLE fd ) noexcept
{
    // Trap that the stream has been CLOSED!
    if ( fd == INVALID_HANDLE_VALUE )
    {
        return false;
    }

    DWORD signaled = WaitForSingleObject( fd, 0 );
    return signaled == WAIT_OBJECT_0;
}


}  // end namespaces
}
}
}