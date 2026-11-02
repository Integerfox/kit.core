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
#include "Kit/System/Assert.h"
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
#define HEADER_LEN          ( META_TRANSACTION_ID + META_LEN )
#define TOTAL_META          ( HEADER_LEN + META_CRC )

// Per update: media A gets the larger transaction ID, media B the smaller
#define TRANS_ID_INCREMENT_A 2
#define TRANS_ID_INCREMENT_B 1

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
    // Determine which Media (if any is the latest copy). NOTE: Each read loads
    // the payload into 'm_workBuffer'.  Media B is read FIRST because media A
    // holds the newest copy in steady state, i.e. A's payload is left in the buffer.
    Size_T   expectedLen = dstHandler.getMaxPayloadSize();
    Size_T   dataLenA;
    Size_T   dataLenB;
    uint64_t transB = getTransactionId( m_mediaB, index, expectedLen, dataLenB );
    uint64_t transA = getTransactionId( m_media, index, expectedLen, dataLenA );
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
        result         = pushToRecord( dstHandler, m_workBuffer + HEADER_LEN, dataLenA );
    }

    // Media B is newest/valid (re-read B since reading A overwrote 'm_workBuffer')
    else
    {
        m_currentMedia = &m_mediaB;
        m_transId      = transB;
        if ( getTransactionId( m_mediaB, index, expectedLen, dataLenB ) == transB )
        {
            result = pushToRecord( dstHandler, m_workBuffer + HEADER_LEN, dataLenB );
        }
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
    // Enforce the IChunk precondition that loadData() is called first
    KIT_SYSTEM_ASSERT( m_currentMedia != nullptr );

    if ( m_workBufferSize < TOTAL_META )
    {
        return false;
    }

    // Get the IPayload data (placed after the chunk's header)
    memset( m_workBuffer, 0, m_workBufferSize );  // zero out all of the data - to ensure known values for the 'extra-space' (if there is any)
    Size_T len = pullFromRecord( srcHandler, m_workBuffer + HEADER_LEN, m_workBufferSize - TOTAL_META );
    if ( len == KIT_PERSISTENCE_SIZE_MAX )
    {
        return false;
    }

    // Zero the data when erasing the record
    if ( invalidate )
    {
        memset( m_workBuffer, 0, m_workBufferSize );
    }

    // Write the data twice, always writing to the 'other/oldest' media first.
    // Media A always gets the larger transaction ID (both copies hold the same
    // data) so that loadData() finds the newest copy with its LAST read.
    bool     currentIsA   = m_currentMedia == &m_media;
    IMedia*  writeOrder[] = { currentIsA ? &m_mediaB : &m_media, currentIsA ? &m_media : &m_mediaB };
    uint64_t baseId       = m_transId;
    for ( IMedia* media : writeOrder )
    {
        uint64_t transId = baseId + ( media == &m_media ? TRANS_ID_INCREMENT_A : TRANS_ID_INCREMENT_B );

        // Prepend the header: Transaction ID + data length (length of the application data WITHOUT any of the chunk's meta data)
        KIT_PERSISTENCE_MEDIA_CURSOR cursor( m_workBuffer, HEADER_LEN );
        cursor.write( transId );
        cursor.write( len );

        // Append the CRC and write the entire chunk with a single media write
        if ( !writeChunk( *media, index, HEADER_LEN + len, invalidate ) )
        {
            return false;
        }

        // Track the newest successfully written copy
        if ( transId > m_transId )
        {
            m_transId      = transId;
            m_currentMedia = media;
        }
    }

    // If I get here - both writes were successful
    return true;
}

uint64_t Mirrored::getTransactionId( IMedia& media, Size_T index, Size_T expectedDataLen, Size_T& dataLen )
{
    // Chunk layout: [Transaction ID][data length][application data][CRC]
    uint64_t transId = 0;
    if ( readChunk( media, index, HEADER_LEN, expectedDataLen, dataLen ) )
    {
        KIT_PERSISTENCE_MEDIA_CURSOR cursor( m_workBuffer, META_TRANSACTION_ID );
        cursor.read( transId );
    }

    // NOTE: A transaction ID of zero indicates an invalid record
    return transId;
}

}  // end namespace
}
}
}
//------------------------------------------------------------------------------