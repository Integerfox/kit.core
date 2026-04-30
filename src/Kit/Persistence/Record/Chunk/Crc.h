#ifndef KIT_PERSISTENCE_RECORD_CHUNK_CRC_H
#define KIT_PERSISTENCE_RECORD_CHUNK_CRC_H
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
#include "Kit/Persistence/Record/Chunk/private_.h"
#include "Kit/Persistence/Record/Chunk/Common.h"


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
class Crc : public Common
{
public:
    /// Constructor
    Crc( IMedia&              media,
         Kit::Checksum::IEdc& edc,
         uint8_t*             workBuffer     = g_workBuffer_,
         Size_T               workBufferSize = sizeof( g_workBuffer_ ) ) noexcept
        : Common( media, edc, workBuffer, workBufferSize )
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

};

}  // end namespaces
}
}
}
#endif  // end header latch
