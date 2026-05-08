/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "HeadRecord.h"
#include "Kit/Persistence/Types.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Persistence {
namespace Record {
namespace Journal {


bool HeadRecord::start( Kit::EventQueue::IQueue& myEventQueue ) noexcept
{
    if ( !m_started )
    {
        m_started     = m_chunk.start( myEventQueue );
        m_validLatest = false;
    }

    return m_started;
}

void HeadRecord::stop() noexcept
{
    if ( m_started )
    {
        m_chunk.stop();
        m_started = false;
    }
}

Size_T HeadRecord::getSize() const noexcept
{
    return getMaxPayloadSize() + m_chunk.getMetadataLength();
}

/////////////////////////////////////////////////////////////////////////////
Size_T HeadRecord::copyTo( void* dst, Size_T maxDstLen ) noexcept
{
    // Check for error cases
    if ( dst == nullptr || maxDstLen < getMaxPayloadSize() || !m_validLatest )
    {
        return KIT_PERSISTENCE_SIZE_MAX;
    }

    KIT_PERSISTENCE_MEDIA_CURSOR cursor( dst, maxDstLen );
    cursor.write( m_latestTimestamp );
    cursor.write( m_latestOffset );
    return getMaxPayloadSize();
}

bool HeadRecord::copyFrom( const void* src, Size_T srcLen ) noexcept
{
    // Check for error cases
    if ( src == nullptr || srcLen < getMaxPayloadSize() )
    {
        return false;
    }
    KIT_PERSISTENCE_MEDIA_CURSOR cursor( const_cast<void*>( src ), srcLen );
    cursor.read( m_latestTimestamp );
    cursor.read( m_latestOffset );
    m_validLatest = true;
    return true;
}

Size_T HeadRecord::getMaxPayloadSize() const noexcept
{
    return sizeof( m_latestTimestamp ) + sizeof( m_latestOffset );
}

/////////////////////////////////////////////////////////////////////////////
bool HeadRecord::writeToMedia( Size_T index ) noexcept
{
    return m_chunk.updateData( *this, index );
}

bool HeadRecord::readFromMedia( Size_T index ) noexcept
{
    if ( !m_chunk.loadData( *this, index ) )
    {
        return processNoValidData();
    }
    return true;
}

bool HeadRecord::processNoValidData() noexcept
{
    m_latestOffset    = 0;
    m_latestTimestamp = 0;
    m_validLatest     = true;
    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool HeadRecord::getLatestOffset( Size_T& offset, uint64_t& indexValue ) const noexcept
{
    if ( !m_validLatest )
    {
        return false;
    }

    offset     = m_latestOffset;
    indexValue = m_latestTimestamp;
    return true;
}

void HeadRecord::setLatestOffset( Size_T offset, uint64_t indexValue ) noexcept
{
    m_validLatest     = true;
    m_latestOffset    = offset;
    m_latestTimestamp = indexValue;
}

}  // end namespace
}
}
}
//------------------------------------------------------------------------------