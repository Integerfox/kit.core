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
    if ( srcData == nullptr || ( offset + srcLen ) > m_maxLen )
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
        result = fd.write( srcData, static_cast<Io::ByteCount_T>( srcLen ) );
    }

    fd.close();
    return result;
}

Size_T FileAdapter::read( Size_T offset, void* dstBuffer, Size_T bytesToRead ) noexcept
{
    if ( dstBuffer == nullptr || ( offset + bytesToRead ) > m_maxLen )
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

    result = fd.read( dstBuffer, static_cast<Io::ByteCount_T>( bytesToRead ) );

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