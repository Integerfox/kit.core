#ifndef KIT_SYSTEM_EVENT_LOOP_H_
#define KIT_SYSTEM_EVENT_LOOP_H_
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
#include "Kit/System/IRunnable.h"
#include "Kit/System/ISignable.h"
#include "Kit/System/IEventManager.h"
#include "Kit/System/Semaphore.h"
#include "Kit/System/TimerManager.h"

/** Specifies the default timeout period for waiting on a event.
 */
#ifndef OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD
#define OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD 1  //!< 1 msec timeout, aka 1 msec timer resolution for Software Timers
#endif

///
namespace Kit {
///
namespace System {

/** This concrete class is a IRunnable object that provides a event driven
    execution model for a thread.  The thread will remaining blocked until
    an "event" occurs.  The following "events" are supported:

    1) The EventLoop Semaphore was signaled (this is separate from the Thread Semaphore)
    2) A Event Flag was signaled
    3) A Software Timer expiring

    To receive a callbacks when Event Flags are signaled, the application must
    provide a list of Event Flag instances to the Event Loop constructor.

    Child classes can be used to extend the 'event processing' to include other
    types of events, e.g. Inter-Thread-Communication messaging.

    NOTE: The EventLoop does NOT use/consume the Thread Semaphore.
 */
class EventLoop : public IRunnable, public IEventManager, public ISignable, public TimerManager
{
public:
    /** Constructor. The 'timeOutPeriodInMsec' parameter specifies how
        long the EventLoop will wait for an event before timing out and
        processing the software timers.  The value of 'timeOutPeriodInMsec' is
        the resolution of the timing source for the Software Timers, i.e. the
        event loop will 'wake-up' at least every 'timeOutPeriodInMsec'
        milliseconds.

        A fatal error is generated if 'timeOutPeriodInMsec' is set to zero.

        When the 'eventFlagsList' parameter is null then the EventLoop will not
        monitor any event flags.

        WARNING: The application CANNOT modify the eventFlagsList after the
                 EventLoop instance is created.
     */
    EventLoop( uint32_t                           timeOutPeriodInMsec = OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
               Kit::Container::SList<IEventFlag>* eventFlagsList      = nullptr ) noexcept;

    /// Virtual destructor
    virtual ~EventLoop() = default;


protected:
    /** This method is used to initialize the Event Loop's thread has started
        to executed.

        This method is intended to be used by child classes that are extending
        the Event Loop.  For this use case - this method MUST be called once on
        the beginning of the entry() method and BEFORE the "main loop" for the
        entry() method is entered.
     */
    virtual void startEventLoop() noexcept;

    /** This method is used to wait (and process) the next event(s).  This
        method returns after being woken up and processing all timer expired
        and pending Event Flag events. This method should always be wrapped
        in a loop (typically a forever loop).

        If the 'skipWait' argument is set to true then the method does not
        wait for event - it simply processing the current event flags and
        timers. The use scenario for skipping the wait is for child classes
        to execute the event loop multiple times to drain their message/notifications
        queues one message/notification at a time (i.e. give equal time/priority
        to all types of events)

        The method typically returns true.  The method only returns false if
        the pleaseStop() method was called on the Event Loop instance.

        This method is intended to be used by child classes that are extending
        the Event Loop.  For this use case - this method MUST be called inside
        the child class's "main loop" in its the appRun() method.

        Example appRun() for a child class extending the Event Loop:
        @code

        void ChildEventLoop::entry()
        {
            startEventLoop();
            <child specific initialization>

            bool run = true;
            while( run )
            {
                run = waitAndProcessEvents();
                if ( run )
                {
                    <child specific event processing>
                }
            }
            stopEventLoop()
        }

        @endcode
     */
    virtual bool waitAndProcessEvents( bool skipWait = false ) noexcept;

    /** This method is used to clean-up the Event Loop's when the thread is
        being stopped.

        This method is intended to be used by child classes that are extending
        the Event Loop.  For this use case - this method MUST be called once
        AFTER the event-processing loop has exited.
     */
    virtual void stopEventLoop() noexcept;

public:
    /// See Cpl::System::ISignable
    int signal() noexcept override;

    /// See Cpl::System::ISignable
    int su_signal() noexcept override;


public:
    /// See Cpl::System::IRunnable
    void pleaseStop() noexcept override;


protected:
    /// See Cpl::System::IRunnable
    void entry() noexcept override;

public:
    /// See Cpl::System::IEventManager
    void signalMultipleEvents( uint32_t events ) noexcept override;

    /// See Cpl::System::IEventManager
    void signalEvent( uint8_t eventNumber ) noexcept override;

    /// See Cpl::System::IEventManager
    void su_signalMultipleEvents( uint32_t events ) noexcept override;

    /// See Cpl::System::IEventManager
    void su_signalEvent( uint8_t eventNumber ) noexcept override;

protected:
    /// Semaphore associated with the EventLoop (note: the Thread semaphore is NOT used)
    Semaphore m_sema;

    /// List of Event Flags that the Event Loop monitors (can be null)
    Kit::Container::SList<IEventFlag>* m_eventList;

    /// Timeout period for waiting on the next event
    uint32_t m_timeout;

    /// Timestamp, in milliseconds, of start of event/wait loop
    uint32_t m_timeStartOfLoop;

    /// The variable holds the current state of all Event Flags
    uint32_t m_events;

    /// Flag used to help with the pleaseStop() request
    volatile bool m_run;
};

}  // end namespaces
}
#endif  // end header latch
