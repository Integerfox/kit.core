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
#include "Kit/System/Trace.h"

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
}

bool EntryRecord::getPrevious( uint64_t       olderThanTimestamp,
                               const Marker_T beginHereMarker,
                               IPayload&      dst,
                               Marker_T&      entryMarker ) noexcept
{
}

bool EntryRecord::getByEntryIndex( size_t    entryIndex,
                                   IPayload& dst,
                                   Marker_T& entryMarker ) noexcept
{
}

size_t EntryRecord::getMaxIndex() const noexcept
{
    return m_maxEntries - 1;
}

bool EntryRecord::addEntry( const IPayload& src ) noexcept
{
}

void EntryRecord::resetHead() noexcept
{
}

bool EntryRecord::eraseAllEntries() noexcept
{
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
}

void EntryRecord::scanAllEntries()
{
}
size_t EntryRecord::incrementOffset( Size_T offsetToIncrement ) const noexcept
{
}
size_t EntryRecord::decrementOffset( Size_T offsetToDecrement ) const noexcept
{
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
        KIT_SYSTEM_TRACE_MSG( SECT_, "getByOffest(): offset=%lu, timestamp=%lu", offset, (uint32_t) m_entryTimestamp  );
        return false;
    }

    entryMarker.indexValue  = m_entryTimestamp;
    entryMarker.mediaOffset = offset;
    return true;
}


}  // end namespaces
}
}
}
//------------------------------------------------------------------------------
