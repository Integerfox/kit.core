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

void Crc::start( Kit::EventQueue::IQueue& myEventQueue ) noexcept
{
    m_media.start( myEventQueue );
}

void Crc::stop() noexcept
{
    m_media.stop();
}

bool Crc::pushToRecord( IPayload& dstHandler, Size_T sizeDataToPush )
{
    return dstHandler.copyFrom( m_workBuffer, sizeDataToPush );
}
Size_T Crc::pullFromRecord( IPayload& srcHandler )
{
    return srcHandler.copyFrom( m_workBuffer, m_workBufferSize );
}

void Crc::resetChunkOnBadData()
{
    // Nothing required at this time (i.e. is a hook for child classes)
}

Size_T Crc::getMetadataLength() const noexcept
{
    return TOTAL_META;
}

/////////////////////
bool Crc::loadData( IPayload& dstHandler, Size_T index ) noexcept
{
    m_crc.reset();
    Size_T offset = index;

    // Read the data length. This is the length of the application data WITHOUT
    // any of the chunk's meta data
    if ( m_media.read( offset, m_workBuffer, sizeof( Size_T ) ) == sizeof( Size_T ) )
    {
        // Convert the data length from the media's Endianess to the Host/MCU Endianess
        KIT_PERSISTENCE_MEDIA_CURSOR cursor( m_workBuffer, sizeof( Size_T ) );
        Size_T                       datalen;
        cursor.read( datalen );

        // Make sure we have enough buffer space
        Size_T dataRemaining = datalen + META_CRC;
        if ( dataRemaining > m_workBufferSize )
        {
            resetChunkOnBadData();
            return false;
        }

        // CRC includes the record's data length field
        m_crc.accumulate( &datalen, sizeof( datalen ) );
        offset += sizeof( datalen );

        // Read the data AND CRC bytes
        // NOTE: The endianess of the CRC in the payload is determined by the concrete IEdc
        //       implementation, i.e. no 'cursor' is required to when reading the CRC bytes.
        uint8_t* dstPtr = m_workBuffer;
        while ( dataRemaining )
        {
            size_t bytesRead = m_media.read( offset, dstPtr, dataRemaining );
            if ( bytesRead == 0 )
            {
                break;
            }
            m_crc.accumulate( m_workBuffer, bytesRead );
            offset        += bytesRead;
            dataRemaining -= bytesRead;
        }

        // Check the CRC
        if ( m_crc.isOkay() )
        {
            // Pass the data to the client
            return pushToRecord( dstHandler, datalen );
        }
    }

    // If I get here that was an error OR a BAD CRC
    resetChunkOnBadData();
    return false;
}

bool Crc::updateData( IPayload& srcHandler, Size_T index, bool invalidate ) noexcept
{
    // Get the IPayload data
    memset( m_workBuffer, 0, m_workBufferSize );  // zero out all of the data - to ensure known values for the 'extra-space' (if there is any)
    Size_T len = pullFromRecord( srcHandler );
    if ( len == KIT_PERSISTENCE_SIZE_MAX )
    {
        return false;
    }

    // Zero the data when erasing the record
    if ( invalidate )
    {
        memset( m_workBuffer, 0, m_workBufferSize );
    }

    // Housekeeping
    Size_T offset = index;
    bool   result = true;
    m_crc.reset();

    // Convert the data length from the Host/MCU Endianess to the media's Endianess
    Size_T                       bufferRecordLen;
    KIT_PERSISTENCE_MEDIA_CURSOR cursor( &bufferRecordLen, sizeof( bufferRecordLen ) );
    cursor.write( len );

    // Write the data length to persistent storage. This is the length of the application data WITHOUT any of the chunk's meta data
    result &= m_media.write( offset, &bufferRecordLen, sizeof( bufferRecordLen ) );
    m_crc.accumulate( &bufferRecordLen, sizeof( bufferRecordLen ) );
    offset += sizeof( bufferRecordLen );

    // Write Application's data
    result &= m_media.write( offset, m_workBuffer, len );
    m_crc.accumulate( m_workBuffer, len );
    offset += len;

    // Finalize the CRC and write it to persistent storage.
    // Note: The endianess of the CRC in the payload is determined by the concrete
    //       IEdc implementation, i.e. no 'cursor' is required to when writing the
    //       CRC bytes.
    result &= m_crc.finalize( m_workBuffer, m_workBufferSize );
    if ( result )
    {
        if ( invalidate )
        {
            // Corrupt the CRC when erasing the data
            m_workBuffer[0] ^= 0xA5;
        }
        result &= m_media.write( offset, m_workBuffer, META_CRC);
    }

    return result;
}


}  // end namespace
}
}
}
//------------------------------------------------------------------------------