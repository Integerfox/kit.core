/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Record.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Trace.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/FatalError.h"
#include "Kit/Itc/SyncReturnHandler.h"


#define SECT_ "Kit::Dm::Persistence"
//
#define DM_RECORD_METADATA_LEN 2

///
using namespace Kit::Dm::Persistence;

//////////////////////////////////////////////////////
bool Record::start( Kit::EventQueue::IQueue& myEventQueue ) noexcept
{
    if ( !m_started )
    {
        // Validate we have valid data
        for ( unsigned i = 0; i < m_numItems; i++ )
        {
            if ( m_items[i].mpPtr == nullptr )
            {
                Kit::System::FatalError::logf( Kit::System::Shutdown::eDATA_MODEL, "Kit::Dm::Persistence::Record::m_items[%u].mpPtr is null", i );
                return false;  // Not really needed -->but helps with off-target unit tests
            }
        }

        // Start the chunk handler
        if ( !m_chunkHandler.start( myEventQueue ) )
        {
            return false;
        }

        // Housekeeping
        m_started         = true;
        m_myEventQueuePtr = &myEventQueue;
        setTimingSource( myEventQueue );

        // Load the record's data from persistent storage
        bool subscribeForChanges = true;
        if ( !m_chunkHandler.loadData( *this ) )
        {
            // No valid data -->reset to my defaults
            KIT_SYSTEM_TRACE_MSG( SECT_, "Initial loadData() failed. mp[0]=%s", m_items[0].mpPtr->getName() );
            if ( resetData() )
            {
                updateNVRAM();
            }

            // The Record has requested that NO UPDATES to persistence storage occur -->so we prevent the subscriptions
            else
            {
                subscribeForChanges = false;
            }
        }

        // Record was successfully loaded -->allow for optional child class processing
        else
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "Initial loadData() succeeded! mp[0]=%s", m_items[0].mpPtr->getName() );
            hookProcessPostRecordLoaded();
        }

        // Subscribe for change notification
        if ( subscribeForChanges )
        {
            for ( unsigned i = 0; i < m_numItems; i++ )
            {
                // Only allocate a subscriber when requested
                if ( m_items[i].observerPtr != NO_SUBSCRIBER )
                {
                    // Subscribe with the current sequence number so there will be NO IMMEDIATE call back
                    m_items[i].observerPtr->setEventQueue( *m_myEventQueuePtr );
                    m_items[i].observerPtr->setCallback<Record, &Record::dataChanged>( this );
                    m_items[i].mpPtr->genericAttach( *( m_items[i].observerPtr ), m_items[i].mpPtr->getSequenceNumber() );
                }
            }
        }
    }

    return true;
}

void Record::stop() noexcept
{
    if ( m_started )
    {
        m_started = false;
        Timer::stop();

        // Cancel subscriptions
        for ( unsigned i = 0; i < m_numItems; i++ )
        {
            // Skip if there is no Subscriber instance
            if ( m_items[i].observerPtr != NO_SUBSCRIBER )
            {
                m_items[i].mpPtr->genericDetach( *( m_items[i].observerPtr ) );
            }
        }

        // Stop the Chunk Handler
        m_chunkHandler.stop();
    }
}

Kit::Persistence::Size_T Record::getSize() const noexcept
{
    return m_chunkHandler.getMetadataLength() + getMaxPayloadSize();
}

//////////////////////////////////////////////////////
Kit::Persistence::Size_T Record::getMaxPayloadSize() const noexcept
{
    Kit::Persistence::Size_T byteCount = DM_RECORD_METADATA_LEN;
    for ( unsigned i = 0; i < m_numItems; i++ )
    {
        byteCount += m_items[i].mpPtr->getExternalSize();
    }
    return byteCount;
}

Kit::Persistence::Size_T Record::copyTo( void* dst, Kit::Persistence::Size_T maxDstLen ) noexcept
{
    KIT_SYSTEM_ASSERT( maxDstLen > DM_RECORD_METADATA_LEN );

    // Write Schema identifiers
    uint8_t* buffer  = (uint8_t*)dst;
    *buffer++        = m_major;
    *buffer++        = m_minor;
    maxDstLen       -= DM_RECORD_METADATA_LEN;

    // Export the Model Point data
    Kit::Persistence::Size_T bytesWritten = DM_RECORD_METADATA_LEN;
    for ( unsigned i = 0; i < m_numItems; i++ )
    {
        // Check to see if we would overflow the buffer
        Kit::Persistence::Size_T mpDataSize = m_items[i].mpPtr->getExternalSize();
        if ( mpDataSize > maxDstLen )
        {
            return Kit::Persistence::KIT_PERSISTENCE_SIZE_MAX;
        }

        // Export the data (and check for errors)
        if ( m_items[i].mpPtr->exportData( buffer, maxDstLen ) == 0 )
        {
            return Kit::Persistence::KIT_PERSISTENCE_SIZE_MAX;
        }

        // Housekeeping
        maxDstLen    -= mpDataSize;
        bytesWritten += mpDataSize;
        buffer       += mpDataSize;
    }

    return bytesWritten;
}

