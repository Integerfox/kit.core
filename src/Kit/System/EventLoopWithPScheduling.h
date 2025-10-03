#ifndef KIT_SYSTEM_EVENT_LOOP_WITH_PSCHEDULINGH_
#define KIT_SYSTEM_EVENT_LOOP_WITH_PSCHEDULINGH_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/EventLoop.h"
#include "Kit/System/PeriodicScheduler.h"


///
namespace Kit {
///
namespace System {

/** This class extends the EventLoop class to add periodic scheduling to an
    event based 'thread'.  The timing resolution of the periodic scheduling is a
    determined by the 'timingTickInMsec' argument value in the class's constructor.
    For example if 'timingTickInMsec' is 10ms then no interval should have a
    interval duration less than 10ms.

    The order of processing is:

       1. The Event Loop signal, Event Flags, and Software Timers are processed.
       2. The Periodic scheduler's executeScheduler() method is called
       3. The optional 'Idle' function is called.
       4. The loop is repeated until there are no expired timers, and no event
          flags - at which point the thread blocks and wait for any of the above
          asynchronous actions to wake up the thread.
 */
class EventLoopWithPScheduling : public EventLoop, public PeriodicScheduler
{
public:
    /** Constructor.  The argument 'timingTickInMsec' specifies the timing
        resolution that will be used for Kit::System::Timers AND for the
        periodic scheduling.

        The 'watchdog' parameter is optional and can be used to provide
        watchdog monitoring for this event loop with periodic scheduling.
     */
    EventLoopWithPScheduling( Interval_T                         intervals[],
                              Hook_T                             beginThreadProcessing = nullptr,
                              Hook_T                             endThreadProcessing   = nullptr,
                              ReportSlippageFunc_T               slippageFunc          = nullptr,
                              NowFunc_T                          nowFunc               = ElapsedTime::millisecondsEx,
                              IdleFunc_T                         idleFunc              = nullptr,
                              uint32_t                           timeOutPeriodInMsec   = OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
                              Kit::Container::SList<IEventFlag>* eventFlagsList        = nullptr,
#ifdef USE_KIT_SYSTEM_WATCHDOG
                              Kit::System::Watchdog::WatchedEventLoopApi* watchdog = nullptr
#else
                              void* watchdog = nullptr  // Placeholder when watchdog is disabled
#endif
                              ) noexcept;


public:
    /// See Kit::System::IRunnable
    void entry() noexcept override;

protected:
    /// Cache the Idle function pointer
    IdleFunc_T m_idleFunc;
};

}  // end namespaces
}
#endif  // end header latch
