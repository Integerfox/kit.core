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
#include "Kit/Io/Types.h"
#include "Kit/Persistence/Types.h"

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

    bool result = fd.setAbsolutePos( offset );
    if ( result )
    {
        result = fd.write( srcData, static_cast<Io::Kit::Type::SSize_T>( srcLen ) );
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

    Kit::Io::File::Input fd( m_fname );
    if ( !fd.isOpened() )
    {
        return KIT_PERSISTENCE_SIZE_MAX;
    }

    bool result = fd.setAbsolutePos( offset );
    if ( !result )
    {
        fd.close();
        return KIT_PERSISTENCE_SIZE_MAX;
    }

    result = fd.read( dstBuffer, static_cast<Io::Kit::Type::SSize_T>( bytesToRead ) );

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