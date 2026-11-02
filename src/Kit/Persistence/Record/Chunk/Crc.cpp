/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Crc.h"
#include "Kit/Persistence/Types.h"
#include <string.h>

#define SECT_ "Kit::Persistence::Record::Chunk::Crc"

//------------------------------------------------------------------------------
namespace Kit {
namespace Persistence {
namespace Record {
namespace Chunk {

#define META_LEN   sizeof( Size_T )
#define META_CRC   m_crc.getEdcSize()
#define TOTAL_META ( META_LEN + META_CRC )

/////////////////////

bool Crc::start( Kit::EventQueue::IQueue& myEventQueue ) noexcept
{
    return m_media.start( myEventQueue );
}

void Crc::stop() noexcept
{
    m_media.stop();
}

Size_T Crc::getMetadataLength() const noexcept
{
    return TOTAL_META;
}

/////////////////////
bool Crc::loadData( IPayload& dstHandler, Size_T index ) noexcept
{
    // Chunk layout: [data length][application data][CRC]
    Size_T dataLen;
    if ( readChunk( m_media, index, META_LEN, dstHandler.getMaxPayloadSize(), dataLen ) )
    {
        // Pass the data to the client
        return pushToRecord( dstHandler, m_workBuffer + META_LEN, dataLen );
    }

    // If I get here that was an error OR a BAD CRC
    resetChunkOnBadData();
    return false;
}

bool Crc::updateData( IPayload& srcHandler, Size_T index, bool invalidate ) noexcept
{
    if ( m_workBufferSize < TOTAL_META )
    {
        return false;
    }

    // Get the IPayload data (placed after the data length field)
    memset( m_workBuffer, 0, m_workBufferSize );  // zero out all of the data - to ensure known values for the 'extra-space' (if there is any)
    Size_T dataLen = pullFromRecord( srcHandler, m_workBuffer + META_LEN, m_workBufferSize - TOTAL_META );
    if ( dataLen == KIT_PERSISTENCE_SIZE_MAX )
    {
        return false;
    }

    // Zero the data when erasing the record
    if ( invalidate )
    {
        memset( m_workBuffer, 0, m_workBufferSize );
    }

    // Prepend the data length. This is the length of the application data WITHOUT any of the chunk's meta data
    KIT_PERSISTENCE_MEDIA_CURSOR cursor( m_workBuffer, META_LEN );
    cursor.write( dataLen );

    // Append the CRC and write the entire chunk with a single media write
    return writeChunk( m_media, index, META_LEN + dataLen, invalidate );
}


}  // end namespace
}
}
}
//------------------------------------------------------------------------------