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

#define SECT_ "Kit::Persistence::Record::Journal::EntryRecord"

//------------------------------------------------------------------------------
namespace Kit {
namespace Persistence {
namespace Record {
namespace Journal {


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
    if ( m_entryPayloadHandlerPtr == nullptr ||
         dst == nullptr ||
         maxDstLen < ( m_entryPayloadHandlerPtr->getMaxPayloadSize() + getEntryMetadataSize() ) )
    {
        return KIT_PERSISTENCE_SIZE_MAX;
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
    cursor.read( m_entryTimestamp );

    // ONLY Consuming the meta-data/timestamp
    if ( m_entryPayloadHandlerPtr == nullptr )
    {
        return true;
    }

    // If there is sufficient space -->go ahead copy the entry data
    if ( srcLen >= ( m_entryPayloadHandlerPtr->getMaxPayloadSize() + getEntryMetadataSize() ) )
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
    Size_T offset            = incrementOffset( beginHereMarker.mediaOffset );
    Size_T consecutiveCorrupt = 0;

    // Loop through all possible entries
    for ( Size_T i = 0; i < m_maxEntries - 1; i++, offset = incrementOffset( offset ) )
    {
        if ( !getByOffset( offset, dst, entryMarker ) )
        {
            // Corrupt entry: abort if too many consecutive corrupt entries are encountered
            if ( ++consecutiveCorrupt > m_maxConsecutiveCorruptSkip )
            {
                return false;
            }
        }
        else
        {
            consecutiveCorrupt = 0;
            if ( m_entryTimestamp > newerThanTimestamp )
            {
                return true;
            }
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
    Size_T offset            = decrementOffset( beginHereMarker.mediaOffset );
    Size_T consecutiveCorrupt = 0;

    // Loop through all possible entries
    for ( Size_T i = 0; i < m_maxEntries - 1; i++, offset = decrementOffset( offset ) )
    {
        if ( !getByOffset( offset, dst, entryMarker ) )
        {
            // Corrupt entry: abort if too many consecutive corrupt entries are encountered
            if ( ++consecutiveCorrupt > m_maxConsecutiveCorruptSkip )
            {
                return false;
            }
        }
        else
        {
            consecutiveCorrupt = 0;
            if ( m_entryTimestamp < olderThanTimestamp )
            {
                return true;
            }
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
    size_t latestOffset = incrementOffset( m_latestOffset );
    m_entryTimestamp    = m_latestTimestamp + 1;

    // Set the application payload handler
    m_entryPayloadHandlerPtr = const_cast<IPayload*>( &src );

    // Write the entry
    if ( !writeToMedia( latestOffset ) )
    {
        return false;
    }

    // Write the index record ONLY after the entry has been successfully written to persistent storage.
    m_headRecord.setLatestOffset( latestOffset, m_entryTimestamp );
    if ( !m_headRecord.writeToMedia() )
    {
        return false;
    }
    m_latestOffset    = latestOffset;
    m_latestTimestamp = m_entryTimestamp;
    hookOnLatestTimestampUpdated( m_latestTimestamp );
    return true;
}

void EntryRecord::resetHead() noexcept
{
    m_latestOffset    = 0;
    m_latestTimestamp = 0;
    m_headRecord.setLatestOffset( m_latestOffset, m_latestTimestamp );
    hookOnLatestTimestampUpdated( m_latestTimestamp );
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
        // 3. The 'integrity' of the Ring Buffer has been compromised

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
    // Helper: read only the timestamp metadata at a given byte offset.
    // Returns 0 for any corrupt or unwritten entry, so that corrupt entries
    // sort as the smallest possible value (i.e. "older than any valid entry").
    auto readTimestampAt = [this]( Size_T byteOffset ) -> uint64_t
    {
        m_entryPayloadHandlerPtr = nullptr;
        if ( readFromMedia( byteOffset ) )
        {
            return m_entryTimestamp;
        }
        return 0;
    };

    // NOTE: By definition there are at least 2 entries, so the binary search is
    // always valid and the "wrap-around" case is naturally handled by the 
    // rotated-sorted property of the timestamps.

    // The ring buffer's timestamps form a rotated-sorted ascending sequence
    // (0 for unwritten/corrupt entries, which are the natural minimum).
    // Binary search for the MINIMUM -- its predecessor (circularly) is the MAXIMUM.
    // This reduces reads from O(N) to O(log N), which is critical for large
    // entry counts backed by slow serial media (EEPROM, SPI NOR Flash).
    //
    // Examples (4 entries):
    //   Not yet wrapped:  [0, 1, 2, 0]  -> min at idx 3, max at idx 2 (ts=2)
    //   Fully wrapped:    [4, 5, 6, 7]  -> min at idx 0, max at idx 3 (ts=7)
    //   Latest at idx 0:  [8, 5, 6, 7]  -> min at idx 1, max at idx 0 (ts=8)
    //   Factory (erased): [0, 0, 0, 0]  -> min at idx 0, max at idx 3 (ts=0 -> no valid entries)
    Size_T   lo   = 0;
    Size_T   hi   = m_maxEntries - 1;
    uint64_t tsHi = readTimestampAt( hi * m_entrySize );

    while ( lo < hi )
    {
        Size_T   mid   = lo + ( hi - lo ) / 2;
        uint64_t tsMid = readTimestampAt( mid * m_entrySize );

        if ( tsMid > tsHi )
        {
            lo = mid + 1;   // minimum is in the upper half
        }
        else
        {
            hi   = mid;     // minimum is in the lower half (inclusive)
            tsHi = tsMid;
        }
    }

    // 'lo' is the index of the minimum; the maximum is at the preceding index
    Size_T maxIndex    = ( lo + m_maxEntries - 1 ) % m_maxEntries;
    m_latestOffset    = maxIndex * m_entrySize;
    m_latestTimestamp = readTimestampAt( m_latestOffset );

    // The candidate maximum may itself be corrupt (e.g. a hardware bit-flip on a
    // previously valid entry).  Walk backward at most m_maxCorruptScan steps to
    // find the most-recent valid entry before it.  The bound keeps startup firmly at
    // O(log N + m_maxCorruptScan) — no linear-scan fallback.
    //
    // Note: the factory/all-erased case (every entry returns 0) is handled
    // naturally: the backward walk exhausts m_maxCorruptScan steps, and
    // m_latestTimestamp remains 0, which correctly signals "no valid entries".
    for ( Size_T i = 0; i < m_maxCorruptScan && m_latestTimestamp == 0 && i < m_maxEntries - 1; i++ )
    {
        maxIndex          = ( maxIndex + m_maxEntries - 1 ) % m_maxEntries;
        m_latestOffset    = maxIndex * m_entrySize;
        m_latestTimestamp = readTimestampAt( m_latestOffset );
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
