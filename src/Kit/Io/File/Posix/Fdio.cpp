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
#include "Kit/Io/File/System.h"
#include "Kit/System/Assert.h"
#include <cerrno>
#include <sys/types.h>
#include <fcntl.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace File {
namespace Posix {


//////////////////////
KitIoFileHandle_T Fdio::open( const char* fileEntryName, bool readOnly, bool forceCreate, bool forceEmptyFile ) noexcept
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
    return fd;
}

bool Fdio::length( KitIoFileHandle_T fd, ByteCount_T& length ) noexcept
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

bool Fdio::currentPos( KitIoFileHandle_T fd, ByteCount_T& currentPos ) noexcept
{
    if ( fd == INVALID_FD )
    {
        return false;
    }

    off_t pos  = lseek( fd, 0, SEEK_CUR );
    currentPos = (ByteCount_T)pos;
    return pos != (off_t)-1;
}

bool Fdio::setRelativePos( KitIoFileHandle_T fd, ByteCount_T deltaOffset ) noexcept
{
    if ( fd == INVALID_FD )
    {
        return false;
    }

    off_t pos = lseek( fd, (off_t)deltaOffset, SEEK_CUR );
    return pos != (off_t)-1;
}

bool Fdio::setAbsolutePos( KitIoFileHandle_T fd, ByteCount_T newoffset ) noexcept
{
    if ( fd == INVALID_FD )
    {
        return false;
    }

    off_t pos = lseek( fd, (off_t)newoffset, SEEK_SET );
    return pos != (off_t)-1;
}

bool Fdio::setToEof( KitIoFileHandle_T fd ) noexcept
{
    if ( fd == INVALID_FD )
    {
        return false;
    }

    return lseek( fd, 0, SEEK_END ) != (off_t)-1;
}

bool Fdio::getInfo( const char* fsEntryName, struct stat& statOut ) noexcept
{
    KIT_SYSTEM_ASSERT( fsEntryName != nullptr );
    return stat( fsEntryName, &statOut ) == 0;
}

bool Fdio::createFile( const char* fileName ) noexcept
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

bool Fdio::createDirectory( const char* dirName ) noexcept
{
    KIT_SYSTEM_ASSERT( dirName != nullptr );
    return mkdir( dirName, 0755 ) == 0;
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
    return ::remove( fsEntryName ) == 0;
}

bool Fdio::openDirectory( KitIoFileDirectory_T& hdl, const char* dirName ) noexcept
{
    KIT_SYSTEM_ASSERT( dirName != nullptr );
    hdl = opendir( dirName );
    return hdl != nullptr;
}

void Fdio::closeDirectory( KitIoFileDirectory_T& hdl ) noexcept
{
    closedir( hdl );
}

bool Fdio::readDirectory( KitIoFileDirectory_T& hdl, NameString& dstEntryName ) noexcept
{

    errno                   = 0;
    struct dirent* entryPtr = readdir( hdl );
    if ( entryPtr != nullptr )
    {
        dstEntryName = entryPtr->d_name;
        return true;
    }
    else
    {
        // No more entries
        if ( errno == 0 )
        {
            dstEntryName.clear();
            return true;
        }
    }

    // File system error
    return false;
}

}  // end namespace
}
}
}
//------------------------------------------------------------------------------