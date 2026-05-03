#ifndef KIT_PERSISTENCE_RECORD_MEDIA_NULL_H
#define KIT_PERSISTENCE_RECORD_MEDIA_NULL_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Record/IMedia.h"
#include <string.h>


///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Media {

/** This concrete class provides a 'null' implementation of the IMedia
    interface. The implementation is a 'happy path' implementation that does
    nothing, but does check/trap some basic errors. This class can be used to
    'satisfy' persistent storage clients when there is no physical persistent
    storage available on the platform.
 */
class Null : public Kit::Persistence::Record::IMedia
{
public:
    /// Constructor
    Null( Size_T maxSize ) noexcept
        : m_maxSize( maxSize )
    {
    }

public:
    /// See Kit::Persistence::Record::IMedia
    bool start( Kit::EventQueue::IQueue& myEventQueue ) noexcept override { return true; }

    /// See Kit::Persistence::Record::IMedia
    void stop() noexcept override {};

public:
    /// See Kit::Persistence::Record::IMedia.
    bool write( Size_T offset, const void* srcData, Size_T srcLen ) noexcept override
    {
        if ( srcData == nullptr || ( offset + srcLen ) > m_maxSize )
        {
            return false;
        }
        return true;
    }

    /// See Kit::Persistence::Record::IMedia.
    Size_T read( Size_T offset, void* dstBuffer, Size_T bytesToRead ) noexcept override
    {
        if ( ( offset + bytesToRead ) > m_maxSize || dstBuffer == nullptr )
        {
            return KIT_PERSISTENCE_SIZE_MAX;
        }

        memset( dstBuffer, 0, bytesToRead );
        return bytesToRead;
    }

public:
    /// See Kit::Persistence::Record::IMedia
    Size_T getMaxSize() const noexcept override { return m_maxSize; }

protected:
    /// The maximum size of the media (in bytes)
    Size_T m_maxSize;
};

}  // end namespaces
}
}
}
#endif  // end header latch
