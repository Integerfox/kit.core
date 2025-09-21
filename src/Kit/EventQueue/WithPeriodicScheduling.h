#ifndef KIT_EVENT_QUEUE_WITH_PERIODIC_SCHEDULING_H_
#define KIT_EVENT_QUEUE_WITH_PERIODIC_SCHEDULING_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/EventQueue/Server.h"
#include "Kit/System/PeriodicScheduler.h"


///
namespace Kit {
///
namespace EventQueue {

/** This class extends the Kit::EventQueue::Server class to add periodic scheduling
    to an event based 'thread'.  The timing resolution of the periodic scheduling
    is a determined by the 'timingTickInMsec' argument value in the class's
    constructor.  For example if 'timingTickInMsec' is 10ms then no interval
    should have a interval duration less than 10ms.

    The order of processing is:

       1. The Event Loop is unblocked.  Any of the following will trigger the
          Event Loop to 'wake-up'
          a. A ITC message was posted to its Event Queue
          b. A Event Flag was signaled
          c. A Data Model change notification was generated
          d. The EventLoop tick-timing expired
       2. All Event Flags are processed.
       3. The timers and their callbacks (if any timers have expired) are
          processed.
       4. The Periodic scheduler's executeScheduler() method is called
       5. The Event Queue is processed (e.g. ITC messages, DM Change notifications)
       6. The optional 'Idle' function is called.
       7. The loop is repeated until there are no expired timers, no event
          flags, no MP change notifications, and no ITC messages - at which
          point the thread blocks and waits till the next wake-up-action.

 */
class WithPeriodicScheduling : public Server, public Kit::System::PeriodicScheduler
{

public:
    /** Constructor.  The argument 'timingTickInMsec' specifies the timing
        resolution that will be used for Kit::Timer::Local Timers AND for the
        periodic scheduling.
     */
    WithPeriodicScheduling( Interval_T                                      intervals[],
                            Hook_T                                          beginThreadProcessing = nullptr,
                            Hook_T                                          endThreadProcessing   = nullptr,
                            ReportSlippageFunc_T                            slippageFunc          = nullptr,
                            NowFunc_T                                       nowFunc               = Kit::System::ElapsedTime::millisecondsEx,
                            IdleFunc_T                                      idleFunc              = nullptr,
                            uint32_t                                        timeOutPeriodInMsec   = OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
                            Kit::Container::SList<Kit::System::IEventFlag>* eventFlagsList        = nullptr ) noexcept;

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