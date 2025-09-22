/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/PeriodicScheduler.h"
#include "WithPeriodicScheduling.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace EventQueue {

///////////////////////
WithPeriodicScheduling::WithPeriodicScheduling( Interval_T                                      intervals[],
                                      Hook_T                                          beginThreadProcessing,
                                      Hook_T                                          endThreadProcessing,
                                      ReportSlippageFunc_T                            slippageFunc,
                                      NowFunc_T                                       nowFunc,
                                      IdleFunc_T                                      idleFunc,
                                      uint32_t                                        timeOutPeriodInMsec,
                                      Kit::Container::SList<Kit::System::IEventFlag>* eventFlagsList ) noexcept
    : Server( timeOutPeriodInMsec, eventFlagsList )
    , Kit::System::PeriodicScheduler( intervals, beginThreadProcessing, endThreadProcessing, slippageFunc, nowFunc )
    , m_idleFunc( idleFunc )
{
}

void WithPeriodicScheduling::entry() noexcept
{
    startEventLoop();
    beginLoop();

    bool run = true;
    while ( run )
    {
        run = waitAndProcessEvents( isPendingMessage() );
        if ( run )
        {
            bool atLeastOne = executeScheduler();
            processMessages();
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