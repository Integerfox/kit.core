#ifndef KIT_SYSTEM_TIMER_H_
#define KIT_SYSTEM_TIMER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/TimerManager.h"

///
namespace Kit {
///
namespace System {

/** This mostly concrete interface defines the operations that can be performed
    on a software timer.  Software timers execute 'in-thread' in that all
    operations (start, timer expired callbacks, etc.) are performed in a single
    thread.

    Because the timer context (i.e. the timer owner), timer methods and callbacks
    all occur in the same thread, the timer context will never receive a timer
    expired callback AFTER the timer's stop() method has been called.

    NOTES:
        o The timer context must implement the following method:
                virtual void expired( void ) noexcept;

        o Because the timing source of an individual thread may NOT be a clean
          divider of the timer duration, the timer duration is taken as the
          minimum.  For example: if the timing source has a resolution of 20msec
          per count, and the timer duration on the start() timer call is 5 msec,
          then the timer will expire after the next full count, i.e. after 20msec,
          not 5msec.  IT IS THE APPLICATION'S RESPONSIBILITY TO MANAGE THE
          RESOLUTION OF THE TIMING SOURCES.
 */
class Timer : public ICounter
{
public:
    /// Constructor
    Timer( TimerManager& timingSource ) noexcept;

    /// Constructor. Alternate constructor - that optionally defers the assignment of the timing source
    Timer( TimerManager* timingSource = nullptr ) noexcept;

public:
    /** Starts the timer with an initial count down count duration of
        'timerDurationInMilliseconds'.  If the timer is currently running, the
        timer is first stopped, and then restarted.
     */
    virtual void start( uint32_t timerDurationInMilliseconds ) noexcept;

    /** Stops the timer. It is okay to call stop() even after the timer has
        previously expired or explicitly stopped.
     */
    virtual void stop() noexcept;

public:
    /** Sets the timing source.  This method CAN ONLY BE CALLED when the timer
        has never been started or it has been stopped
     */
    virtual void setTimingSource( TimerManager& timingSource ) noexcept;

protected:
    /// See Cpl::System::ICounter
    void decrement( uint32_t milliseconds = 1 ) noexcept override;

    /// See Cpl::System::ICounter
    void increment( uint32_t milliseconds = 1 ) noexcept override;

    /// See Cpl::System::ICounter
    uint32_t count() const noexcept override;

protected:
    /// The timer's tick source
    TimerManager* m_timingSource;

    /// Current count
    uint32_t m_count;
};


/** This template class implements a Software Timer that is context
    independent and allows for a single context to contain many Timers.

    Template args:
        CONTEXT   Type of the Class that implements the context for the timer
  */

template <class CONTEXT>
class TimerComposer : public Timer
{
public:
    /** Definition of the call-back method that notifies the
        context/client when the timer expires.
     */
    typedef void ( CONTEXT::*TimerExpiredFunction_T )();

public:
    /// Constructor
    TimerComposer( CONTEXT&               timerContextInstance,
                   TimerExpiredFunction_T expiredCallbackFunc,
                   TimerManager&          timingSource ) noexcept
        : Timer( timingSource )
        , m_context( timerContextInstance )
        , m_expiredFuncPtr( expiredCallbackFunc )
    {
    }

    /** Alternate Constructor that is used to defer the assignment of the time source.
        When using this constructor - the Application logic is REQUIRED to use
        the setTimingSource() method to set the timing source BEFORE the timer
        is used.
     */
    TimerComposer( CONTEXT&               timerContextInstance,
                   TimerExpiredFunction_T expiredCallbackFunc,
                   TimerManager*          timingSourcePtr = nullptr ) noexcept
        : Timer( timingSourcePtr )
        , m_context( timerContextInstance )
        , m_expiredFuncPtr( expiredCallbackFunc )
    {
    }

protected:
    /// See Cpl::System::ICounter
    void expired() noexcept
    {
        ( m_context.*m_expiredFuncPtr )();
    }

protected:
    /// The callback context, i.e. the class that implements the 'expiredCallbackFunc' method
    CONTEXT& m_context;

    /// The context's method for 'expiredCallbackFunc' method, i.e. this method gets called when the timer expires
    TimerExpiredFunction_T m_expiredFuncPtr;
};

}  // end namespaces
}
#endif  // end header latch
