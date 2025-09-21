#ifndef KIT_IO_FILE_SYSTEM_H_
#define KIT_IO_FILE_SYSTEM_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "kit_map.h"
#include "kit_config.h"
#include "Kit/Io/Types.h"
#include "Kit/Text/FString.h"
#include "Kit/Type/TraverserStatus.h"

#include <time.h>

/** Defer the HAL type to the platform */
#define KitIoFileDirectory_T KitIoFileDirectory_T_MAP


/** Size, in bytes, of internal work buffer.  WARNING: The internal buffer is
    allocated on the STACK - so do NOT make this too large
 */
#ifndef OPTION_KIT_IO_FILE_SYSTEM_WORK_BUFFER_SIZE
#define OPTION_KIT_IO_FILE_SYSTEM_WORK_BUFFER_SIZE 256
#endif


///
namespace Kit {
///
namespace Io {
///
namespace File {

/*----------------------------------------------------------------------------*/
/** The KIT standard directory separator is "/", but allow the application to
    override/change it.
 */
#ifndef OPTION_KIT_IO_DIRECTORY_SEPARATOR
static constexpr const char dirSep = '/';
#else
static constexpr const char dirSep = OPTION_KIT_IO_DIRECTORY_SEPARATOR;
#endif

/// The application must provide/map the native directory separator
#define KIT_IO_DIRECTORY_SEPARATOR_NATIVE KIT_IO_DIRECTORY_SEPARATOR_NATIVE_MAP

/// Provide an alias for the native directory separator
static constexpr const char nativeDirSep = KIT_IO_DIRECTORY_SEPARATOR_NATIVE;


/// Defer the maximum size of path/file name to the platform
#define KIT_IO_FILE_MAX_NAME KIT_IO_FILE_MAX_NAME_MAP

/// The size, in bytes, of the maximum allowed path/file name
static constexpr const unsigned maxName = KIT_IO_FILE_MAX_NAME;

/** This typed FString is guaranteed to be large enough to hold the
    maximum allowed file system name for the native OS/Platform
 */
typedef Kit::Text::FString<maxName> NameString;


/** This concrete class defines the interface for handling and manipulating
    entries in the System's File system.

    The directory separator is ALWAYS  'dirSep'.  If necessary it will be
    internally converted to the OS/platform  specific delimiter. Directory paths
    can contain drive designators, but ONLY if the native OS supports it.
 */
class System
{
public:
    /** Returns true if the FS Entry exists physically exists in the OS's file
        system.
     */
    inline static bool exists( const char* fsEntryName ) noexcept
    {
        System::Info_T entryInfo;
        if ( System::getInfo( fsEntryName, entryInfo ) )
        {
            return entryInfo.m_isFile || entryInfo.m_isDir;
        }

        return false;
    }

    /// Returns true if the FS Entry is a file.
    inline static bool isFile( const char* fsEntryName ) noexcept
    {
        System::Info_T entryInfo;
        if ( System::getInfo( fsEntryName, entryInfo ) )
        {
            return entryInfo.m_isFile;
        }

        return false;
    }

    /// Returns true if the FS Entry is a directory
    static bool isDirectory( const char* fsEntryName ) noexcept
    {
        System::Info_T entryInfo;
        if ( System::getInfo( fsEntryName, entryInfo ) )
        {
            return entryInfo.m_isDir;
        }

        return false;
    }

    /// Return true if the user has read permission for the file system entry
    static bool isReadable( const char* fsEntryName ) noexcept
    {
        System::Info_T entryInfo;
        if ( System::getInfo( fsEntryName, entryInfo ) )
        {
            return entryInfo.m_readable;
        }

        return false;
    }

    /// Return true if the user has write permission for the file system entry
    static bool isWriteable( const char* fsEntryName ) noexcept
    {
        System::Info_T entryInfo;
        if ( System::getInfo( fsEntryName, entryInfo ) )
        {
            return entryInfo.m_writeable;
        }

        return false;
    }

    /** Returns the size, in bytes, of the file.  If the File Entry is not
        a file or an error occurs, then 0 is returned.
     */
    static ByteCount_T size( const char* fsEntryName ) noexcept
    {
        System::Info_T entryInfo;
        if ( System::getInfo( fsEntryName, entryInfo ) )
        {
            return entryInfo.m_size;
        }

        return 0;
    }

    /** Returns the time/date the file entry was last modified. If an error
        occurred, then  ((time_t)-1) is returned.
     */
    static time_t timeModified( const char* fsEntryName ) noexcept
    {
        System::Info_T entryInfo;
        if ( System::getInfo( fsEntryName, entryInfo ) )
        {
            return entryInfo.m_mtime;
        }

        return -1;
    }


public:
    /// This data structure defines status attributes for a file system entry
    struct Info_T
    {
        /// Size, in bytes, of the file entry
        ByteCount_T m_size;

