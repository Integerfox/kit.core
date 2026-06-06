/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Mirrored.h"
#include "Kit/Persistence/Types.h"
#include <cstdint>
#include <string.h>

#define SECT_ "Kit::Persistence::Record::Chunk::Mirrored"

//------------------------------------------------------------------------------
namespace Kit {
namespace Persistence {
namespace Record {
namespace Chunk {

#define META_TRANSACTION_ID sizeof( uint64_t )
#define META_LEN            sizeof( Size_T )
#define META_CRC            m_crc.getEdcSize()
#define TOTAL_META          ( META_TRANSACTION_ID + META_LEN + META_CRC )

/////////////////////

bool Mirrored::start( Kit::EventQueue::IQueue& myEventQueue ) noexcept
{
    bool result     = true;
    result         &= m_media.start( myEventQueue );
    result         &= m_mediaB.start( myEventQueue );
    m_transId       = 0;
    m_currentMedia  = nullptr;
    return result;
}

void Mirrored::stop() noexcept
{
    m_media.stop();
    m_mediaB.stop();
}

Size_T Mirrored::getMetadataLength() const noexcept
{
    return TOTAL_META;
}

/////////////////////
bool Mirrored::loadData( IPayload& dstHandler, Size_T index ) noexcept
{
    // Determine which Media (if any is the latest copy)
    Size_T   dataLenA;
    Size_T   dataLenB;
    uint64_t transA = getTransactionId( m_media, dataLenA, index );  // NOTE: This also loads the payload into 'm_workBuffer'
    uint64_t transB = getTransactionId( m_mediaB, dataLenB, index );
    bool     result = false;

    // No valid data!
    if ( transA == 0 && transB == 0 )
    {
        m_currentMedia = &m_media;
        m_transId      = 0;
    }

    // Media A is newest/valid
    else if ( transA > transB )
    {
        m_currentMedia = &m_media;
        m_transId      = transA;
        result         = pushToRecord( dstHandler, dataLenA );
    }

    // Media B is newest/valid
    else
    {
        m_currentMedia = &m_mediaB;
        m_transId      = transB;
        result         = pushToRecord( dstHandler, dataLenB );
    }

    // Reset the chunk if/when an error occurred
    if ( !result )
    {
        resetChunkOnBadData();
    }
    return result;
}

bool Mirrored::updateData( IPayload& srcHandler, Size_T index, bool invalidate ) noexcept
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

    // Write the data twice
    for ( unsigned i = 0; i < 2; i++ )
    {
        // Select which media to write to first (always write to the 'other/oldest' media first)
        m_currentMedia = m_currentMedia == &m_media ? &m_mediaB : &m_media;

        // Housekeeping
        Size_T offset = index;
        bool   result = true;
        m_transId++;
        m_crc.reset();

        // Write the Transaction ID.
        result &= writeUint64( m_transId, offset, *m_currentMedia );

        // Write the data length to persistent storage. This is the length of the application data WITHOUT any of the chunk's meta data
        result &= writeSizeT( len, offset, *m_currentMedia );

        // Write Application's data
        result &= writeRecordData( len, offset, *m_currentMedia );

        // Finalize the CRC and write it to persistent storage.
        // NOTE: The endianess of the CRC in the payload is determined by the concrete
        //       IEdc implementation, i.e. no endian conversion is required by the
        //       Chunk when writing the CRC bytes.
        uint8_t crcBuffer[OPTION_KIT_PERSISTENCE_RECORD_CHUNK_MAX_EDC_SIZE];
        result &= m_crc.finalize( crcBuffer, sizeof( crcBuffer ) );
        if ( result )
        {
            if ( invalidate )
            {
                // Corrupt the CRC when erasing the data
                crcBuffer[0] ^= 0xA5;
            }
            result &= m_currentMedia->write( offset, crcBuffer, META_CRC );
        }

        if ( !result )
        {
            return false;
        }
    }

    // If I get here - both writes were successful
    return true;
}

uint64_t Mirrored::getTransactionId( IMedia& media, Size_T& dataLen, Size_T index )
{
    m_crc.reset();
    uint64_t transId = 0;
    Size_T   offset  = index;

    // Read the Transaction ID
    if ( readUint64( transId, offset, media ) )
    {
        // Read the data length
        if ( readSizeT( dataLen, offset, media ) )
        {
            // Make sure we have enough buffer space (account of possible integer overflow of 'datalen + META_CRC')
            Size_T dataRemaining = dataLen + META_CRC;
            if ( dataRemaining > m_workBufferSize || dataRemaining < dataLen )
            {
                return 0;  // return 'error'
            }

            // Read the data AND CRC bytes
            if ( readRecordData( dataLen + META_CRC, offset, media ) )
            {
                // Check the CRC
                if ( m_crc.isOkay() )
                {
                    // Valid record -->return its Transaction ID
                    return transId;
                }
            }
        }
    }

    // If I get here, the record is invalid (return 'error')
    return 0;
}

}  // end namespace
}
}
}
//------------------------------------------------------------------------------