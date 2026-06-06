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

#include "kit_config.h"
#include "Kit/Persistence/Record/IPayload.h"
#include "Kit/Persistence/Record/IChunk.h"
#include "Kit/Persistence/Record/IMedia.h"
#include "Kit/Checksum/IEdc.h"

/** The supported maximum size, in bytes, of the EDC/CRC bytes in a chunk. This
    value is used to allocate a work buffer on ths STACK - so do NOT arbitrarily
    make it large.
 */
#ifndef OPTION_KIT_PERSISTENCE_RECORD_CHUNK_MAX_EDC_SIZE 
#define OPTION_KIT_PERSISTENCE_RECORD_CHUNK_MAX_EDC_SIZE 4
#endif

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
      the additional meta data persistent stored. Note: The private_.h header
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
    }

protected:
    /** Helper method to read a Size_T value from the persistence storage. It
        has the following behavior/side-effects:

        - The 'value' is accumulated into the CRC calculation (i.e. m_crc.accumulate()
          is called)

        - The 'offset' is updated to point to the next byte after the Size_T
          value that was read

        - The default behavior is to use m_workBuffer as the buffer to read the
          Size_T value into RAM.  However, a different buffer can be provided via
          the workBuffer parameter.

        Returns true if the read was successful, else false is returned.
     */
    bool readSizeT( Size_T& value, Size_T& offset, IMedia& media ) noexcept;

    /// Same as readSizeT() but reads a uint64_t value
    bool readUint64( uint64_t& value, Size_T& offset, IMedia& media ) noexcept;

    /** Helper method to read the application record data into m_workBuffer. It
        has the following behavior/side-effects:

        - The method advances the 'offset' to point to the next byte after the
          'datalen'

        - The method also accumulates the data into the CRC calculation.

        Returns true if the read was successful, else false is returned.
     */
    bool readRecordData( Size_T dataLen, Size_T& offset, IMedia& media ) noexcept;

protected:
    /** Helper method to write a Size_T value to persistent storage.  It has the
        following behavior/side-effects:

        - Converts the 'value' from the Host/MCU Endianess to the media's Endianess
          before writing to persistent storage

        - The 'value' is accumulated into the CRC calculation (i.e. m_crc.accumulate()
          is called).  The accumulation is done AFTER any endianess conversion
          is done.

        - The 'offset' is updated to point to the next byte after the Size_T
          value that was written

        Returns true if the write was successful, else false is returned.
     */
    bool writeSizeT( Size_T value, Size_T& offset, IMedia& media ) noexcept;

    /// Same as writeSizeT() but writes a uint64_t value
    bool writeUint64( uint64_t value, Size_T& offset, IMedia& media ) noexcept;

    /** Helper method to write the application record data stored in m_workBuffer.
        It has the following behavior/side-effects:

        - The method advances the 'offset' to point to the next byte after the
          'datalen'

        - The method also accumulates the data into the CRC calculation.

        Returns true if the write was successful, else false is returned.
     */
    bool writeRecordData( Size_T dataLen, Size_T& offset, IMedia& media ) noexcept;

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
