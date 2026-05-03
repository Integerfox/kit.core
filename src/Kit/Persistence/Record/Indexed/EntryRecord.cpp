/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "EntryRecord.h"
#include "Kit/Persistence/Types.h"
#include "Kit/System/Trace.h"
#include <inttypes.h>

#define SECT_ "Kit::Persistence::Record::Indexed::EntryRecord"

//------------------------------------------------------------------------------
namespace Kit {
namespace Persistence {
namespace Record {
namespace Indexed {


bool EntryRecord::start( Kit::EventQueue::IQueue& myEventQueue ) noexcept
{
    if ( !m_started )
    {
        m_started = m_chunk.start( myEventQueue );
        if ( m_started )
        {
            m_started &= m_headRecord.start( myEventQueue );
            if ( m_started )
            {
                // Housekeeping
                m_entryTimestamp  = 0;
                m_latestOffset    = 0;
                m_latestTimestamp = 0;

                // Load the head record
                m_headRecord.readFromMedia();  // NOTE: If the Head data is corrupt, then the Head record will be 'reset' and the head pointer is set to zero
                verifyIndex();
                hookOnLatestTimestampUpdated( m_latestTimestamp );
            }
        }
    }
    return m_started;
}

void EntryRecord::stop() noexcept
{
    if ( m_started )
    {
        m_headRecord.stop();
        m_chunk.stop();
        m_started = false;
    }
}

Size_T EntryRecord::getSize() const noexcept
{
    return m_maxEntries * m_entrySize;
}

////////////////////////////////////////////////////////////////////////////
Size_T EntryRecord::copyTo( void* dst, Size_T maxDstLen ) noexcept
{
    // Error cases
    if ( maxDstLen < m_entrySize || m_entryPayloadHandlerPtr == 0 || dst == nullptr )
    {
        return 0;
    }

    // Plant the timestamp
    KIT_PERSISTENCE_MEDIA_CURSOR cursor( dst, maxDstLen );
    cursor.write( m_entryTimestamp );

    // Copy the entry data
    auto*  dstPtr = static_cast<uint8_t*>( dst ) + getEntryMetadataSize();
    Size_T result = m_entryPayloadHandlerPtr->copyTo( dstPtr, maxDstLen - getEntryMetadataSize() );
    return result == KIT_PERSISTENCE_SIZE_MAX ? KIT_PERSISTENCE_SIZE_MAX : result + getEntryMetadataSize();
}

bool EntryRecord::copyFrom( const void* src, Size_T srcLen ) noexcept
{
    // Error cases
    if ( src == nullptr || srcLen < getEntryMetadataSize() )
    {
        return false;
    }

    // Extract the timestamp
    KIT_PERSISTENCE_MEDIA_CURSOR cursor( const_cast<void*>( src ), srcLen );
    cursor.read( m_latestTimestamp );

    // ONLY Consuming the meta-data/timestamp
    if ( m_entryPayloadHandlerPtr == 0 )
    {
        return true;
    }

    // If there is sufficient space -->go ahead copy the entry data
    if ( srcLen >= m_entrySize )
    {
        auto* srcPtr = static_cast<const uint8_t*>( src ) + getEntryMetadataSize();
        return m_entryPayloadHandlerPtr->copyFrom( srcPtr, srcLen - getEntryMetadataSize() );
    }
    return false;
}

Size_T EntryRecord::getMaxPayloadSize() const noexcept
{
    return m_entrySize;
}

////////////////////////////////////////////////////////////////////////////
bool EntryRecord::getLatest( IPayload& dst, Marker_T& entryMarker ) noexcept
{
    // Read the latest record
    return getByOffset( m_latestOffset, dst, entryMarker );
}

bool EntryRecord::getNext( uint64_t       newerThanTimestamp,
                           const Marker_T beginHereMarker,
                           IPayload&      dst,
                           Marker_T&      entryMarker ) noexcept
{
    // Set the starting offset to on where to begin the search
    Size_T offset = incrementOffset( beginHereMarker.mediaOffset );

    // Loop through all possible entries
    for ( Size_T i = 0; i < m_maxEntries - 1; i++, offset = incrementOffset( offset ) )
    {
        if ( getByOffset( offset, dst, entryMarker ) && m_entryTimestamp > newerThanTimestamp )
        {
            return true;
        }
    }

    // If I get here, no next entry was found
    return false;
}

bool EntryRecord::getPrevious( uint64_t       olderThanTimestamp,
                               const Marker_T beginHereMarker,
                               IPayload&      dst,
                               Marker_T&      entryMarker ) noexcept
{
    // Set the starting offset to on where to begin the search
    Size_T offset = decrementOffset( beginHereMarker.mediaOffset );

    // Loop through all possible entries
    for ( Size_T i = 0; i < m_maxEntries - 1; i++, offset = decrementOffset( offset ) )
    {
        if ( getByOffset( offset, dst, entryMarker ) && m_entryTimestamp < olderThanTimestamp )
        {
            return true;
        }
    }

    // If I get here, no previous entry was found
    return false;
}

bool EntryRecord::getByEntryIndex( Size_T    entryIndex,
                                   IPayload& dst,
                                   Marker_T& entryMarker ) noexcept
{
    // Fail request with a bad buffer index
    if ( entryIndex > getMaxIndex() )
    {
        return false;
    }

    // The 'first' record actually starts at offset_zero + entrySize
    // This means that the index-zero maps to offset: entrySize,  and last index maps to offset: 0
    // So we need to convert the logical index to physical offsets
    Size_T offset = ( entryIndex + 1 ) * m_entrySize;
    if ( offset > m_maxOffset )
    {
        offset = 0;
    }
    return getByOffset( offset, dst, entryMarker );
}

Size_T EntryRecord::getMaxIndex() const noexcept
{
    return m_maxEntries - 1;
}

bool EntryRecord::addEntry( const IPayload& src ) noexcept
{
    // Increment my head pointer/index
    Size_T latestOffset = incrementOffset( m_latestOffset );
    m_entryTimestamp    = m_latestTimestamp + 1;

    // Write the index record
    m_headRecord.setLatestOffset( latestOffset, m_entryTimestamp );
    if ( !m_headRecord.writeToMedia() )
    {
        return false;
    }
    m_latestOffset    = latestOffset;
    m_latestTimestamp = m_entryTimestamp;
    hookOnLatestTimestampUpdated( m_latestTimestamp );

    // Set the application payload handler
    m_entryPayloadHandlerPtr = const_cast<IPayload*>( &src );

    // Write the entry
    return writeToMedia( m_latestOffset );
}

void EntryRecord::resetHead() noexcept
{
}

bool EntryRecord::eraseAllEntries() noexcept
{
    // 'clear' the entire ENTRY Region - 128 bytes at a time
    static const uint8_t buffer[128] = {
        0,
    };
    bool   result       = true;
    Size_T remainingLen = m_entryMedia.getMaxSize();
    Size_T offset       = 0;
    while ( remainingLen && result )
    {
        Size_T len    = remainingLen < sizeof( buffer ) ? remainingLen : sizeof( buffer );
        result        = m_entryMedia.write( offset, buffer, len );
        offset       += len;
        remainingLen -= len;
    }

    // Reset my head pointer
    m_latestOffset    = 0;
    m_latestTimestamp = 0;

    // If there was an error -->try our best to recover the actual head pointer
    if ( !result )
    {
        scanAllEntries();  // Note: This will also update m_latestOffset and m_latestTimestamp to the 'correct' values
    }

    // Update the index record in persistent storage
    m_headRecord.setLatestOffset( m_latestOffset, m_latestTimestamp );
    m_headRecord.writeToMedia();
    hookOnLatestTimestampUpdated( m_latestTimestamp );
    return result;
}

////////////////////////////////////////////////////////////////////////////
bool EntryRecord::processNoValidData() noexcept
{
    // There is no "default" value/action when an entry is corrupt
    // TODO: Is there were we skip over N corrupt entries?
    return false;
}

bool EntryRecord::writeToMedia( Size_T index ) noexcept
{
    return m_chunk.updateData( *this, index );
}

bool EntryRecord::readFromMedia( Size_T index ) noexcept
{
    if ( !m_chunk.loadData( *this, index ) )
    {
        return processNoValidData();
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////
void EntryRecord::verifyIndex() noexcept
{
    // Only interested in the timestamp/meta-data
    m_entryPayloadHandlerPtr = 0;

    // Check if the 'latest' record is valid
    m_headRecord.getLatestOffset( m_latestOffset, m_latestTimestamp );  // Note: The head pointer is guaranteed to be valid (i.e. either valid, or have been reset to zero if/when bad data)
    if ( !readFromMedia( m_latestOffset ) || m_entryTimestamp != m_latestTimestamp )
    {
        // The Latest entry record is corrupt OR there is mismatch in timestamps!
        // Possible causes:
        // 1. Virgin flash
        // 2. Corruption of the entry
        // 3. A new index was written, but the associated entry was not written due to a power fail
        // 4. The 'integrity' of the Ring Buffer has been compromised

        // To Recover: Scan the entire region to derive the latest timestamp from what
        //             is persistently stored and then update index record in persistent media
        scanAllEntries();

        // Update the index record in persistent storage
        m_headRecord.setLatestOffset( m_latestOffset, m_latestTimestamp );
        m_headRecord.writeToMedia();
    }
}

void EntryRecord::scanAllEntries()
{
    m_latestOffset    = 0;
    m_latestTimestamp = 0;

    for ( Size_T offset = 0; offset <= m_maxOffset; offset += m_entrySize )
    {
        if ( readFromMedia( offset ) )
        {
            if ( m_entryTimestamp > m_latestTimestamp || m_latestTimestamp == 0 )
            {
                m_latestTimestamp = m_entryTimestamp;
                m_latestOffset    = offset;
            }
        }
    }
}

Size_T EntryRecord::incrementOffset( Size_T offsetToIncrement ) const noexcept
{
    offsetToIncrement += m_entrySize;
    if ( offsetToIncrement > m_maxOffset )
    {
        return 0;
    }
    return offsetToIncrement;
}

Size_T EntryRecord::decrementOffset( Size_T offsetToDecrement ) const noexcept
{
    offsetToDecrement -= m_entrySize;
    if ( offsetToDecrement > m_maxOffset )
    {
        return m_maxOffset;
    }
    return offsetToDecrement;
}

bool EntryRecord::getByOffset( Size_T            offset,
                               IPayload&         dst,
                               IEntry::Marker_T& entryMarker ) noexcept
{
    // Read the entry
    m_entryPayloadHandlerPtr = &dst;
    if ( !readFromMedia( offset ) )
    {
        // No valid entry
        KIT_SYSTEM_TRACE_MSG( SECT_,
                              "getByOffest(): offset=%" PRIu32 ", timestamp=%" PRIu64,
                              static_cast<uint32_t>( offset ),
                              m_entryTimestamp );
        return false;
    }

    entryMarker.timestamp   = m_entryTimestamp;
    entryMarker.mediaOffset = offset;
    return true;
}


}  // end namespaces
}
}
}
//------------------------------------------------------------------------------
