#ifndef KIT_IO_STDIO_WIN32_FDIO_H_
#define KIT_IO_STDIO_WIN32_FDIO_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file


*/


#include <conio.h>
#include "Kit/Io/Types.h"

///
namespace Kit {
///
namespace Io {
///
namespace Stdio {

/** Enable/disables the work-around for STDIN (see comments on availableStdin()
    method).  
    NOTE: The hack is defaulted OFF.  This is because any unit tests that use
          STDIN (and there are several) uses STDIN as a piped stream not as
          a live console.
 */
#ifdef USE_KIT_O_STDIO_WIN32_STDIN_CONSOLE_HACK
#define ISAVAILBLE( fd ) return _kbhit()
#else
#define ISAVAILBLE( fd )                           \
    DWORD signaled = WaitForSingleObject( fd, 0 ); \
    return signaled == WAIT_OBJECT_0
#endif

//////////////////////
/** This static class provides a collection of functions for operating on Win32 handles.
 */
class Win32IO
{
public:
    /** Attempts to write up to 'maxBytes' from 'buffer' to the file descriptor
        'fd'. The actual number of bytes written is returned in 'bytesWritten'.

        The 'eosFlag' is set to true if End-of-Stream was encountered, else it is
        cleared (i.e. set to false)

        The method blocks until at least one byte is written to the file descriptor.

        Returns true if successful; else false on error.
     */
    static bool write( HANDLE fd, bool& eosFlag, const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept
    {
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
        bytesWritten         = (int)work;
        DWORD lastError      = GetLastError();
        eosFlag              = ( result != 0 || bytesWritten > 0 )                                                    ? false
                               : lastError == ERROR_HANDLE_EOF || lastError == ERROR_BROKEN_PIPE || bytesWritten == 0 ? true
                                                                                                                      : false;
        return result != 0 && bytesWritten > 0;
    }

    /// Flushes the file descriptor 'fd'
    static void flush( HANDLE fd ) noexcept
    {
        // Ignore if the stream has been CLOSED!
        if ( fd != INVALID_HANDLE_VALUE )
        {
            FlushFileBuffers( fd );
        }
    }

    /** Returns true if the file descriptor is in the open state
     */
    static bool isOpened( HANDLE fd ) noexcept
    {
        return fd != INVALID_HANDLE_VALUE;
    }

    /** Closes the file descriptor 'fd'. If 'fd' is already closed (i.e. INVALID_HANDLE_VALUE),
        the method is a NOP.  Upon a successful close, 'fd' is set to INVALID_HANDLE_VALUE.
     */
    static void close( HANDLE& fd ) noexcept
    {
        if ( fd != INVALID_HANDLE_VALUE )
        {
            CloseHandle( fd );
            fd = INVALID_HANDLE_VALUE;
        }
    }


public:
    /** The method attempts to read up to 'numBytes' from the file descriptor.  The
        actual number of bytes read is returned via 'bytesRead'

        The 'eosFlag' is set to true if End-of-Stream was encountered, else it is
        cleared (i.e. set to false)

        The method blocks until at least one byte is read from the file descriptor.

        Returns true if successful; else false on error.
     */
    static bool read( HANDLE fd, bool& eosFlag, void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept
    {
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

        unsigned long work;
        BOOL          result = ReadFile( fd, buffer, numBytes, &work, 0 );
        bytesRead            = (int)work;
        DWORD lastError      = GetLastError();
        eosFlag              = ( result != 0 && bytesRead > 0 )                                                    ? false
                               : lastError == ERROR_HANDLE_EOF || lastError == ERROR_BROKEN_PIPE || bytesRead == 0 ? true
                                                                                                                   : false;
        return result != 0 && bytesRead > 0;
    }


    /** Returns true if there is data available to be read from STDIN

        THE PROBLEM with STDIN on Windoze:
        Windows consider mouse events, change in focus, etc. as events that get
        routed to the StdIn Handle.  The ReadFile() method ignores these 'non-character'
        events, but WaitForSingleObject() does NOT. This means that WaitForSingleObject()
        can/will return false positive (that data is available) when in fact the
        ReadFile() call will block.

        AND to make matters worse - the console hack ONLY WORKS where is a 'live'
        terminal window, i.e. when the terminal's stdin is piped from a file the
        hack DOES NOT WORK! Not a very good solution :(, but it solved my 
        immediate needs.
     */
    static bool availableStdin( HANDLE fd ) noexcept
    {
        // Trap that the stream has been CLOSED!
        if ( fd == INVALID_HANDLE_VALUE )
        {
            return false;
        }
        ISAVAILBLE( fd );
    }

    /// Returns true if there is data available to be read from the file descriptor
    static bool available( HANDLE fd ) noexcept
    {
        // Trap that the stream has been CLOSED!
        if ( fd == INVALID_HANDLE_VALUE )
        {
            return false;
        }

        DWORD signaled = WaitForSingleObject( fd, 0 );
        return signaled == WAIT_OBJECT_0;
    }
};

}  // end namespaces
}
}
#endif  // end header latch