        /// Time the file entry was last modified/written
        time_t m_mtime;

        /// True if the file entry is readable by the application
        bool m_readable;

        /// True if the file entry is writable by the application
        bool m_writeable;

        /// True if the file entry is a directory
        bool m_isDir;

        /// True if the file entry is a file
        bool m_isFile;
    };


    /** Returns information about the file system entry.  If there is any
        error, the function returns false; else true is returned.
     */
    static bool getInfo( const char* fsEntryName, Info_T& infoOut ) noexcept;


public:
    /** Creates the 'fileName' as an empty file.  The parent directory must
        exist. Returns true if successful.
     */
    static bool createFile( const char* fileName ) noexcept;

    /** Creates the 'dirName' as a directory.  The parent directory must
        exist. Returns true if successful.
     */
    static bool createDirectory( const char* dirName ) noexcept;


public:
    /** Renames the file system entry name in the OS's file system. This
        is NOT a general purpose 'move' that can change the path as well
        as the name.  This method only effects the entry's name ->no changes
        to its path is made.

        @returns true if successful.
        @returns false if the 'oldName' does not exist or if the 'newName'
        already exists.
     */
    static bool renameInPlace( const char* oldName, const char* newName ) noexcept;

    /** Moves and/or renames the specified file.  This method can NOT be used to
        move directories.

        @returns true if successful.
        @returns false if the 'oldFileName' does not exist or if the 'newFileName'
        already exists; OR the move operation itself fails.  When false is returned
        the state of the 'oldFileName' and 'newFileName' files is undetermined.
     */
    static bool moveFile( const char* oldFileName, const char* newFileName ) noexcept;

    /** Deletes the file system entry from the OS's file system.  If the entry
        is an directory, the directory must be empty before it is deleted.
        Returns true if successful.
     */
    static bool remove( const char* fsEntryName ) noexcept;

    /** Copies the context of the 'srcName' file to the specified 'dstName'
        file.

        @returns true if successful.
        @returns false if their is an error.  For example: 'srcName' is NOT a
        file, 'srcName' does not exist, 'dstName' already exists.
     */
    static bool copyFile( const char* srcName, const char* dstName ) noexcept;

    /** Appends the context of the 'srcName' file to the specified 'dstName'
        file.
        @returns true if successful.
        @returns false if their is an error.  For example: 'srcName' is NOT a
        file, 'destFile' does not exist, 'destFile' is a directory.
     */
    static bool appendFile( const char* srcName, const char* destFile ) noexcept;

public:
    /** Returns the file system entry name in its in native format. The scope
        of the returned value is limited, i.e. the value needs to be consumed
        BEFORE any other call to this interface and/or methods in the
        Kit::Io::File* namespaces.  Note: This method IS thread safe in that
        each thread has it own internal storage that is used for the translation.
     */
    static const char* getNative( const char* fsEntryName ) noexcept;

    /** Returns the file system entry name in the 'Standard' format. The scope
        of the returned value is limited, i.e. the value needs to be consumed
        BEFORE any other call to this interface and/or methods in the
        Kit::Io::File* namespaces.  Note: This method IS thread safe in that
        each thread has it own internal storage that is used for the translation.
     */
    static const char* getStandard( const char* fsEntryName ) noexcept;


public:
    /** Returns the first entry in the directory.  If the directory cannot be
        opened false is returned; else true is returned.

        The 'hdl' argument is populated with the platform specific directory
        handle information needed for subsequent calls to getNextEntry().

        The 'firstEntryName' argument is populated with the name of the first
        entry in the directory.  If the directory is empty then 'firstEntryName'
        is set to an empty string

        The 'maxNameLen' argument specifies the byte size - which needs to include
        space for the null terminator - of the 'firstEntryName' buffer.
     */
    static bool getFirstDirEntry( KitIoFileDirectory_T& hdl,
                                  NameString&           dirNameToList,
                                  NameString&           dstFirstEntryName ) noexcept;

    /** Returns the next entry in the directory.  If a file system error was
        encountered false is returned; else true is returned.

        The 'hdl' argument must be the instance that was populated by
        getFirstEntry() or a previous call to getNextEntry().

        The 'nextEntryName' argument is populated with the name of the next
        entry in the directory.  If there are no more entries then
        'nextEntryName' is set to an empty string

        The 'maxNameLen' argument specifies the size - which needs to include
        space for the null terminator, in bytes, of the 'nextEntryName' buffer.
     */
    static bool getNextDirEntry( KitIoFileDirectory_T& hdl, NameString& dstNextEntryName ) noexcept;

