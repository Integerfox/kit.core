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


#include "Kit/Io/Stdio/Win32/Fdio.h"
#include "Kit/Io/File/System.h"
#include "Kit/Io/Types.h"
#include "Kit/System/Assert.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <direct.h>

///
namespace Kit {
///
namespace Io {
///
namespace File {
///
namespace Win32 {


//////////////////////
/** This static class provides a collection of functions for operating on Windows
    file handles.

    NOTE: All file name/paths MUST be in native format (i.e. use '\' as the
          directory separator)
 */
class Fdio : public Kit::Io::Stdio::Win32::Fdio
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
        DWORD createOpt = OPEN_EXISTING;
        DWORD accessOpt = readOnly ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE;
        if ( forceCreate )
        {
            createOpt = OPEN_ALWAYS;
        }
        if ( forceEmptyFile )
        {
            createOpt = CREATE_ALWAYS;
        }

        // Open the file
        KitIoFileHandle_T fd( CreateFile( fileEntryName,
                                          accessOpt,
                                          FILE_SHARE_READ | FILE_SHARE_WRITE,
                                          0,
                                          createOpt,
                                          FILE_ATTRIBUTE_NORMAL,
                                          0 ) );
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
        if ( fd == INVALID_HANDLE_VALUE )
        {
            return false;
        }

        DWORD len = GetFileSize( fd, 0 );

        // GetFileSize returns INVALID_FILE_SIZE on error, but this is also a valid
        // file size (4GB - 1). Must check GetLastError() to distinguish.
        if ( len == INVALID_FILE_SIZE )
        {
            if ( GetLastError() != NO_ERROR )
            {
                return false;
            }
        }

        length = static_cast<ByteCount_T>( len );
        return true;
    }

public:
    /** Returns the current file pointer offset, in bytes, from the top of the
        file.  If there is an error than false is returned.
     */
    inline static bool currentPos( KitIoFileHandle_T fd, ByteCount_T& currentPos ) noexcept
    {
        if ( fd == INVALID_HANDLE_VALUE )
        {
            return false;
        }

        DWORD pos = SetFilePointer( fd, 0, 0, FILE_CURRENT );

        // SetFilePointer returns INVALID_SET_FILE_POINTER on error, but this is also
        // a valid position. Must check GetLastError() to distinguish.
        if ( pos == INVALID_SET_FILE_POINTER )
        {
            if ( GetLastError() != NO_ERROR )
            {
                return false;
            }
        }

        currentPos = static_cast<ByteCount_T>( pos );
        return true;
    }

    /** Adjusts the current pointer offset by the specified delta (in bytes).
        Returns true if successful, else false (i.e. setting the pointer
        past/before the file boundaries).
     */
    inline static bool setRelativePos( KitIoFileHandle_T fd, ByteCount_T deltaOffset ) noexcept
    {
        if ( fd == INVALID_HANDLE_VALUE )
        {
            return false;
        }

        DWORD result = SetFilePointer( fd, deltaOffset, 0, FILE_CURRENT );

        // SetFilePointer returns INVALID_SET_FILE_POINTER on error, but this is also
        // a valid position. Must check GetLastError() to distinguish.
        if ( result == INVALID_SET_FILE_POINTER )
        {
            return GetLastError() == NO_ERROR;
        }
        return true;
    }

    /** Sets the file pointer to the absolute specified offset (in bytes).
        Returns true if successful, else false (i.e. setting the
        pointer past the end of the file).
     */
    inline static bool setAbsolutePos( KitIoFileHandle_T fd, ByteCount_T newoffset ) noexcept
    {
        if ( fd == INVALID_HANDLE_VALUE )
        {
            return false;
        }

        DWORD result = SetFilePointer( fd, newoffset, 0, FILE_BEGIN );

        // SetFilePointer returns INVALID_SET_FILE_POINTER on error, but this is also
        // a valid position. Must check GetLastError() to distinguish.
        if ( result == INVALID_SET_FILE_POINTER )
        {
            return GetLastError() == NO_ERROR;
        }
        return true;
    }

