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
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <direct.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace File {
namespace Win32 {


//////////////////////
KitIoFileHandle_T Fdio::open( const char* fileEntryName, bool readOnly, bool forceCreate, bool forceEmptyFile ) noexcept
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

bool Fdio::length( KitIoFileHandle_T fd, ByteCount_T& length ) noexcept
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

bool Fdio::currentPos( KitIoFileHandle_T fd, ByteCount_T& currentPos ) noexcept
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

bool Fdio::setRelativePos( KitIoFileHandle_T fd, ByteCount_T deltaOffset ) noexcept
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

bool Fdio::setAbsolutePos( KitIoFileHandle_T fd, ByteCount_T newoffset ) noexcept
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

bool Fdio::setToEof( KitIoFileHandle_T fd ) noexcept
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

bool Fdio::getInfo( const char* fsEntryName, struct _stat& statOut ) noexcept
{
    KIT_SYSTEM_ASSERT( fsEntryName != nullptr );
    return _stat( fsEntryName, &statOut ) == 0;
}

bool Fdio::createFile( const char* fileName ) noexcept
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

bool Fdio::createDirectory( const char* dirName ) noexcept
{
    KIT_SYSTEM_ASSERT( dirName != nullptr );
    return _mkdir( dirName ) == 0;
}

bool Fdio::move( const char* oldName, const char* newName ) noexcept
{
    KIT_SYSTEM_ASSERT( oldName != nullptr );
    KIT_SYSTEM_ASSERT( newName != nullptr );
    return ::rename( oldName, newName ) == 0;
}

bool Fdio::remove( const char* fsEntryName )
{
    KIT_SYSTEM_ASSERT( fsEntryName != nullptr );
    if ( ::remove( fsEntryName ) != 0 )  // NOTE: remove() on Windoze does NOT delete directories!
    {
        return _rmdir( fsEntryName ) == 0;
    }
    return true;
}

bool Fdio::findFirstDirEntry( KitIoFileDirectory_T& hdl,
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

void Fdio::closeDirectory( KitIoFileDirectory_T& hdl ) noexcept
{
    FindClose( hdl );
}

bool Fdio::findNextDirEntry( KitIoFileDirectory_T& hdl, NameString& dstEntryName ) noexcept
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

}  // end namespace
}
}
}
//------------------------------------------------------------------------------