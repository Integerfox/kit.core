/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Driver/NV/File/Api.h"
#include "Kit/Io/File/Input.h"
#include "Kit/Io/File/InputOutput.h"
#include "Kit/Io/File/System.h"
#include "Kit/Type/SSize.h"
#include <limits>
#include <stdint.h>
#include <string.h>


//------------------------------------------------------------------------------
namespace Kit {
namespace Driver {
namespace NV {
namespace File {

//////////////////////////////////////////////////////////////////////////////
Api::Api( size_t      numPages,
          size_t      bytesPerPage,
          const char* filename ) noexcept
    : m_fname( filename )
    , m_numPages( numPages )
    , m_pageSize( bytesPerPage )
    , m_totalSize( numPages * bytesPerPage )
    , m_started( false )
{
}

bool Api::start( void* ) noexcept
{
    // Skip processing if already started
    if ( m_started )
    {
        return true;
    }

    // The file offsets/lengths are SSize_T -->the storage must fit
    if ( m_totalSize > static_cast<size_t>( ( std::numeric_limits<Kit::Type::SSize_T>::max )() ) )
    {
        return false;
    }

    // Check if there is an existing directory with the same file name
    if ( Kit::Io::File::System::isDirectory( m_fname ) )
    {
        return false;
    }

    // Open the file, creating it if needed, WITHOUT discarding existing content
    Kit::Io::File::InputOutput fd( m_fname, true, false );
    Kit::Type::SSize_T         fileLen = 0;
    if ( !fd.isOpened() || !fd.length( fileLen ) )
    {
        return false;
    }

    // Pad a new, short (e.g. the storage size grew), or partially initialized (e.g. an earlier start() failed
    // part way) file up to the storage size with the 'erased value'.  A file longer than the storage size (e.g. the
    // storage size shrank) is accepted as-is, i.e. only the first m_totalSize bytes are used
    if ( static_cast<size_t>( fileLen ) < m_totalSize )
    {
        if ( !fd.setToEof() )
        {
            return false;
        }

        uint8_t pageBuffer[OPTION_KIT_DRIVER_NV_FILE_BYTES_PER_PAGE];
        memset( pageBuffer, OPTION_KIT_DRIVER_NV_FILE_ERASED_VALUE, sizeof( pageBuffer ) );
        size_t bytesRemaining = m_totalSize - static_cast<size_t>( fileLen );
        while ( bytesRemaining )
        {
            size_t bytesToWrite = bytesRemaining > sizeof( pageBuffer ) ? sizeof( pageBuffer ) : bytesRemaining;
            if ( !fd.write( pageBuffer, static_cast<Kit::Type::SSize_T>( bytesToWrite ) ) )
            {
                return false;
            }
            bytesRemaining -= bytesToWrite;
        }
    }

    m_started = true;
    return true;
}

void Api::stop() noexcept
{
    m_started = false;
}

//////////////////////////////////////////////////////////////////////////////
bool Api::write( size_t dstOffset, const void* srcData, size_t numBytesToWrite ) noexcept
{
    // Fail if not started
    if ( !m_started )
    {
        return false;
    }

    // Fail immediately if out-of-range (computed to avoid size_t overflow)
    if ( dstOffset > m_totalSize || numBytesToWrite > ( m_totalSize - dstOffset ) )
    {
        return false;
    }


    // Open the file
    Kit::Io::File::InputOutput fd( m_fname, false, false );
    bool                       result = false;

    // Set the file position/offset (start() guarantees the range fits in SSize_T)
    if ( fd.setAbsolutePos( static_cast<Kit::Type::SSize_T>( dstOffset ) ) )
    {
        // Write the data
        result = fd.write( srcData, static_cast<Kit::Type::SSize_T>( numBytesToWrite ) );
    }

    return result;
}

bool Api::read( size_t srcOffset, void* dstData, size_t sizeDstData, size_t numBytesToRead ) noexcept
{
    // Fail if not started, or the destination buffer is too small
    if ( !m_started || sizeDstData < numBytesToRead )
    {
        return false;
    }

    // Fail immediately if out-of-range (computed to avoid size_t overflow)
    if ( srcOffset > m_totalSize || numBytesToRead > ( m_totalSize - srcOffset ) )
    {
        return false;
    }

    // Open the file (read-only)
    Kit::Io::File::Input fd( m_fname );
    bool                 result = false;

    // Set the file position/offset (start() guarantees the range fits in SSize_T)
    if ( fd.setAbsolutePos( static_cast<Kit::Type::SSize_T>( srcOffset ) ) )
    {
        // Read the data
        uint8_t*           dstPtr    = static_cast<uint8_t*>( dstData );
        Kit::Type::SSize_T remaining = static_cast<Kit::Type::SSize_T>( numBytesToRead );
        while ( remaining )
        {
            Kit::Type::SSize_T bytesRead;
            if ( !fd.read( dstPtr, remaining, bytesRead ) )
            {
                return false;
            }

            remaining -= bytesRead;
            dstPtr    += bytesRead;
        }

        // If I get here I have read all of the requested data
        result = true;
    }

    return result;
}


size_t Api::getNumPages() const noexcept
{
    return m_numPages;
}

size_t Api::getPageSize() const noexcept
{
    return m_pageSize;
}

size_t Api::getTotalSize() const noexcept
{
    return m_totalSize;
}

}  // end namespaces
}
}
}
//------------------------------------------------------------------------------
