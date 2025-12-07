/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"
#include "Kit/System/EventLoop.h"
#include "Kit/System/Thread.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/TimerManager.h"
#include "Kit/System/Tls.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Timer.h"
#include "kit_config.h"
#include <inttypes.h>


#define SECT_ "_0test"

///
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
namespace {

/// The time (in milliseconds) to wait between LED toggles
#ifndef OPTION_TEST_LED_DELAY_TIME_MS
#define OPTION_TEST_LED_DELAY_TIME_MS 500
#endif


class MyEventLoop : public Kit::System::EventLoop, public Kit::System::Timer
{
public:
    uint8_t  m_ledNum;
    Tls&     m_tlsKey;
    size_t   m_tlsCounter;
    size_t   m_maxCount;
    bool     m_started;
    uint32_t m_timeMarker;
public:
    ///
    MyEventLoop( Tls& tlsKey, uint8_t ledNum, size_t maxCount )
        : Kit::System::Timer( static_cast<Kit::System::TimerManager*>( this ) )
        , m_ledNum( ledNum )
        , m_tlsKey( tlsKey )
        , m_tlsCounter( ledNum )
        , m_maxCount( maxCount )
        , m_started( false )
        , m_timeMarker( 0 )
    {
    }

public:
    // Runs once
    void startEventLoop() noexcept override
    {
        // Call base class method
        EventLoop::startEventLoop();

        // Verify the many of the thread scheduling APIs work as expected
        if ( isSchedulingEnabled() == false )
        {
            FatalError::logf( Shutdown::eFAILURE,
                              "(%s) Scheduling NOT enabled at thread start",
                              Thread::myName() );
            return;
        }
        suspendScheduling();
        resumeScheduling();
        if ( isSchedulingEnabled() == false )
        {
            FatalError::logf( Shutdown::eFAILURE,
                              "(%s) Scheduling NOT enabled after resumeScheduling()",
                              Thread::myName() );
            return;
        }

        uint32_t now = ElapsedTime::milliseconds();
        sleep( OPTION_TEST_LED_DELAY_TIME_MS );
        uint32_t elapsed = ElapsedTime::deltaMilliseconds( now, ElapsedTime::milliseconds() );
        if ( elapsed < OPTION_TEST_LED_DELAY_TIME_MS || elapsed > ( (uint32_t)( OPTION_TEST_LED_DELAY_TIME_MS * 1.25 ) ) )
        {
            FatalError::logf( Shutdown::eFAILURE,
                              "(%s) Bad sleep duration (%" PRIu32 " ms). Expected around %" PRIu32 " ms",
                              Thread::myName(),
                              elapsed,
                              (uint32_t)OPTION_TEST_LED_DELAY_TIME_MS );
            return;
        }

        // Test some of the semaphore APIs (does not test su_signal or 'hard' wait methods)
        bool result = Thread::tryWait();
        if ( result == true )
        {
            FatalError::logf( Shutdown::eFAILURE,
                              "(%s) Thread::tryWait() returned true unexpectedly",
                              Thread::myName() );
            return;
        }
        Thread::getCurrent().signal();
        KIT_SYSTEM_TRACE_MSG( SECT_, "(%s) BEFORE Thread::wait() is called (with sema count:=1)...", Thread::myName() );
        Thread::wait();
        KIT_SYSTEM_TRACE_MSG( SECT_, "(%s) AFTER Thread::wait()", Thread::myName() );
        result = Thread::tryWait();
        if ( result == true )
        {
            FatalError::logf( Shutdown::eFAILURE,
                              "(%s) Thread::tryWait() returned true unexpectedly (wait() did not decrement the count or signal() didn't increment the count correctly)",
                              Thread::myName() );
            return;
        }
        Thread::getCurrent().signal();
        result = Thread::tryWait();
        if ( result == false )
        {
            FatalError::logf( Shutdown::eFAILURE,
                              "(%s) Thread::tryWait() returned false unexpectedly (after signal())",
                              Thread::myName() );
            return;
        }

        now = ElapsedTime::milliseconds();
        Thread::timedWait( OPTION_TEST_LED_DELAY_TIME_MS );
        elapsed = ElapsedTime::deltaMilliseconds( now, ElapsedTime::milliseconds() );
        if ( elapsed < OPTION_TEST_LED_DELAY_TIME_MS || elapsed > ( (uint32_t)( OPTION_TEST_LED_DELAY_TIME_MS * 1.25 ) ) )
        {
            FatalError::logf( Shutdown::eFAILURE,
                              "(%s) Bad Thread::timedWait duration (%" PRIu32 " ms). Expected around %" PRIu32 " ms",
                              Thread::myName(),
                              elapsed,
                              (uint32_t) OPTION_TEST_LED_DELAY_TIME_MS );
            return;
        }

        // Start the event loop
        KIT_SYSTEM_TRACE_MSG( SECT_, "(%s) Starting Test Event Loop...", Thread::myName() );
        m_tlsKey.set( &m_tlsCounter );
        m_started    = true;
        m_timeMarker = ElapsedTime::milliseconds();
        Timer::start( OPTION_TEST_LED_DELAY_TIME_MS );
    }

