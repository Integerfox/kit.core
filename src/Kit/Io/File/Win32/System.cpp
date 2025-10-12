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
#include "Fdio.h"

/// Helper method that does all of the work for populating the Info struct
static void populate_( Kit::Io::File::System::Info_T& infoOut, struct _stat& filestats )
{
    infoOut.m_isDir     = ( filestats.st_mode & _S_IFDIR ) == _S_IFDIR;
    infoOut.m_isFile    = ( filestats.st_mode & _S_IFREG ) == _S_IFREG;
    infoOut.m_size      = filestats.st_size;
    infoOut.m_mtime     = filestats.st_mtime;
    infoOut.m_readable  = ( filestats.st_mode & _S_IREAD ) == _S_IREAD;
    infoOut.m_writeable = ( filestats.st_mode & _S_IWRITE ) == _S_IWRITE;
}


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace File {

bool System::getInfo( const char* fsEntryName, Info_T& infoOut ) noexcept
{
    struct _stat filestats;
    if ( Win32::Fdio::getInfo( getNative( fsEntryName ), filestats ) == false )
    {
        return false;
    }

    populate_( infoOut, filestats );
    return true;
}

bool System::createDirectory( const char* dirName ) noexcept
{
    return Win32::Fdio::createDirectory( getNative( dirName ) );
}

bool System::createFile( const char* fileName ) noexcept
{
    return Win32::Fdio::createFile( getNative( fileName ) );
}

bool System::renameInPlace( const char* oldName, const char* newName ) noexcept
{
    NameString nativeNewName = getNative( newName );
    return Win32::Fdio::move( getNative( oldName ), nativeNewName );
}

bool System::moveFile( const char* oldFileName, const char* newFileName ) noexcept
{
    // Note: On Windoze, the underlying ::rename() function will NOT move directories
    return renameInPlace( oldFileName, newFileName );
}

bool System::remove( const char* fsEntryName ) noexcept
{
    return Win32::Fdio::remove( getNative( fsEntryName ) );
}

/////////////////////////////
bool System::getFirstDirEntry( KitIoFileDirectory_T& hdl,
                               NameString&           dirNameToList,
                               NameString&           firstEntryName ) noexcept
{
    // Append the wildcard to the directory name
    dirNameToList += '*';
    bool result = Win32::Fdio::findFirstDirEntry( hdl, getNative( dirNameToList() ), firstEntryName );
    dirNameToList.trimRight( 1 );
    return result;
}

bool System::getNextDirEntry( KitIoFileDirectory_T& hdl, NameString& nextEntryName ) noexcept
{
    return Win32::Fdio::findNextDirEntry( hdl, nextEntryName );
}

void System::closeDirectory( KitIoFileDirectory_T& hdl ) noexcept
{
    Win32::Fdio::closeDirectory( hdl );
}

// end namespace
}
}
}
//------------------------------------------------------------------------------