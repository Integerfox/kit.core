#ifndef KIT_PERSISTENCE_RECORD_CRC_H
#define KIT_PERSISTENCE_RECORD_CRC_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/EventQueue/IQueue.h"
#include "Kit/Persistence/Record/IPayload.h"
#include "Kit/Persistence/Record/IChunk.h"
#include "Kit/Persistence/Record/IMedia.h"
#include "Kit/Checksum/IEdc.h"

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Chunk {

/** This concrete class implements the IChunk interface and provides error
    detection of corrupted persistently stored data using a CRC and or Checksum.
    The strength and bit width of the CRC is determined by the IEdc instance
    provided in the class's constructor.
 */
class Crc : public IChunk
{
public:
    /// Constructor
    Crc( IMedia&              media,
         Kit::Checksum::IEdc& edc,
         uint8_t*             workBuffer ) noexcept
        : m_media( media )
        , m_crc( edc )
        , m_workBuffer( workBuffer )
    {
    }

public:
    /// See Kit::Persistence::Record::IChunk
    void start( Kit::EventQueue::IQueue& myEventQueue ) noexcept override;

    /// See Kit::Persistence::Record::IChunk
    void stop() noexcept override;

    /// See Kit::Persistence::Record::IChunk
    bool loadData( IPayload& destination,
                   Size_T    index = 0 ) noexcept override;

    /// See Kit::Persistence::Record::IChunk
    bool updateData( IPayload& source,
                     Size_T    index      = 0,
                     bool      invalidate = false ) noexcept override;
    /// See Kit::Persistence::Record::IChunk
    Size_T getMetadataLength() const noexcept override;

protected:
    /// Persistent storage media
    IMedia& m_media;

    /// CRC handler
    Kit::Checksum::IEdc& m_crc;

    /// Buffer for storing the Record's data when transferring to/from persistent storage
    uint8_t* m_workBuffer;
};

}  // end namespaces
}
}
}
#endif  // end header latch
