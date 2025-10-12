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
/** @file */


#include "Kit/Io/Types.h"

///
namespace Kit {
///
namespace Io {
///
namespace Stdio {
///
namespace Win32 {

/** This static class provides a collection of functions for operating on Win32 handles.
 */
class Fdio
{
public:
    /** Attempts to write up to 'maxBytes' from 'buffer' to the file descriptor
        'fd'. The actual number of bytes written is returned in 'bytesWritten'.

        The 'eosFlag' is set to true if End-of-Stream was encountered, else it is
        cleared (i.e. set to false)

        The method blocks until at least one byte is written to the file descriptor.

        Returns true if successful; else false on error. Note: .
     */
    static bool write( HANDLE fd, bool& eosFlag, const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept;

    /// Flushes the file descriptor 'fd'
    static void flush( HANDLE fd ) noexcept;

    /** Returns true if the file descriptor is in the open state
     */
    static bool isOpened( HANDLE fd ) noexcept;

    /** Closes the file descriptor 'fd'. If 'fd' is already closed (i.e. INVALID_HANDLE_VALUE),
        the method is a NOP.  Upon a successful close, 'fd' is set to INVALID_HANDLE_VALUE.
     */
    static void close( HANDLE& fd ) noexcept;

public:
    /** The method attempts to read up to 'numBytes' from the file descriptor.  The
        actual number of bytes read is returned via 'bytesRead'

        The 'eosFlag' is set to true if End-of-Stream was encountered, else it is
        cleared (i.e. set to false)

        The method blocks until at least one byte is read from the file descriptor.

        Returns true if successful; else false on error.
     */
    static bool read( HANDLE fd, bool& eosFlag, void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept;

    /** Returns true if there is data available to be read from STDIN

        THE PROBLEM with STDIN on Windoze:
        Windows consider mouse events, change in focus, etc. as events that get
        routed to the StdIn Handle.  The ReadFile() method ignores these 'non-character'
        events, but WaitForSingleObject() does NOT. This means that WaitForSingleObject()
        can/will return false positive (that data is available) when in fact the
        ReadFile() call will block.

        I have a partial work-around/hack for this problem. However, the hack
        ONLY WORKS where is a 'live' terminal window, i.e. when the terminal's
        stdin is piped from a file the hack DOES NOT WORK! Not a very good
        solution :(, but it solved my immediate needs.

        To enable the work-around define the following in kit_config.h: 
        USE_KIT_IO_STDIO_WIN32_STDIN_CONSOLE_HACK
     */
    static bool availableStdin( HANDLE fd ) noexcept;

    /// Returns true if there is data available to be read from the file descriptor
    static bool available( HANDLE fd ) noexcept;
};

}  // end namespaces
}
}
}
#endif  // end header latch