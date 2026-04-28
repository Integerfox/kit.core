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
#include "Kit/Persistence/Record/IPayload.h"
#include "Kit/Persistence/Record/IChunk.h"
#include "Kit/Persistence/Record/IMedia.h"
#include "Kit/Persistence/Record/Chunk/private_.h"
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

    NOTE: This class requires a work buffer to transfer data to/from the Record's
          IPayload.  That buffer MUST be able to hold the entire Record's data 
          payload plus the additional meta data persistent stored. There is a
          default buffer provided in the class's constructor.  See private_.h for
          additional details about the default buffer.
 */
class Crc : public IChunk
{
public:
    /// Constructor
    Crc( IMedia&              media,
         Kit::Checksum::IEdc& edc,
         uint8_t*             workBuffer     = g_workBuffer_,
         Size_T               workBufferSize = sizeof( g_workBuffer_ ) ) noexcept
        : m_media( media )
        , m_crc( edc )
        , m_workBuffer( workBuffer )
        , m_workBufferSize( workBufferSize )
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
    /// Helper method. Encapsulates pushing data to the record
    virtual bool pushToRecord( IPayload& dstHandler, Size_T sizeDataToPush );

    /// Helper method. Encapsulates retrieving data from the record.  Returns the length of the data
    virtual Size_T pullFromRecord( IPayload& srcHandler );

    /// Helper method. Encapsulates actions that occur when there is NO VALID data
    virtual void resetChunkOnBadData();

protected:
    /// Persistent storage media
    IMedia& m_media;

    /// CRC handler
    Kit::Checksum::IEdc& m_crc;

    /// Buffer for storing the Record's data when transferring to/from persistent storage
    uint8_t* m_workBuffer;

    /// Work buffer size (in bytes)
    Size_T m_workBufferSize;
};

}  // end namespaces
}
}
}
#endif  // end header latch
