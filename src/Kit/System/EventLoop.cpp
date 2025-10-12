/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "EventLoop.h"
#include "Assert.h"
#include "ElapsedTime.h"
#include "Kit/System/IEventFlag.h"
#include "Trace.h"
#include "GlobalLock.h"
#include "Kit/System/Watchdog/IWatchedEventLoop.h"

#define SECT_ "EventLoop"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

///////////////////////
EventLoop::EventLoop( uint32_t                           timeOutPeriodInMsec,
                      Kit::Container::SList<IEventFlag>* eventFlagsList,
                      IWatchedEventLoop*                 watchdog ) noexcept
    : IRunnable()
    , m_eventList( eventFlagsList )
    , m_timeout( timeOutPeriodInMsec )
    , m_timeStartOfLoop( 0 )
    , m_events( 0 )
    , m_run( true )
    , m_watchdog( watchdog )
{
    KIT_SYSTEM_ASSERT( timeOutPeriodInMsec > 0 );
}

void EventLoop::entry() noexcept
{
    startEventLoop();
    bool run = true;
    while ( run )
    {
        run = waitAndProcessEvents();
    }
    stopEventLoop();
}

void EventLoop::startEventLoop() noexcept
{
    // Initialize/start the timer manager
    startManager();
    m_run = true;

    // Start watchdog monitoring if enabled
    KIT_SYSTEM_WATCHDOG_START_EVENTLOOP( m_watchdog, *this );
}

void EventLoop::stopEventLoop() noexcept
{
    // Stop watchdog monitoring if enabled
    KIT_SYSTEM_WATCHDOG_STOP_EVENTLOOP( m_watchdog );
}

bool EventLoop::waitAndProcessEvents( bool skipWait ) noexcept
{
    // Trap my exit/please-stop condition
    GlobalLock::begin();
    bool stayRunning = m_run;
    GlobalLock::end();
    if ( !stayRunning )
    {
        return false;
    }

    // Skip waiting if it has been a 'long time' since we last processed events
    uint32_t now = ElapsedTime::milliseconds();
    if ( ElapsedTime::deltaMilliseconds( m_timeStartOfLoop, now ) > m_timeout )
    {
        skipWait = true;
    }
    m_timeStartOfLoop = now;

    // Wait for something to happen...
    if ( !skipWait )
    {
        m_sema.timedWait( m_timeout );  // Note: For Tick Simulation: the timedWait() calls topLevelWait() if the semaphore has not been signaled
    }

    // Trap my exit/please-stop condition AGAIN since a lot could have happen while I was waiting....
    GlobalLock::begin();
    stayRunning = m_run;
    GlobalLock::end();
    if ( !stayRunning )
    {
        return false;
    }

    // Capture the current state of the event flags
    GlobalLock::begin();
    uint32_t events = m_events;
    m_events        = 0;
    GlobalLock::end();

    // Process Event Flags
    if ( events && m_eventList )
    {
        // Walk my list of event flag handlers and notify the ones that are interested in the active event(s)
        IEventFlag* eventPtr = m_eventList->first();
        while ( eventPtr )
        {
            uint32_t activeEvents = events & eventPtr->getEventFlagsMask();
            if ( activeEvents )
            {
                eventPtr->notified( activeEvents );
            }
            eventPtr = m_eventList->next( *eventPtr );
        }
    }

    // Timer Check
    processTimers();

    // Watchdog monitoring (if enabled)
    KIT_SYSTEM_WATCHDOG_EVENTLOOP_MONITOR( m_watchdog );

    return true;
}

//////////////////////////////////////
int EventLoop::signal() noexcept
{
    return m_sema.signal();
}

int EventLoop::su_signal() noexcept
{
    return m_sema.su_signal();
}

void EventLoop::pleaseStop() noexcept
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );

    // Set my flag/state to exit my top level thread loop
    GlobalLock::begin();
    m_run = false;
    GlobalLock::end();

    // Signal myself in case the thread is blocked waiting for the 'next event'
    m_sema.signal();
}

void EventLoop::signalMultipleEvents( uint32_t events ) noexcept
{
    GlobalLock::begin();
    m_events |= events;
    GlobalLock::end();

    m_sema.signal();
}

void EventLoop::signalEvent( uint8_t eventNumber ) noexcept
{
    signalMultipleEvents( 1 << eventNumber );
}

void EventLoop::su_signalMultipleEvents( uint32_t events ) noexcept
{
    m_events |= events;
    m_sema.su_signal();
}

void EventLoop::su_signalEvent( uint8_t eventNumber ) noexcept
{
    su_signalMultipleEvents( 1 << eventNumber );
}

}  // end namespace
}
//------------------------------------------------------------------------------