    /** Sets the file pointer to End-Of-File.  Returns true  if successful, else
        false if an error occurred.
     */
    inline static bool setToEof( KitIoFileHandle_T fd ) noexcept
    {
        if ( fd == INVALID_HANDLE_VALUE )
        {
            return false;
        }

        DWORD result = SetFilePointer( fd, 0, 0, FILE_END );
        
        // SetFilePointer returns INVALID_SET_FILE_POINTER on error, but this is also
        // a valid position. Must check GetLastError() to distinguish.
        if ( result == INVALID_SET_FILE_POINTER )
        {
            return GetLastError() == NO_ERROR;
        }
        return true;
    }

public:
    /** Returns information about the file system entry.  If there is any
        error, the function returns false; else true is returned.
     */
    inline static bool getInfo( const char* fsEntryName, struct _stat& statOut ) noexcept
    {
        KIT_SYSTEM_ASSERT( fsEntryName != nullptr );
        return _stat( fsEntryName, &statOut ) == 0;
    }

    /** Creates a new, empty file.  If the file already exists, the call
        fails.  Returns true if successful.
     */
    static bool createFile( const char* fileName ) noexcept
    {
        KIT_SYSTEM_ASSERT( fileName != nullptr );

        // Set open flags to only open+create if the file does not already exist
        DWORD createOpt = CREATE_NEW;
        DWORD accessOpt = GENERIC_READ | GENERIC_WRITE;

        // Open the file to create it
        KitIoFileHandle_T fd( CreateFile( fileName,
                                          accessOpt,
                                          FILE_SHARE_READ | FILE_SHARE_WRITE,
                                          0,
                                          createOpt,
                                          FILE_ATTRIBUTE_NORMAL,
                                          0 ) );
        bool              result = ( fd != INVALID_HANDLE_VALUE );
        Fdio::close( fd );
        return result;
    }

    /** Creates a new directory.  If the directory already exists, the call fails.
        Returns true if successful.
     */
    static bool createDirectory( const char* dirName ) noexcept
    {
        KIT_SYSTEM_ASSERT( dirName != nullptr );
        return _mkdir( dirName ) == 0;
    }

public:
    /** Renames and/or moves a file.  Returns true if successful.
        Note: On Windoze, the rename() command does NOT move directories,
              but it will move files. The rename() command will fail with 'EACCES'
              when attempting to move a directory.
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
        if ( ::remove( fsEntryName ) != 0 )  // NOTE: remove() on Windoze does NOT delete directories!
        {
            return _rmdir( fsEntryName ) == 0;
        }
        return true;
    }

public:
    /** Returns the first entry in the directory.  Each successful 'find-first'
        call must be followed (at some point) by a call to closeDirectory().

        The entry name is copied into 'dstEntryName'.  The 'maxEntryNameSize'
        argument specifies the size - which needs to include space for the null
        terminator - of the 'dstEntryName' buffer.  If there are no entries in
        the directory, then 'dstEntryName' is set to an empty string.

        Returns true if successful; else false if a file system error was
        encountered.
     */
    static bool findFirstDirEntry( KitIoFileDirectory_T& hdl,
                                   const char*           dirName,
                                   NameString&           dstEntryName ) noexcept
    {
        KIT_SYSTEM_ASSERT( dirName != nullptr );

        WIN32_FIND_DATA fdata;
        hdl = FindFirstFile( dirName, &( fdata ) );
        if ( hdl != INVALID_HANDLE_VALUE )
        {
            dstEntryName = fdata.cFileName;
            return true;
        }

        // Check if the error is 'no files found'
        if ( GetLastError() == ERROR_FILE_NOT_FOUND )
        {
            dstEntryName.clear();
            return true;
        }

        // File system error
        return false;
    }

    /** Closes a directory.  Can only be called after a successful call to
        findFirstDirEntry()
     */
    static void closeDirectory( KitIoFileDirectory_T& hdl ) noexcept
    {
        FindClose( hdl );
    }

    /** Reads 'next' entry in the directory.  The entry name is copied into
        'dstEntryName'.  If there are no more entries in the directory, then
        'dstEntryName' is set to an empty string.  The 'maxEntryNameSize'
        argument specifies the size - which needs to include space for the null
        terminator - of the 'dstEntryName' buffer.

        This method can only be called after a successful call to
        findFirstDirEntry().

        Returns true if successful; else false if a file system error was
        encountered.
     */
    static bool findNextDirEntry( KitIoFileDirectory_T& hdl, NameString& dstEntryName ) noexcept
    {
        WIN32_FIND_DATA fdata;
        if ( FindNextFile( hdl, &fdata ) )
        {
            dstEntryName = fdata.cFileName;
            return true;
        }

        // Check if the error is 'no files found'
        if ( GetLastError() == ERROR_NO_MORE_FILES )
        {
            dstEntryName.clear();
            return true;
        }

        // File system error
        return false;
    }
};

}  // end namespaces
}
}
}
#endif  // end header latch
