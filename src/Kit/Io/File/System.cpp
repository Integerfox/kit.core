/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "System.h"
#include "Kit/Io/File/Input.h"
#include "Kit/Io/File/Output.h"
#include "Kit/System/PrivateStartup.h"
#include "Kit/System/Tls.h"
#include "Kit/System/Assert.h"
#include <string.h>
#include <new>


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace File {

/////////////////////////////////////////////////////////////////////////////
static Kit::System::Tls* workNameTlsPtr_;


// Anonymous namespace
namespace {

class RegisterInitHandler : public Kit::System::IStartupHook
{
public:
    ///
    RegisterInitHandler()
        : IStartupHook( MIDDLE_WARE ) {}


protected:
    ///
    void notify( InitLevel init_level ) noexcept override
    {
        // Set up TLS storage for working buffer for File/Path names
        if ( workNameTlsPtr_ == nullptr )
        {
            workNameTlsPtr_ = new ( std::nothrow ) Kit::System::Tls();
            KIT_SYSTEM_ASSERT( workNameTlsPtr_ != nullptr );
        }
    }
};
}  // end anonymous namespace

///
static RegisterInitHandler autoRegister_systemInit_hook;

/////////////////////////////////////////////////////////////////////////////
static bool copyHelper_( IInput& src, IOutput& dst )
{
    int  bytesRead = 0;
    char buf[OPTION_KIT_IO_FILE_SYSTEM_WORK_BUFFER_SIZE];
    while ( src.isEof() == false )
    {
        if ( !src.read( buf, sizeof( buf ), bytesRead ) )
        {
            // was it a read error OR eof?
            return src.isEof() ? true : false;
        }

        if ( !dst.write( buf, bytesRead ) && !dst.isEof() )
        {
            return false;  // write error
        }
    }

    return true;
}


/////////////////////////////////////////////////////////////////////////////
bool System::exists( const char* fsEntryName ) noexcept
{
    System::Info_T entryInfo;
    if ( System::getInfo( fsEntryName, entryInfo ) )
    {
        return entryInfo.m_isFile || entryInfo.m_isDir;
    }

    return false;
}

 bool System::isFile( const char* fsEntryName ) noexcept
{
    System::Info_T entryInfo;
    if ( System::getInfo( fsEntryName, entryInfo ) )
    {
        return entryInfo.m_isFile;
    }

    return false;
}

bool System::isDirectory( const char* fsEntryName ) noexcept
{
    System::Info_T entryInfo;
    if ( System::getInfo( fsEntryName, entryInfo ) )
    {
        return entryInfo.m_isDir;
    }

    return false;
}

bool System::isReadable( const char* fsEntryName ) noexcept
{
    System::Info_T entryInfo;
    if ( System::getInfo( fsEntryName, entryInfo ) )
    {
        return entryInfo.m_readable;
    }

    return false;
}

bool System::isWriteable( const char* fsEntryName ) noexcept
{
    System::Info_T entryInfo;
    if ( System::getInfo( fsEntryName, entryInfo ) )
    {
        return entryInfo.m_writeable;
    }

    return false;
}

ByteCount_T System::size( const char* fsEntryName ) noexcept
{
    System::Info_T entryInfo;
    if ( System::getInfo( fsEntryName, entryInfo ) )
    {
        return entryInfo.m_size;
    }

    return 0;
}

time_t System::timeModified( const char* fsEntryName ) noexcept
{
    System::Info_T entryInfo;
    if ( System::getInfo( fsEntryName, entryInfo ) )
    {
        return entryInfo.m_mtime;
    }

    return -1;
}

/////////////////////////////////////////////////////////////////////////////
const char* System::getNative( const char* fsEntryName ) noexcept
{
    // Convert any/all directory separators -->Use Brute force to ensure I ALWAYS ended up with ALL native separators (i.e. handle mixed-separator cases)
    Kit::Text::IString& workName = getNameBuffer();
    workName                     = fsEntryName;
    workName.replace( '/', nativeDirSep );
    workName.replace( '\\', nativeDirSep );
    return workName;
}

const char* System::getStandard( const char* fsEntryName ) noexcept
{
    // Convert any/all directory separators -->Use Brute force to ensure I ALWAYS ended up with ALL Standard separators (i.e. handle mixed-separator cases)
    Kit::Text::IString& workName = getNameBuffer();
    workName                     = fsEntryName;
    workName.replace( '/', dirSep );
    workName.replace( '\\', dirSep );
    return workName;
}


/////////////////////////////////////////////////////////////////////////////
void System::split( const char*         fileEntryName,
                    Kit::Text::IString* fullPath,
                    Kit::Text::IString* drive,
                    Kit::Text::IString* path,
                    Kit::Text::IString* fullName,
                    Kit::Text::IString* name,
                    Kit::Text::IString* extension,
                    char                dirSeparator,
                    char                extensionSeparator,
                    char                driveSeparator ) noexcept
{
    // Initialize all parameters to 'NULL'
    if ( drive )
    {
        drive->clear();
    }
    if ( fullPath )
    {
        fullPath->clear();
    }
    if ( path )
    {
        path->clear();
    }
    if ( fullName )
    {
        fullName->clear();
    }
    if ( name )
    {
        name->clear();
    }
    if ( extension )
    {
        extension->clear();
    }

    // Split off the Drive letter
    const char* entry = fileEntryName;
    const char* ptr;
    if ( ( ptr = strchr( entry, driveSeparator ) ) != 0 )
    {
        int len = ptr - entry + 1;
        if ( drive )
        {
            drive->copyIn( entry, len - 1 );
        }
        if ( fullPath )
        {
            fullPath->copyIn( entry, len );
        }
        entry += len;
    }

    // Split off the path
    if ( ( ptr = strrchr( entry, dirSeparator ) ) != 0 )
    {
        int len = ptr - entry + 1;
        if ( fullPath )
        {
            fullPath->appendTo( entry, len );
        }
        if ( path )
        {
            path->copyIn( entry, len );
        }
        entry += len;
    }

    // Get full name
    if ( fullName )
    {
        *fullName = entry;
    }

    // Split off name+extension
    if ( ( ptr = strrchr( entry, extensionSeparator ) ) != 0 )
    {
        int len = ptr - entry;
        if ( name )
        {
            name->copyIn( entry, len );
        }
        if ( extension )
        {
            *extension = entry + len + 1;
        }
    }

    // Just name ->no extension
    else
    {
        if ( name )
        {
            *name = entry;
        }
    }
}


/////////////////////////////////////////////////////////////////////////////
bool System::copyFile( const char* srcName, const char* dstName ) noexcept
{
    if ( !isFile( srcName ) || isDirectory( dstName ) )
    {
        return false;
    }

    Input  src( srcName );
    Output dst( dstName, true, true );
    return copyHelper_( src, dst );
}


bool System::appendFile( const char* srcName, const char* dstName ) noexcept
{
    if ( !isFile( srcName ) || isDirectory( dstName ) )
    {
        return false;
    }

    Input  src( srcName );
    Output dst( dstName, true );
    dst.setToEof();
    return copyHelper_( src, dst );
}


/////////////////////////////////////////////////////////////////////////////
NameString& System::getNameBuffer( void )
{
    // Get my thread local storage for the working buffer
    auto* workNamePtr = (NameString*)workNameTlsPtr_->get();
    if ( workNamePtr == 0 )
    {
        workNamePtr = new ( std::nothrow ) NameString;
        KIT_SYSTEM_ASSERT( workNamePtr != nullptr );
        workNameTlsPtr_->set( workNamePtr );
    }

    return *workNamePtr;
}

}  // end namespace
}
}
//------------------------------------------------------------------------------