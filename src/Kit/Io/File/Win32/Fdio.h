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
    static KitIoFileHandle_T open( const char* fileEntryName, bool readOnly = true, bool forceCreate = false, bool forceEmptyFile = false ) noexcept;

public:
    /** Returns the length, in bytes, of the file. If there is an error than
        false is returned.  Note: the implementation of this method is brute force
        in that it sets the file position indicator to EOF and uses the EOF byte
        offset as the length. The file position indicator is restored to its
        original position before returning.
     */
    static bool length( KitIoFileHandle_T fd, ByteCount_T& length ) noexcept;

public:
    /** Returns the current file pointer offset, in bytes, from the top of the
        file.  If there is an error than false is returned.
     */
    static bool currentPos( KitIoFileHandle_T fd, ByteCount_T& currentPos ) noexcept;

    /** Adjusts the current pointer offset by the specified delta (in bytes).
        Returns true if successful, else false (i.e. setting the pointer
        past/before the file boundaries).
     */
    static bool setRelativePos( KitIoFileHandle_T fd, ByteCount_T deltaOffset ) noexcept;

    /** Sets the file pointer to the absolute specified offset (in bytes).
        Returns true if successful, else false (i.e. setting the
        pointer past the end of the file).
     */
    static bool setAbsolutePos( KitIoFileHandle_T fd, ByteCount_T newoffset ) noexcept;

    /** Sets the file pointer to End-Of-File.  Returns true  if successful, else
        false if an error occurred.
     */
    static bool setToEof( KitIoFileHandle_T fd ) noexcept;

public:
    /** Returns information about the file system entry.  If there is any
        error, the function returns false; else true is returned.
     */
    static bool getInfo( const char* fsEntryName, struct _stat& statOut ) noexcept;

    /** Creates a new, empty file.  If the file already exists, the call
        fails.  Returns true if successful.
     */
    static bool createFile( const char* fileName ) noexcept;

    /** Creates a new directory.  If the directory already exists, the call fails.
        Returns true if successful.
     */
    static bool createDirectory( const char* dirName ) noexcept;

public:
    /** Renames and/or moves a file.  Returns true if successful.
        Note: On Windoze, the rename() command does NOT move directories,
              but it will move files. The rename() command will fail with 'EACCES'
              when attempting to move a directory.
     */
    static bool move( const char* oldName, const char* newName ) noexcept;

    /** Removes a file or directory.  The directory must be empty to be removed.
        Returns true if successful.
     */
    static bool remove( const char* fsEntryName );

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
                                   NameString&           dstEntryName ) noexcept;

    /** Closes a directory.  Can only be called after a successful call to
        findFirstDirEntry()
     */
    static void closeDirectory( KitIoFileDirectory_T& hdl ) noexcept;

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
    static bool findNextDirEntry( KitIoFileDirectory_T& hdl, NameString& dstEntryName ) noexcept;
};

}  // end namespaces
}
}
}
#endif  // end header latch
