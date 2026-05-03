#ifndef KIT_PERSISTENCE_RECORD_CHUNK_MIRRORED_H
#define KIT_PERSISTENCE_RECORD_CHUNK_MIRRORED_H
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
#include "Kit/Persistence/Record/IMedia.h"


///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Chunk {

/** This concrete class implements the IChunk interface by storing two copies
    of the Record's data, where each copy is CRC'd.  This ensures that if power
    fails during a write operation to persistent media - there will also be a
    good 'previous' copy of the data available.

    - The strength and bit width of the CRC is determined by the IEdc instance
      provided in the class's constructor.

    - The class requires two IMedia instances, one for each copy of the Record.
 */
class Mirrored : public Common
{
public:
    /// Constructor
    Mirrored( IMedia&              mediaA,
              IMedia&              mediaB,
              Kit::Checksum::IEdc& edc,
              uint8_t*             workBuffer     = g_workBuffer_,
              Size_T               workBufferSize = sizeof( g_workBuffer_ ) ) noexcept
        : Common( mediaA, edc, workBuffer, workBufferSize )
        , m_mediaB( mediaB )
    {
    }

public:
    /// See Kit::Persistence::Record::IChunk
    bool start( Kit::EventQueue::IQueue& myEventQueue ) noexcept override;

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
    /// Helper method.  If the region is 'corrupt' a transaction ID of zero is returned
    uint64_t virtual getTransactionId( IMedia& media, Size_T& dataLen, Size_T index=0  );

    protected:
    /// Current Transaction ID (the larger the value - the newer the data)
    uint64_t m_transId;

    /// 2nd media instance for the mirrored copy of the record
    IMedia& m_mediaB;

    /// Pointer to the current media (i.e. newest read/written media)
    IMedia* m_currentMedia;
};

}  // end namespaces
}
}
}
#endif  // end header latch