    /** Closes the directory handle. This method MUST always be called after a
        successful call to getFirstEntry().
     */
    static void closeDirectory( KitIoFileDirectory_T& hdl ) noexcept;



public:
    /** Extracts the drive letter from the complete name. The drive letter
        will NOT contain the drive separator character.
     */
    static inline void splitDrive( const char*         fsEntryName,
                                   Kit::Text::IString& drive,
                                   char                driveSeparator = ':' ) noexcept
    {
        split( fsEntryName, nullptr, &drive, nullptr, nullptr, nullptr, nullptr, dirSep, '.', driveSeparator );
    }


    /// Extracts the full path (drive+path) from the complete name.
    static inline void splitFullpath( const char*         fsEntryName,
                                      Kit::Text::IString& fullpath,
                                      char                dirSeparator       = dirSep,
                                      char                extensionSeparator = '.',
                                      char                driveSeparator     = ':' ) noexcept
    {
        split( fsEntryName, &fullpath, nullptr, nullptr, nullptr, nullptr, nullptr, dirSeparator, extensionSeparator, driveSeparator );
    }

    /// Extracts the path (no drive) from the complete name.
    static inline void splitPath( const char*         fsEntryName,
                                  Kit::Text::IString& path,
                                  char                dirSeparator       = dirSep,
                                  char                extensionSeparator = '.',
                                  char                driveSeparator     = ':' ) noexcept
    {
        split( fsEntryName, nullptr, nullptr, &path, nullptr, nullptr, nullptr, dirSeparator, extensionSeparator, driveSeparator );
    }


    /// Extracts the full file name (name+extension) from the complete name.
    static inline void splitFullname( const char*         fsEntryName,
                                      Kit::Text::IString& fullname,
                                      char                dirSeparator       = dirSep,
                                      char                extensionSeparator = '.',
                                      char                driveSeparator     = ':' ) noexcept
    {
        split( fsEntryName, nullptr, nullptr, nullptr, &fullname, nullptr, nullptr, dirSeparator, extensionSeparator, driveSeparator );
    }

    /// Extracts the file name from the complete name.
    static inline void splitName( const char*         fsEntryName,
                                  Kit::Text::IString& name,
                                  char                dirSeparator       = dirSep,
                                  char                extensionSeparator = '.',
                                  char                driveSeparator     = ':' ) noexcept
    {
        split( fsEntryName, nullptr, nullptr, nullptr, nullptr, &name, 0, dirSeparator, extensionSeparator, driveSeparator );
    }


    /// Extracts the file extension from the complete name.
    static inline void splitExtension( const char*         fsEntryName,
                                       Kit::Text::IString& extension,
                                       char                dirSeparator       = dirSep,
                                       char                extensionSeparator = '.',
                                       char                driveSeparator     = ':' ) noexcept
    {
        split( fsEntryName, nullptr, nullptr, nullptr, nullptr, nullptr, &extension, dirSeparator, extensionSeparator, driveSeparator );
    }


    /// Extracts the full path and name from the complete name.
    static inline void splitFullpathFullName( const char*         fsEntryName,
                                              Kit::Text::IString& fullpath,
                                              Kit::Text::IString& fullname,
                                              char                dirSeparator       = dirSep,
                                              char                extensionSeparator = '.',
                                              char                driveSeparator     = ':' ) noexcept
    {
        split( fsEntryName, &fullpath, nullptr, nullptr, &fullname, nullptr, nullptr, dirSeparator, extensionSeparator, driveSeparator );
    }


    /** This method breaks the specified File System Entry name into it's
        individual components.  If the specific component is not
        need, then specify nullptr as the argument and it will be skipped.
        The fullPath argument contains the drive+path. Path only contains
        the path.  The fullName argument contains the file name+extension.
        Name only contains the file name.  On return, fullPath string (if
        specified and drive exists) will contain the driver separator.  The
        drive string will NOT 'include' the drive separator. The fullName
        parameter will contain the extensionSeparator.  The name and
        extension will NOT 'include' the extensionSeparator.

        Note: 'fullPath' and 'path' WILL contain the trailing directory
              separator.
     */
    static void split( const char*         fsEntryName,
                       Kit::Text::IString* fullPath,
                       Kit::Text::IString* drive,
                       Kit::Text::IString* path,
                       Kit::Text::IString* fullName,
                       Kit::Text::IString* name,
                       Kit::Text::IString* extension,
                       char                dirSeparator       = dirSep,
                       char                extensionSeparator = '.',
                       char                driveSeparator     = ':' ) noexcept;




protected:
    /// Helper method.  Returns an internal work buffer large enough to 'max path'
    static NameString& getNameBuffer( void );
};


}  // end namespaces
}
}
#endif  // end header latch
