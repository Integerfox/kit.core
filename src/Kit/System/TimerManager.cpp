/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "TimerManager.h"
#include "ElapsedTime.h"
#include "Kit/System/Trace.h"
#include <inttypes.h>

#define SECT_ "TimerManager"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {


/// Constructor
TimerManager::TimerManager()
    : m_counters( &m_listA )
    , m_pendingAttach( &m_listB )
    , m_timeNow( 0 )
    , m_inTickCall( false )
{
}

/////////////////////////////
void TimerManager::startManager( void ) noexcept
{
    m_timeNow = m_timeMark = ElapsedTime::milliseconds();
}

void TimerManager::processTimers( void ) noexcept
{
    // Calculate the elapsed time in milliseconds since we last checked the timers
    m_timeNow          = ElapsedTime::milliseconds();
    uint32_t deltaTime = ElapsedTime::deltaMilliseconds( m_timeMark, m_timeNow );

    // Update my timers
    KIT_SYSTEM_TRACE_MSG( SECT_, " @@ START TICK: %" PRIu32 ", now=%" PRIu32 ", [m_timeMark=%" PRIu32 "]", deltaTime, m_timeNow, m_timeMark );
    tick( deltaTime );
    m_timeMark = m_timeNow;
    KIT_SYSTEM_TRACE_MSG( SECT_, " @@ TICK COMPLETE..., m_timeMark=%" PRIu32, m_timeMark );
    tickComplete();
}

void TimerManager::tick( uint32_t msec ) noexcept
{
    // Set my state to: PROCESSING TICK(S)
    m_inTickCall = true;

    while ( msec )
    {
        ICounter* counterPtr = m_counters->first();
        if ( counterPtr == 0 )
        {
            // No timers registered -->do NOTHING
            break;
        }
        else
        {
            // Decrement the first/active counter in the list
            uint32_t nextCount = counterPtr->count();
            if ( msec <= nextCount )
            {
                counterPtr->decrement( msec );
                msec = 0;
            }
            else
            {
                counterPtr->decrement( nextCount );
                msec -= nextCount;
            }

            // Process ALL local Timers that have a ZERO countdown value
            while ( counterPtr && counterPtr->count() == 0 )
            {
                m_counters->get();                 // Remove the expired counter from the list
                counterPtr->expired();             // Expire the counter
                counterPtr = m_counters->first();  // Get next counter
            }
        }
    }
}

void TimerManager::tickComplete( void ) noexcept
{
    // Swap the pending list and the current list (which is now empty)
    auto temp       = m_counters;
    m_counters      = m_pendingAttach;
    m_pendingAttach = temp;

    // Clear my PROCESSING TICK(S) state
    m_inTickCall = false;
}

////////////////////////////////////
void TimerManager::addToActiveList( ICounter& clientToCallback ) noexcept
{
    // Insert the counter wisely into the list.  The counters are
    // stored in the list in the order they will expire and have their
    // raw counter value adjusted to be relative to any/all preceding list
    // elements.  This allows me to only decrement the first counter in
    // the list - instead of all counters every tick.
    ICounter* counterPtr = m_counters->first();
    while ( counterPtr )
    {
        // Insert at the head of the list
        if ( clientToCallback.count() < counterPtr->count() )
        {
            counterPtr->decrement( clientToCallback.count() );
            m_counters->insertBefore( *counterPtr, clientToCallback );
            KIT_SYSTEM_TRACE_MSG( SECT_, ">> INSERT: %p, count=%" PRIu32 ", BEFORE %p (%" PRIu32 ")", &clientToCallback, clientToCallback.count(), counterPtr, counterPtr->count() );
            return;
        }

        // Insert in the middle
        clientToCallback.decrement( counterPtr->count() );
        if ( clientToCallback.count() == 0 )
        {
            m_counters->insertAfter( *counterPtr, clientToCallback );
            KIT_SYSTEM_TRACE_MSG( SECT_, ">> INSERT:: %p, count=%" PRIu32 ", AFTER %p (%" PRIu32 ")", &clientToCallback, clientToCallback.count(), counterPtr, counterPtr->count() );
            return;
        }

        counterPtr = m_counters->next( *counterPtr );
    }

    // Insert at the tail (list is empty or largest counter value)
    KIT_SYSTEM_TRACE_MSG( SECT_, ">> INSERT @ end: %p, count=%" PRIu32, &clientToCallback, clientToCallback.count() );
    m_counters->putLast( clientToCallback );
}

/////////////////////////////////////
bool TimerManager::areActiveTimers( void ) const noexcept
{
    return m_counters->first() != nullptr;
}

void TimerManager::attach( ICounter& clientToCallback ) noexcept
{
    // Do NOT add to my active timer list while I am processing tick(s)!
    if ( m_inTickCall )
    {
        m_pendingAttach->put( clientToCallback );
    }

    // Add client timer
    else
    {
        addToActiveList( clientToCallback );
    }
}

bool TimerManager::detach( ICounter& clientToCallback ) noexcept
{
    // Try my pending list FIRST
    if ( m_pendingAttach->remove( clientToCallback ) )
    {
        return true;
    }

    // If there is timer-in-progress -->it will be in the active list.
    if ( m_counters->find( clientToCallback ) )
    {
        // Add the remaining time of the counter being remove to the next counter in the list
        ICounter* nextPtr = m_counters->next( clientToCallback );
        if ( nextPtr )
        {
            nextPtr->increment( clientToCallback.count() );
        }

        // remove the counter
        m_counters->remove( clientToCallback );
        return true;
    }

    // If I get here, the Counter was NOT in the active list (AND it was not in the staging list)
    return false;
}

uint32_t TimerManager::msecToCounts( uint32_t durationInMsecs ) const noexcept
{
    uint32_t delta = ElapsedTime::deltaMilliseconds( m_timeNow );
    KIT_SYSTEM_TRACE_MSG( SECT_, "milliseconds IN=%" PRIu32 ", count out=%" PRIu32, durationInMsecs, durationInMsecs + delta );
    return durationInMsecs + delta;
}

}  // end namespace
}
//------------------------------------------------------------------------------