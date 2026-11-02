/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "FileAdapter.h"
#include "Kit/Io/File/Input.h"
#include "Kit/Io/File/Output.h"
#include "Kit/Io/File/System.h"
#include "Kit/Io/Types.h"
#include "Kit/Persistence/Types.h"
#include <string.h>

/// Value of media bytes that have never been written
#define ERASED_VALUE_ 0xFF

/// Size, in bytes, of the stack buffer used to pad gaps with erased bytes
#define PAD_BUFFER_SIZE_ 16

//------------------------------------------------------------------------------
namespace Kit {
namespace Persistence {
namespace Record {
namespace Media {


bool FileAdapter::start( Kit::EventQueue::IQueue& myEventQueue ) noexcept
{
    // Nothing needed
    return true;
}

void FileAdapter::stop() noexcept
{
    // Nothing needed
}

bool FileAdapter::write( Size_T offset, const void* srcData, Size_T srcLen ) noexcept
{
    // Validate parameters (and check for overflow)
    Size_T endOffset = offset + srcLen;
    if ( srcData == nullptr || endOffset > m_maxLen || endOffset < offset )
    {
        return false;
    }

    Kit::Io::File::Output fd( m_fname );
    if ( !fd.isOpened() )
    {
        return false;
    }

    // Fill any gap between the physical end of the file and 'offset' with erased bytes
    Kit::Type::SSize_T fileLen = 0;
    bool               result  = fd.length( fileLen ) && fd.setToEof();
    if ( result && static_cast<Size_T>( fileLen ) < offset )
    {
        uint8_t pad[PAD_BUFFER_SIZE_];
        memset( pad, ERASED_VALUE_, sizeof( pad ) );
        Size_T gapLen = offset - static_cast<Size_T>( fileLen );
        while ( result && gapLen > 0 )
        {
            Size_T numBytes  = gapLen < sizeof( pad ) ? gapLen : sizeof( pad );
            result           = fd.write( pad, static_cast<Kit::Type::SSize_T>( numBytes ) );
            gapLen          -= numBytes;
        }
    }

    if ( result )
    {
        result = fd.setAbsolutePos( static_cast<Kit::Type::SSize_T>( offset ) ) && fd.write( srcData, static_cast<Kit::Type::SSize_T>( srcLen ) );
    }

    fd.close();
    return result;
}

Size_T FileAdapter::read( Size_T offset, void* dstBuffer, Size_T bytesToRead ) noexcept
{
    Size_T endOffset = offset + bytesToRead;
    if ( dstBuffer == nullptr || endOffset > m_maxLen || endOffset < offset )
    {
        return KIT_PERSISTENCE_SIZE_MAX;
    }

    // Bytes beyond the physical end of the file (or when there is no file yet) read as erased
    memset( dstBuffer, ERASED_VALUE_, bytesToRead );
    Kit::Io::File::Input fd( m_fname );
    if ( !fd.isOpened() )
    {
        return Kit::Io::File::System::exists( m_fname ) ? KIT_PERSISTENCE_SIZE_MAX : bytesToRead;
    }

    Kit::Type::SSize_T fileLen = 0;
    bool               result  = fd.length( fileLen );
    if ( result && static_cast<Size_T>( fileLen ) > offset )
    {
        Size_T available = static_cast<Size_T>( fileLen ) - offset;
        Size_T numBytes  = bytesToRead < available ? bytesToRead : available;
        result           = fd.setAbsolutePos( static_cast<Kit::Type::SSize_T>( offset ) ) && fd.read( dstBuffer, static_cast<Kit::Type::SSize_T>( numBytes ) );
    }

    fd.close();
    return result ? bytesToRead : KIT_PERSISTENCE_SIZE_MAX;
}

Size_T FileAdapter::getMaxSize() const noexcept
{
    return m_maxLen;
}


}  // end namespace
}
}
}
//------------------------------------------------------------------------------