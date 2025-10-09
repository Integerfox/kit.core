/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/IEventFlag.h"
#include "kit_config.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/Timer.h"
#include "Kit/System/EventLoop.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Api.h"
#include <inttypes.h>

#define SECT_ "_0test"

#ifndef NUM_SEQ_
#define NUM_SEQ_ 3
#endif
#define TOLERANCE_                2
#define DELAY_                    2000

#define START_EVENT_NUM           0
#define START_EVENT_MASK          ( 1 << START_EVENT_NUM )

#define FAST_TICKSRC_MS_PER_TICK  1
#define SLOW_TICKSRC_MS_PER_TICK  10

#define ROSE_T1                   10   // Tick Hz 1000 (1ms)
#define ROSE_T2                   23   // Tick Hz 1000 (1ms)
#define DAISY_T1                  11   // Tick Hz 1000 (1ms)
#define DAISY_T2                  47   // Tick Hz 1000 (1ms)
#define OAK_T1                    60   // Tick Hz 1000 (1ms)
#define OAK_T2                    125  // Tick Hz 1000 (1ms)
#define PINE_T1                   50   // Tick Hz 1000 (1ms)
#define PINE_T2                   300  // Tick Hz 1000 (1ms)
#define APPLE_T1                  50   // Tick Hz 20   (50ms)
#define APPLE_T2                  300  // Tick Hz 20   (50ms)
#define ORANGE_T1                 60   // Tick Hz 20   (50ms)
#define ORANGE_T2                 125  // Tick Hz 20   (50ms)


#define MAX_COUNT_( ttime, tdur ) ( ttime / tdur )


using namespace Kit::System;

////////////////////////////////////////////////////////////////////////////////
namespace {  // being anonymous namespace

class AppObject
{
public:
    ///
    const char* m_name;

    ///
    uint32_t m_timer1Duration;
    ///
    TimerComposer<AppObject> m_timer1;
    ///
    uint32_t m_count1;
    ///
    uint32_t m_startTime1;
    ///
    uint32_t m_minTime1;
    ///
    uint32_t m_maxTime1;
    ///
    uint32_t m_sumTime1;

    ///
    uint32_t m_timer2Duration;
    ///
    TimerComposer<AppObject> m_timer2;
    ///
    uint32_t m_count2;

    ///
    uint32_t m_timeMark1;
    ///
    uint32_t m_timeMark2;
    ///
    uint32_t m_deltaTime1;
    ///
    uint32_t m_deltaTime2;
    ///
    uint32_t m_startTime2;
    ///
    uint32_t m_minTime2;
    ///
    uint32_t m_maxTime2;
    ///
    uint32_t m_sumTime2;


    /// Constructor
    AppObject( const char*   name,
               TimerManager& timingSource,
               uint32_t      timer1Duration,
               uint32_t      timer2Duration )
        : m_name( name )
        , m_timer1Duration( timer1Duration )
        , m_timer1( *this, &AppObject::timer1Expired, timingSource )
        , m_timer2Duration( timer2Duration )
        , m_timer2( *this, &AppObject::timer2Expired, timingSource )
    {
    }

    /// Constructor
    AppObject( const char* name,
               uint32_t    timer1Duration,
               uint32_t    timer2Duration )
        : m_name( name )
        , m_timer1Duration( timer1Duration )
        , m_timer1( *this, &AppObject::timer1Expired, nullptr )
        , m_timer2Duration( timer2Duration )
        , m_timer2( *this, &AppObject::timer2Expired, nullptr )
    {
    }

public:
    ///
    void start()
    {
        KIT_SYSTEM_TRACE_MSG( SECT_,
                              "Starting timers (%" PRIu32 ", %" PRIu32 ") for: %s",
                              m_timer1Duration,
                              m_timer2Duration,
                              m_name );
        m_count1   = 0;
        m_minTime1 = (uint32_t)( -1 );
        m_maxTime1 = 0;
        m_sumTime1 = 0;
        m_count2   = 0;
        m_minTime2 = (uint32_t)( -1 );
        m_maxTime2 = 0;
        m_sumTime2 = 0;

        m_timeMark1 = m_startTime1 = ElapsedTime::milliseconds();
        m_timer1.start( m_timer1Duration );
        m_timeMark2 = m_startTime2 = ElapsedTime::milliseconds();
        m_timer2.start( m_timer2Duration );
    }

    ///
    void stop()
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Stopping AppObject: %s....", m_name );

        m_timer1.stop();
        m_deltaTime1 = ElapsedTime::deltaMilliseconds( m_timeMark1 );
        m_timer2.stop();
        m_deltaTime2 = ElapsedTime::deltaMilliseconds( m_timeMark1 );
    }

public:
    ///
    void timer1Expired() noexcept
    {
        // Capture stats
        uint32_t elasped = ElapsedTime::deltaMilliseconds( m_startTime1 );
        if ( elasped < m_minTime1 )
        {
            m_minTime1 = elasped;
        }
        if ( elasped > m_maxTime1 )
        {
            m_maxTime1 = elasped;
        }
        m_sumTime1 += elasped;

        m_count1++;
        KIT_SYSTEM_TRACE_MSG( SECT_,
                              "(%s)::Timer 1::Expired.  Total count=%" PRIu32 " (%" PRIu32 ")",
                              m_name,
                              m_count1,
                              m_timer1Duration );
        m_startTime1 = ElapsedTime::milliseconds();
        m_timer1.start( m_timer1Duration );
    }