    bool waitAndProcessEvents( bool skipWait = false ) noexcept override
    {
        if ( !m_started )
        {
            FatalError::logf( Shutdown::eFAILURE,
                              "(%s) Event loop NOT started properly",
                              Thread::myName() );
            m_started = false;
            return false;
        }
        return EventLoop::waitAndProcessEvents( skipWait );
    }

    void stopEventLoop() noexcept override
    {
        // Call base class method
        if ( m_started )
        {
            EventLoop::stopEventLoop();
        }
    }

    // Test Event Loop
    void expired() noexcept override
    {
        // Capture the expired time
        uint32_t now = ElapsedTime::milliseconds();
        KIT_SYSTEM_TRACE_MSG( SECT_,
                              "(%s) Timer expired. TLS Counter=%zu",
                              Thread::myName(),
                              m_tlsCounter );

        // Exit the "loop"
        if ( m_tlsCounter >= m_maxCount )
        {
            pleaseStop();
            return;
        }

        // TLS validation
        size_t* tlsPtr = (size_t*)m_tlsKey.get();
        if ( tlsPtr == nullptr )
        {
            FatalError::logf( Shutdown::eFAILURE,
                              "(%s) Bad TLS value (nullptr)",
                              Thread::myName() );
            pleaseStop();
            return;
        }
        if ( *tlsPtr != m_tlsCounter )
        {
            FatalError::logf( Shutdown::eFAILURE,
                              "(%s) Bad TLS value (%p) - should be (%p)",
                              Thread::myName(),
                              (void*)*tlsPtr,
                              (void*)m_tlsCounter );
            pleaseStop();
            return;
        }
        m_tlsCounter++;
        *tlsPtr = m_tlsCounter;

        // Toggle the LED
        toggleLED();

        // Validate the timer interval
        uint32_t elapsed = ElapsedTime::deltaMilliseconds( m_timeMarker, now );
        if ( elapsed < OPTION_TEST_LED_DELAY_TIME_MS || elapsed > ( (uint32_t)( OPTION_TEST_LED_DELAY_TIME_MS * 1.25 ) ) )
        {
            FatalError::logf( Shutdown::eFAILURE,
                              "(%s) Bad Timer duration (%" PRIu32 " ms). Expected around %" PRIu32 " ms",
                              Thread::myName(),
                              elapsed,
                              (uint32_t) OPTION_TEST_LED_DELAY_TIME_MS );
            pleaseStop();
            return;
        }
        m_timeMarker = now;
        Timer::start( (uint32_t) OPTION_TEST_LED_DELAY_TIME_MS );
        sleep( (uint32_t) OPTION_TEST_LED_DELAY_TIME_MS );
    }

    void toggleLED()
    {
        if ( m_ledNum == 1 )
        {
            Bsp_toggle_debug1();
        }
        else
        {
            Bsp_toggle_debug2();
        }
    }
};

}
// end namespace


////////////////////////////////////////////////////////////////////////////////
void runtests( size_t maxLoopCount )
{
    // Create my TLS key (can't be done statically)
    Tls* keyPtr = new ( std::nothrow ) Tls();
    if ( !keyPtr )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, ( "Failed to create TLS key" ) );
        return;
    }

    // Create some threads....
    MyEventLoop* apple    = new ( std::nothrow ) MyEventLoop( *keyPtr, 1, maxLoopCount );
    Thread*      applePtr = Thread::create( *apple, "Apple" );
    if ( !applePtr )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, ( "Failed to create Apple thread" ) );
        return;
    }


    // Start the scheduler
    KIT_SYSTEM_TRACE_MSG( SECT_, ( "Starting scheduler..." ) );
    enableScheduling();  // Returns when the apple's entry() method ends

    // Cleanup
    Thread::destroy( *applePtr );
    return;
}
