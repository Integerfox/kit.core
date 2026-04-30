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
#include "Kit/Persistence/Types.h"
#include <string.h>

#define SECT_ "Kit::Persistence::Record::Chunk::Common"

//------------------------------------------------------------------------------
namespace Kit {
namespace Persistence {
namespace Record {
namespace Chunk {

#define META_LEN   sizeof( Size_T )
#define META_CRC   m_crc.getEdcSize()
#define TOTAL_META ( META_LEN + META_CRC )

/////////////////////
bool Common::readSizeT( Size_T& value, Size_T& offset, void* workBuffer ) noexcept
{
    uint8_t* buffer = ( workBuffer != nullptr ) ? static_cast<uint8_t*>( workBuffer ) : m_workBuffer;
    if ( m_media.read( offset, buffer, sizeof( Size_T ) ) == sizeof( Size_T ) )
    {
        // Convert the data length from the media's Endianess to the Host/MCU Endianess
        KIT_PERSISTENCE_MEDIA_CURSOR cursor( buffer, sizeof( Size_T ) );
        cursor.read( value );

        // Accumulate the data length into the CRC calculation
        m_crc.accumulate( buffer, sizeof( Size_T ) );
        offset += sizeof( Size_T );  // Advance the offset to point to the next byte
        return true;
    }
    return false;
}

bool Common::readUint64( uint64_t& value, Size_T& offset, void* workBuffer ) noexcept
{
    uint8_t* buffer = ( workBuffer != nullptr ) ? static_cast<uint8_t*>( workBuffer ) : m_workBuffer;
    if ( m_media.read( offset, buffer, sizeof( uint64_t ) ) == sizeof( uint64_t ) )
    {
        // Convert the data length from the media's Endianess to the Host/MCU Endianess
        KIT_PERSISTENCE_MEDIA_CURSOR cursor( buffer, sizeof( uint64_t ) );
        cursor.read( value );

        // Accumulate the data length into the CRC calculation
        m_crc.accumulate( buffer, sizeof( uint64_t ) );
        offset += sizeof( uint64_t );  // Advance the offset to point to the next byte
        return true;
    }
    return false;
}

bool Common::readRecordData( Size_T dataLen, Size_T& offset ) noexcept
{
    if ( m_media.read( offset, m_workBuffer, dataLen ) == dataLen )
    {
        m_crc.accumulate( m_workBuffer, dataLen );
        offset += dataLen;  // Advance the offset to point to the next byte
        return true;
    }
    return false;
}

/////////////////////
bool Common::pushToRecord( IPayload& dstHandler, Size_T sizeDataToPush )
{
    return dstHandler.copyFrom( m_workBuffer, sizeDataToPush );
}
Size_T Common::pullFromRecord( IPayload& srcHandler )
{
    return srcHandler.copyTo( m_workBuffer, m_workBufferSize );
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