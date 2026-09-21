#ifndef KIT_PERSISTENCE_RECORD_MEDIA_NVADAPTER_H
#define KIT_PERSISTENCE_RECORD_MEDIA_NVADAPTER_H
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
#include "Kit/Driver/NV/IApi.h"
#include "Kit/System/Assert.h"

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Media {

/** This concrete class implements the IMedia interface using the NV driver
    API (aka EEPROM media)
 */
class NVAdapter : public Kit::Persistence::Record::IMedia
{

public:
    /// Constructor.
    NVAdapter( Kit::Driver::NV::IApi& lowLevelDriver,
               Size_T                 startingOffset,
               Size_T                 allocatedLen ) noexcept
        : m_driver( lowLevelDriver )
        , m_startingOffset( startingOffset )
        , m_allocatedLen( allocatedLen )
    {
        KIT_SYSTEM_ASSERT( allocatedLen <= KIT_PERSISTENCE_SIZE_MAX - startingOffset );
    }

public:
    /// See Kit::Persistence::Record::IMedia
    bool start( Kit::EventQueue::IQueue& myEventQueue ) noexcept override
    {
        return m_driver.start();
    }

    /// See Kit::Persistence::Record::IMedia
    void stop() noexcept override
    {
        m_driver.stop();
    }

public:
    /// See Kit::Persistence::Record::IMedia
    bool write( Size_T offset, const void* srcData, Size_T srcLen ) noexcept override
    {
        KIT_SYSTEM_ASSERT( srcData != nullptr );

        // Avoid Size_T overflow/wraparound in the bounds check
        if ( srcLen > m_allocatedLen || offset > m_allocatedLen - srcLen )
        {
            return false;
        }
        return m_driver.write( m_startingOffset + offset, srcData, srcLen );
    }

    /// See Kit::Persistence::Record::IMedia
    Size_T read( Size_T offset, void* dstBuffer, Size_T bytesToRead ) noexcept override
    {
        KIT_SYSTEM_ASSERT( dstBuffer != nullptr );

        // Avoid Size_T overflow/wraparound in the bounds check
        if ( bytesToRead > m_allocatedLen || offset > m_allocatedLen - bytesToRead )
        {
            return KIT_PERSISTENCE_SIZE_MAX;
        }

        // Delegate the read to the low level driver
        if ( !m_driver.read( m_startingOffset + offset, dstBuffer, bytesToRead, bytesToRead ) )
        {
            return KIT_PERSISTENCE_SIZE_MAX;
        }
        return bytesToRead;
    }


public:
    /// See Kit::Persistence::Record::IMedia
    Size_T getMaxSize() const noexcept override
    {
        return m_allocatedLen;
    }

protected:
    /// Low level NV driver
    Kit::Driver::NV::IApi& m_driver;

    /// Starting offset for 'allocated' space within the NV media
    Size_T m_startingOffset;

    /// Length of the 'allocated' space within the NV media
    Size_T m_allocatedLen;
};

}  // end namespaces
}
}
}
#endif  // end header latch