bool Record::copyFrom( const void* src, Kit::Persistence::Size_T srcLen ) noexcept
{
    KIT_SYSTEM_ASSERT( srcLen > DM_RECORD_METADATA_LEN );

    // Get schema identifiers
    const uint8_t* buffer  = (const uint8_t*)src;
    uint8_t        major   = *buffer++;
    uint8_t        minor   = *buffer++;
    srcLen                -= DM_RECORD_METADATA_LEN;

    // Enforce schema rules
    if ( major != m_major || minor != m_minor )
    {
        return schemaChange( major, minor, buffer, srcLen );
    }

    // Import the Model Point data
    for ( unsigned i = 0; i < m_numItems; i++ )
    {
        // Check to see if there is enough data in the buffer
        size_t mpDataSize = m_items[i].mpPtr->getExternalSize();
        if ( mpDataSize > srcLen )
        {
            return false;
        }

        // Import the data (and check for errors)
        if ( m_items[i].mpPtr->importData( buffer, srcLen ) == 0 )
        {
            return false;
        }

        // Housekeeping
        srcLen -= mpDataSize;
        buffer += mpDataSize;
    }

    return true;
}


void Record::updateNVRAM() noexcept
{
    KIT_SYSTEM_TRACE_MSG( SECT_, "updateNVRAM()" );

    // NOTE: If the write to the storage media failed -->the RAM contents are
    // still valid so no immediate issue.  However, on the next power cycle the
    // record will be defaulted since the CRC is going to be bad
    m_chunkHandler.updateData( *this );
}

/////////////////////////////////////
void Record::dataChanged( Kit::Dm::IModelPoint& point, Kit::Dm::IObserver& observer ) noexcept
{
    KIT_SYSTEM_TRACE_MSG( SECT_, "Record Changed: mp=%s, timeMarker=%u", point.getName(), m_timerMarker );

    // Sync the observer with the 'actual' MP sequence number at the time of the callback
    uint16_t seqNum = point.getSequenceNumber();
    point.genericAttach( observer, seqNum );

    // No delay -->update NVRAM immediately
    if ( m_delayMs == 0 )
    {
        updateNVRAM();
    }

    // Defer/Delay the update to NVRAM
    else
    {
        // If the timer is not running -->then this is the 'first' change notification
        uint32_t now = Kit::System::ElapsedTime::milliseconds();
        if ( !Timer::isRunning() )
        {
            m_timerMarker = now;
        }

        // Update NVRAM if the maximum delay time has expired
        if ( Kit::System::ElapsedTime::expiredMilliseconds( m_timerMarker, m_maxDelayMs, now ) )
        {
            Timer::stop();
            updateNVRAM();
        }

        // Start my software timer to delay the update to NVRAM
        else
        {
            Timer::start( m_delayMs );
        }
    }
}

void Record::expired( void ) noexcept
{
    updateNVRAM();
}

void Record::request( FlushMsg& msg ) noexcept
{
    msg.getPayload().m_success = m_chunkHandler.updateData( *this );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Flush Request. mp[0]=%s. success=%d", m_items[0].mpPtr->getName(), msg.getPayload().m_success );
    msg.returnToSender();
}

void Record::request( EraseMsg& msg ) noexcept
{
    msg.getPayload().m_success = m_chunkHandler.updateData( *this, 0, true );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Erase Request. mp[0]=%s. success=%d", m_items[0].mpPtr->getName(), msg.getPayload().m_success );
    msg.returnToSender();
}

bool Record::flush() noexcept
{
    if ( !m_started )
    {
        return false;
    }

    Kit::Dm::Persistence::IManageRequest::FlushPayload payload;
    Kit::Itc::SyncReturnHandler                        srh;
    Kit::Dm::Persistence::IManageRequest::FlushMsg     msg( *this, payload, srh );
    m_myEventQueuePtr->postSync( msg );
    return payload.m_success;
}

bool Record::erase() noexcept
{
    if ( !m_started )
    {
        return false;
    }

    Kit::Dm::Persistence::IManageRequest::ErasePayload payload;
    Kit::Itc::SyncReturnHandler                        srh;
    Kit::Dm::Persistence::IManageRequest::EraseMsg     msg( *this, payload, srh );
    m_myEventQueuePtr->postSync( msg );
    return payload.m_success;
}
