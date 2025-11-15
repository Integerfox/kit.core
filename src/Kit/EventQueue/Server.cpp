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


//------------------------------------------------------------------------------
namespace Kit {
namespace EventQueue {


/////////////////////
Server::Server( uint32_t                                        timeOutPeriodInMsec,
                Kit::Container::SList<Kit::System::IEventFlag>* eventFlagsList,
                Kit::System::IWatchedEventLoop*                 watchdog ) noexcept
    : Kit::Itc::Mailbox( *( static_cast<Kit::System::ISignable*>( this ) ) )
    , EventLoop( timeOutPeriodInMsec, eventFlagsList, watchdog )
{
}

/////////////////////
void Server::entry() noexcept
{
    startEventLoop();
    bool run = true;
    while ( run )
    {
        run = waitAndProcessEvents( isPendingMessage() );
        if ( run )
        {
            processMessages();
        }
    }
    stopEventLoop();
}

}  // end namespace
}
//------------------------------------------------------------------------------