#ifndef KIT_IO_FILE_WIN32_FDIO_H_
#define KIT_IO_FILE_WIN32_FDIO_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Io/Stdio/_win32/fdio.h"
#include "Kit/System/Assert.h"
#include "Kit/System/Trace.h"

///
namespace Kit {
///
namespace Io {
///
namespace File {


//////////////////////
/** This static class provides a collection of functions for operating on Windows
    file handlers.

    NOTE: All file name/paths MUST be in native format (i.e. use '\' as the
          directory separator)
 */
class Win32FileIO : public Kit::Io::Stdio::Win32IO
{
public:
    /** Opens a file.  The default arguments are set for:
            o Read/Write access
            o Do NOT create the file if the file does not exist
            o Do NOT truncate the file contents
     */
    static KitIoFileHandle_T open( const char* fileEntryName, bool readOnly = true, bool forceCreate = false, bool forceEmptyFile = false ) noexcept
    {
        KIT_SYSTEM_ASSERT( fileEntryName != nullptr );

        // Set open flags as requested
        int flags = readOnly ? O_RDONLY : O_RDWR;
        int mode  = 0666;
        if ( forceCreate )
        {
            flags |= O_CREAT;
        }
        if ( forceEmptyFile )
        {
            flags |= O_TRUNC;
        }


        // Open the file
        errno = 0;
        KitIoFileHandle_T fd( ::open( fileEntryName, flags, mode ) );
        if ( fd == INVALID_FD )
        {
            KIT_SYSTEM_TRACE_MSG( "_0test", "open('%s') failed, errno=%d", fileEntryName, errno );
        }
        return fd;
    }

public:
    /** Returns the length, in bytes, of the file. If there is an error than
        false is returned.  Note: the implementation of this method is brute force
        in that it sets the file position indicator to EOF and uses the EOF byte
        offset as the length. The file position indicator is restored to its
        original position before returning.
     */
    static bool length( KitIoFileHandle_T fd, ByteCount_T& length ) noexcept
    {
        if ( fd == INVALID_FD )
        {
            return false;
        }

        off_t cur     = lseek( fd, 0, SEEK_CUR );
        off_t eof     = lseek( fd, 0, SEEK_END );
        off_t restore = lseek( fd, cur, SEEK_SET );
        length        = (ByteCount_T)eof;
        return ( cur != (off_t)-1 ) && ( eof != (off_t)-1 ) && ( restore != (off_t)-1 );
    }

public:
    /** Returns the current file pointer offset, in bytes, from the top of the
        file.  If there is an error than false is returned.
     */
    inline static bool currentPos( KitIoFileHandle_T fd, ByteCount_T& currentPos ) noexcept
    {
        if ( fd == INVALID_FD )
        {
            return false;
        }

        off_t pos  = lseek( fd, 0, SEEK_CUR );
        currentPos = (ByteCount_T)pos;
        return pos != (off_t)-1;
    }

    /** Adjusts the current pointer offset by the specified delta (in bytes).
        Returns true if successful, else false (i.e. setting the pointer
        past/before the file boundaries).
     */
    inline static bool setRelativePos( KitIoFileHandle_T fd, ByteCount_T deltaOffset ) noexcept
    {
        if ( fd == INVALID_FD )
        {
            return false;
        }

        off_t pos = lseek( fd, (off_t)deltaOffset, SEEK_CUR );
        return pos != (off_t)-1;
    }

    /** Sets the file pointer to the absolute specified offset (in bytes).
        Returns true if successful, else false (i.e. setting the
        pointer past the end of the file).
     */
    inline static bool setAbsolutePos( KitIoFileHandle_T fd, ByteCount_T newoffset ) noexcept
    {
        if ( fd == INVALID_FD )
        {
            return false;
        }

        off_t pos = lseek( fd, (off_t)newoffset, SEEK_SET );
        return pos != (off_t)-1;
    }

    /** Sets the file pointer to End-Of-File.  Returns true  if successful, else
        false if an error occurred.
     */
    inline static bool setToEof( KitIoFileHandle_T fd ) noexcept
    {
        if ( fd == INVALID_FD )
        {
            return false;
        }

        return lseek( fd, 0, SEEK_END ) != (off_t)-1;
    }

public:
    /** Returns information about the file system entry.  If there is any
        error, the function returns false; else true is returned.
     */
    inline static bool getInfo( const char* fsEntryName, struct stat& statOut ) noexcept
    {
        KIT_SYSTEM_ASSERT( fsEntryName != nullptr );
        return stat( fsEntryName, &statOut ) == 0;
    }

    /** Creates a new, empty file.  If the file already exists, the call
        fails.  Returns true if successful.
     */
    static bool createFile( const char* fileName ) noexcept
    {
        KIT_SYSTEM_ASSERT( fileName != nullptr );

        bool result = false;
        int  fd     = ::open( fileName, O_RDWR | O_CREAT | O_EXCL, 0666 );
        if ( fd != -1 )
        {
            result = true;
            ::close( fd );
        }

        return result;
    }

    /** Creates a new directory.  If the directory already exists, the call fails.
        Returns true if successful.
     */
    static bool createDirectory( const char* dirName ) noexcept
    {
        KIT_SYSTEM_ASSERT( dirName != nullptr );
        return mkdir( dirName, 0755 ) == 0;
    }

public:
    /** Renames and/or moves a file.  Returns true if successful.
     */
    static bool move( const char* oldName, const char* newName ) noexcept
    {
        KIT_SYSTEM_ASSERT( oldName != nullptr );
        KIT_SYSTEM_ASSERT( newName != nullptr );
        return ::rename( oldName, newName ) == 0;
    }

    /** Removes a file or directory.  The directory must be empty to be removed.
        Returns true if successful.
     */
    static bool remove( const char* fsEntryName )
    {
        KIT_SYSTEM_ASSERT( fsEntryName != nullptr );
        return ::remove( fsEntryName ) == 0;
    }
};

}  // end namespaces
}
}
#endif  // end header latch