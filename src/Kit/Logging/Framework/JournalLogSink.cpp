/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "JournalLogSink.h"
#include <stdint.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace Logging {
namespace Framework {

JournalLogSink::JournalLogSink( Kit::EventQueue::IQueue&                   myEventQueue,
                                Kit::Persistence::Record::Journal::IEntry& entryRecord,
                                Kit::Container::RingBufferMP<EntryData_T>& incomingEntriesBuffer,
                                const std::atomic<bool>*                   pauseUpdates ) noexcept
    : Kit::Persistence::Record::Journal::Server<EntryData_T>( myEventQueue, entryRecord, incomingEntriesBuffer )
    , m_pauseUpdates( pauseUpdates )
    , m_currentCycleBootCounter( 0 )
{
}

void JournalLogSink::request( OpenMsg& msg ) noexcept
{
    // Capture the boot counter for the current cycle (the boot-counter record has already been loaded) BEFORE
    // the base class starts draining the log buffer
    uint64_t currentElapsedMs = 0;
    Kit::Time::parseBootTime( Kit::Time::getBootTime(), m_currentCycleBootCounter, currentElapsedMs );

    // Forward the request to the base class implementation.
    Kit::Persistence::Record::Journal::Server<EntryData_T>::request( msg );
}

bool JournalLogSink::hookRemovingEntry( EntryData_T& entry ) noexcept
{
    // Updates are paused: leave the entries in the log buffer.  Relaxed ordering is sufficient: the flag does not
    // guard any other data, and pausing is only required to be timely
    if ( m_pauseUpdates != nullptr && m_pauseUpdates->load( std::memory_order_relaxed ) )
    {
        return false;
    }
    return m_buffer.remove( entry );
}

bool JournalLogSink::hookAddingEntry( EntryData_T& entry ) noexcept
{
    // Parse the entry's timestamp to get the boot counter and elapsed time.
    KitTimeBootCount_T entryBootCounter = 0;
    uint64_t           entryElapsedMs   = 0;
    Kit::Time::parseBootTime( entry.m_timestamp, entryBootCounter, entryElapsedMs );

    // Patch entries that do not have the correct boot counter, i.e. entries logged before the boot-counter record
    // was loaded (boot counter zero), or while it was loading (the previous boot's counter)
    if ( entryBootCounter != m_currentCycleBootCounter )
    {
        entry.m_timestamp = Kit::Time::constructBootTime( m_currentCycleBootCounter, entryElapsedMs );
    }

    return true;
}

}  // end namespaces
}
}
