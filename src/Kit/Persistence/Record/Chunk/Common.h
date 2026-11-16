#ifndef KIT_PERSISTENCE_RECORD_CHUNK_COMMON_H
#define KIT_PERSISTENCE_RECORD_CHUNK_COMMON_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Record/IPayload.h"
#include "Kit/Persistence/Record/IChunk.h"
#include "Kit/Persistence/Record/IMedia.h"
#include "Kit/Checksum/IEdc.h"
#include "Kit/System/Assert.h"

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Chunk {

/** This partial concrete class provides common implementations, utility methods,
    etc. for concrete child classes that implement the IChunk interface.  The
    class ASSUMES the following:

    - All child classes will use a checksum to detect corrupted data.

    - There is sufficiently large temporary buffer to hold a Record's data plus
      ALL of the chunk's meta data (i.e. the entire chunk is assembled in the
      buffer so it can be transferred with a single IMedia call). Note: The private_.h header
      file provides a default buffer. See private_.h for additional details
      about the default buffer.

    - Some of helper methods (e.g. pushToRecord() and pullFromRecord()) are
      virtual wrapper methods.  By having a virtual wrapper - pushing/pulling
      to/from the data record facilitates future child classes to extend existing
      chunk classes with the ability to decrypt/encrypt the record data
 */
class Common : public IChunk
{
protected:
    /// Constructor
    Common( IMedia&              media,
            Kit::Checksum::IEdc& edc,
            uint8_t*             workBuffer,
            Size_T               workBufferSize ) noexcept
        : m_media( media )
        , m_crc( edc )
        , m_workBuffer( workBuffer )
        , m_workBufferSize( workBufferSize )
    {
        KIT_SYSTEM_ASSERT( m_workBuffer != nullptr && m_workBufferSize > 0 );
    }

protected:
    /** Helper method that reads an entire chunk (header + record data + EDC)
        from 'media' at 'index' into m_workBuffer and validates its EDC.  The
        chunk layout in m_workBuffer is:

            [header: 'headerLen' bytes][record data: 'dataLen' bytes][EDC]

        The LAST field of the header MUST be the Size_T record data length.

        The 'expectedDataLen' is used to read the entire chunk with a SINGLE
        IMedia::read() call.  Additional reads are only issued when the stored
        data length is larger than 'expectedDataLen', or when the single read
        fails (e.g. the media is shorter than the expected chunk length).

        Returns true and sets 'dataLen' if the chunk is valid; else false is
        returned.  NOTE: m_workBuffer is overwritten, i.e. any previous chunk
        (or pointers into it) are invalidated.
     */
    bool readChunk( IMedia& media, Size_T index, Size_T headerLen, Size_T expectedDataLen, Size_T& dataLen ) noexcept;

    /** Helper method that computes the EDC over the first 'len' bytes of
        m_workBuffer (i.e. header + record data), appends the EDC to the
        buffer, and then writes the entire chunk to 'media' at 'index' with a
        SINGLE IMedia::write() call. When 'invalidate' is true, the EDC is
        corrupted.  The caller is required to ensure that 'len' plus the EDC
        size does not exceed m_workBufferSize.  NOTE: The EDC bytes are written
        into m_workBuffer immediately after the first 'len' bytes.

        Returns true if the write was successful, else false is returned.
     */
    bool writeChunk( IMedia& media, Size_T index, Size_T len, bool invalidate ) noexcept;

protected:
    /** Helper method. Encapsulates pushing 'sizeDataToPush' bytes of record
        data, starting at 'srcData' (typically inside m_workBuffer), to the
        record. Returns the result of IPayload::copyFrom().
     */
    virtual bool pushToRecord( IPayload& dstHandler, const uint8_t* srcData, Size_T sizeDataToPush );

    /** Helper method. Encapsulates retrieving up to 'maxDstLen' bytes of record
        data into 'dstBuffer' (typically inside m_workBuffer).  Returns the
        length of the data, or KIT_PERSISTENCE_SIZE_MAX on error.
     */
    virtual Size_T pullFromRecord( IPayload& srcHandler, uint8_t* dstBuffer, Size_T maxDstLen );

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
