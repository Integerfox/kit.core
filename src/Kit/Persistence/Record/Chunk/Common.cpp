/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Common.h"
#include "Kit/Persistence/Record/IMedia.h"
#include "Kit/Persistence/Types.h"
#include <string.h>

#define SECT_ "Kit::Persistence::Record::Chunk::Common"

//------------------------------------------------------------------------------
namespace Kit {
namespace Persistence {
namespace Record {
namespace Chunk {


/////////////////////
bool Common::readChunk( IMedia& media, Size_T index, Size_T headerLen, Size_T expectedDataLen, Size_T& dataLen ) noexcept
{
    // Largest possible chunk (bounded by the work buffer and the media's remaining space)
    Size_T mediaSize = media.getMaxSize();
    Size_T available = index < mediaSize ? mediaSize - index : 0;
    Size_T limit     = available < m_workBufferSize ? available : m_workBufferSize;
    Size_T metaLen   = headerLen + m_crc.getEdcSize();
    if ( limit < metaLen )
    {
        return false;
    }

    // Read the entire expected chunk with a single read
    Size_T readLen = expectedDataLen <= limit - metaLen ? metaLen + expectedDataLen : limit;
    Size_T numRead = media.read( index, m_workBuffer, readLen );
    if ( numRead == KIT_PERSISTENCE_SIZE_MAX || numRead < headerLen )
    {
        // Fall back to reading just the header (e.g. the media is shorter than 'readLen')
        numRead = media.read( index, m_workBuffer, headerLen );
        if ( numRead != headerLen )
        {
            return false;
        }
    }

    // Convert the data length (last header field) from the media's Endianess to the Host/MCU Endianess
    KIT_PERSISTENCE_MEDIA_CURSOR cursor( m_workBuffer, headerLen, headerLen - sizeof( Size_T ) );
    cursor.read( dataLen );
    if ( dataLen > limit - metaLen )
    {
        return false;
    }

    // Only read the remainder when the stored data is larger than what has already been read
    Size_T chunkLen = metaLen + dataLen;
    if ( chunkLen > numRead )
    {
        Size_T remaining = chunkLen - numRead;
        if ( media.read( index + numRead, m_workBuffer + numRead, remaining ) != remaining )
        {
            return false;
        }
    }

    m_crc.reset();
    m_crc.accumulate( m_workBuffer, chunkLen );
    return m_crc.isOkay();
}

bool Common::writeChunk( IMedia& media, Size_T index, Size_T len, bool invalidate ) noexcept
{
    m_crc.reset();
    m_crc.accumulate( m_workBuffer, len );

    // NOTE: The endianess of the EDC is determined by the concrete IEdc
    //       implementation, i.e. no endian conversion is required.
    if ( !m_crc.finalize( m_workBuffer + len, m_workBufferSize - len ) )
    {
        return false;
    }

    // Corrupt the EDC when erasing the data
    if ( invalidate )
    {
        m_workBuffer[len] ^= 0xA5;
    }

    return media.write( index, m_workBuffer, len + m_crc.getEdcSize() );
}

/////////////////////
bool Common::pushToRecord( IPayload& dstHandler, const uint8_t* srcData, Size_T sizeDataToPush )
{
    return dstHandler.copyFrom( srcData, sizeDataToPush );
}
Size_T Common::pullFromRecord( IPayload& srcHandler, uint8_t* dstBuffer, Size_T maxDstLen )
{
    return srcHandler.copyTo( dstBuffer, maxDstLen );
}

void Common::resetChunkOnBadData()
{
    // Default does nothing
}


}  // end namespace
}
}
}
//------------------------------------------------------------------------------