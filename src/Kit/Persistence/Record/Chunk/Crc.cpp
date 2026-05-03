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
    m_crc.reset();
    Size_T offset = index;

    // Read the data length. This is the length of the application data WITHOUT
    // any of the chunk's meta data
    Size_T datalen;
    if ( readSizeT( datalen, offset, m_media ) )
    {
        // Make sure we have enough buffer space
        Size_T dataRemaining = datalen + META_CRC;
        if ( dataRemaining > m_workBufferSize )
        {
            resetChunkOnBadData();
            return false;
        }

        // Read the data AND CRC bytes
        if ( readRecordData( datalen + META_CRC, offset, m_media ) )
        {
            // Check the CRC
            if ( m_crc.isOkay() )
            {
                // Pass the data to the client
                return pushToRecord( dstHandler, datalen );
            }
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
    Size_T dataLen = pullFromRecord( srcHandler );
    if ( dataLen == KIT_PERSISTENCE_SIZE_MAX )
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

    // Write the data length to persistent storage. This is the length of the application data WITHOUT any of the chunk's meta data
    result &= writeSizeT( dataLen, offset, m_media );

    // Write Application's data
    result &= writeRecordData( dataLen, offset, m_media );

    // Finalize the CRC and write it to persistent storage.
    // NOTE: The endianess of the CRC in the payload is determined by the concrete
    //       IEdc implementation, i.e. no endian conversion is required by the
    //       Chunk when writing the CRC bytes.
    result &= m_crc.finalize( m_workBuffer, m_workBufferSize );
    if ( result )
    {
        if ( invalidate )
        {
            // Corrupt the CRC when erasing the data
            m_workBuffer[0] ^= 0xA5;
        }
        result &= m_media.write( offset, m_workBuffer, META_CRC );
    }

    return result;
}


}  // end namespace
}
}
}
//------------------------------------------------------------------------------