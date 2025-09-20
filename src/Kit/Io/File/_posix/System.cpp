/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/File/System.h"
#include "fdio.h"

/// Helper method that does all of the work for populating the Info struct
static void populate_( Kit::Io::File::System::Info_T& infoOut, struct stat& filestats )
{
    infoOut.m_isDir     = ( filestats.st_mode & S_IFDIR ) == S_IFDIR;
    infoOut.m_isFile    = ( filestats.st_mode & S_IFREG ) == S_IFREG;
    infoOut.m_size      = filestats.st_size;
    infoOut.m_mtime     = filestats.st_mtime;
    infoOut.m_readable  = ( filestats.st_mode & S_IROTH ) == S_IROTH;
    infoOut.m_writeable = ( filestats.st_mode & S_IWOTH ) == S_IWOTH;

    if ( getuid() == filestats.st_uid )
    {
        infoOut.m_readable  |= ( filestats.st_mode & S_IRUSR ) == S_IRUSR;
        infoOut.m_writeable |= ( filestats.st_mode & S_IWUSR ) == S_IWUSR;
    }
    if ( getgid() == filestats.st_gid )
    {
        infoOut.m_readable  |= ( filestats.st_mode & S_IRGRP ) == S_IRGRP;
        infoOut.m_writeable |= ( filestats.st_mode & S_IWGRP ) == S_IWGRP;
    }

    // Make 'size' of directory behave per the Kit::Io::File::Api semantics
    if ( infoOut.m_isDir )
    {
        infoOut.m_size = 0;
    }
}


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace File {

bool System::getInfo( const char* fsEntryName, Info_T& infoOut ) noexcept
{
    struct stat filestats;
    if ( PosixFileIO::getInfo( getNative( fsEntryName ), filestats ) == false )
    {
        return false;
    }

    populate_( infoOut, filestats );
    return true;
}

bool System::createDirectory( const char* dirName ) noexcept
{
    return PosixFileIO::createDirectory( getNative( dirName ) );
}

bool System::createFile( const char* fileName ) noexcept
{
    return PosixFileIO::createFile( getNative( fileName ) );
}

bool System::renameInPlace( const char* oldName, const char* newName ) noexcept
{
    NameString nativeNewName = getNative( newName );
    return PosixFileIO::move( getNative( oldName ), nativeNewName );
}

bool System::moveFile( const char* oldFileName, const char* newFileName ) noexcept
{
    // Note: Under posix, the implementation of renameInPlace will/can move files
    return renameInPlace( oldFileName, newFileName );
}

bool System::remove( const char* fsEntryName ) noexcept
{
    return PosixFileIO::remove( getNative( fsEntryName ) );
}
/////////////////////////////
bool System::getFirstDirEntry( KitIoFileDirectory_T& hdl,
                               NameString&           dirNameToList,
                               NameString&           dstFirstEntryName ) noexcept
{
    if ( PosixFileIO::openDirectory( hdl, dirNameToList ) )
    {
        if ( PosixFileIO::readDirectory( hdl, dstFirstEntryName ) )
        {
            return true;
        }

        // Error reading the first entry - close the directory (per semantics of the getFirstDirEntry() method)
        PosixFileIO::closeDirectory( hdl );
    }
    return false;
}

bool System::getNextDirEntry( KitIoFileDirectory_T& hdl, NameString& dstNextEntryName ) noexcept
{
    return PosixFileIO::readDirectory( hdl, dstNextEntryName );
}

void System::closeDirectory( KitIoFileDirectory_T& hdl ) noexcept
{
    PosixFileIO::closeDirectory( hdl );
}

// end namespace
}
}
}
//------------------------------------------------------------------------------