    void timer2Expired() noexcept
    {
        // Capture stats
        uint32_t elasped = ElapsedTime::deltaMilliseconds( m_startTime2 );
        if ( elasped < m_minTime2 )
        {
            m_minTime2 = elasped;
        }
        if ( elasped > m_maxTime2 )
        {
            m_maxTime2 = elasped;
        }
        m_sumTime2 += elasped;

        m_count2++;
        KIT_SYSTEM_TRACE_MSG( SECT_,
                              "(%s)::Timer 2 ::Expired.  Total count=%" PRIu32 " (%" PRIu32 ")",
                              m_name,
                              m_count2,
                              m_timer2Duration );
        m_startTime2 = ElapsedTime::milliseconds();
        m_timer2.start( m_timer2Duration );
    }

public:
    void displayTimer1( uint32_t maxCount )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_,
                              "%s#1: delta=%" PRIu32 ", dur=%" PRIu32 " --> cnt (%" PRIu32 ") <=? %" PRIu32 ". min=%" PRIu32 ", max=%" PRIu32 ", avg=%.2f",
                              m_name,
                              m_deltaTime1,
                              m_timer1Duration,
                              m_count1,
                              maxCount,
                              m_minTime1,
                              m_maxTime1,
                              m_sumTime1 / (double)m_count1 );
    }

    void displayTimer2( uint32_t maxCount )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_,
                              "%s#2: delta=%" PRIu32 ", dur=%" PRIu32 " --> cnt (%" PRIu32 ") <=? %" PRIu32 ". min=%" PRIu32 ", max=%" PRIu32 ", avg=%.2f",
                              m_name,
                              m_deltaTime2,
                              m_timer2Duration,
                              m_count2,
                              maxCount,
                              m_minTime2,
                              m_maxTime2,
                              m_sumTime2 / (double)m_count2 );
    }
};


////////////////////////////////////////////////////////////////////////////////
class MasterT : public EventLoop, public Timer, public EventFlagBase
{
public:
    ///
    uint32_t m_delayMsecs;
    ///
    AppObject m_appObj1;
    ///
    AppObject m_appObj2;
    ///
    ISignable& m_waiter;

public:
    ///
    MasterT( uint32_t                           timerResolution,
             uint32_t                           delayMsecs,
             const char*                        nameObj1,
             uint32_t                           timer1DurationObj1,
             uint32_t                           timer2DurationObj1,
             const char*                        nameObj2,
             uint32_t                           timer1DurationObj2,
             uint32_t                           timer2DurationObj2,
             ISignable&                         waiter,
             Kit::Container::SList<IEventFlag>& eventList )
        : EventLoop( timerResolution, &eventList )
        , Timer( *( (TimerManager*)this ) )
        , EventFlagBase( eventList, START_EVENT_MASK )
        , m_delayMsecs( delayMsecs )
        , m_appObj1( nameObj1, *this, timer1DurationObj1, timer2DurationObj1 )
        , m_appObj2( nameObj2, *this, timer1DurationObj2, timer2DurationObj2 )
        , m_waiter( waiter )
    {
    }

    MasterT( uint32_t                           timerResolution,
             uint32_t                           delayMsecs,
             const char*                        nameObj1,
             uint32_t                           timer1DurationObj1,
             uint32_t                           timer2DurationObj1,
             const char*                        nameObj2,
             uint32_t                           timer1DurationObj2,
             uint32_t                           timer2DurationObj2,
             ISignable&                         waiter,
             Kit::Container::SList<IEventFlag>& eventList,
             const char*                        ignoredAlternateConstructorSelection )
        : EventLoop( timerResolution, &eventList )
        , Timer( *( (TimerManager*)this ) )
        , EventFlagBase( eventList, START_EVENT_MASK )
        , m_delayMsecs( delayMsecs )
        , m_appObj1( nameObj1, timer1DurationObj1, timer2DurationObj1 )
        , m_appObj2( nameObj2, timer1DurationObj2, timer2DurationObj2 )
        , m_waiter( waiter )
    {
    }
    void expired() noexcept
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "----- Stopping Timer Sequence...." );
        m_appObj1.stop();
        m_appObj2.stop();
        m_waiter.signal();
    }


public:
    ///
    uint32_t getEventFlagsMask() const noexcept
    {
        return START_EVENT_MASK;
    }
    ///
    void notified( uint32_t eventMask ) noexcept
    {
        if ( eventMask & START_EVENT_MASK )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "----- Starting Timer Sequence...." );
            start( m_delayMsecs );
            m_appObj1.start();
            m_appObj2.start();
        }
        else
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "----- UNSUPPORTED EVENT FLAG WAS SET: %" PRIx32, eventMask );
        }
    }
};

}  // end anonymous namespace

