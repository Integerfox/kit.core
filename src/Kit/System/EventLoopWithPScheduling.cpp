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
#include "EventLoopWithPScheduling.h"
#include "Kit/System/EventLoop.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace System {

///////////////////////
EventLoopWithPScheduling::EventLoopWithPScheduling( Interval_T                         intervals[],
                                                    Hook_T                             beginThreadProcessing,
                                                    Hook_T                             endThreadProcessing,
                                                    ReportSlippageFunc_T               slippageFunc,
                                                    NowFunc_T                          nowFunc,
                                                    IdleFunc_T                         idleFunc,
                                                    uint32_t                           timeOutPeriodInMsec,
                                                    Kit::Container::SList<IEventFlag>* eventFlagsList,
#ifdef USE_KIT_SYSTEM_WATCHDOG
                                                    IWatchedEventLoop* watchdog
#else
                                                    void* watchdog
#endif
                                                    ) noexcept
    : EventLoop( timeOutPeriodInMsec, eventFlagsList, watchdog )
    , PeriodicScheduler( intervals, beginThreadProcessing, endThreadProcessing, slippageFunc, nowFunc )
    , m_idleFunc( idleFunc )
{
}

void EventLoopWithPScheduling::entry() noexcept
{
    startEventLoop();
    beginLoop();

    bool run = true;
    while ( run )
    {
        run = waitAndProcessEvents();
        if ( run )
        {
            bool atLeastOne = executeScheduler();
            if ( m_idleFunc )
            {
                ( m_idleFunc )( (m_nowFunc)(), atLeastOne );
            }
        }
    }

    endLoop();
    stopEventLoop();
}

}  // end namespace
}
//------------------------------------------------------------------------------