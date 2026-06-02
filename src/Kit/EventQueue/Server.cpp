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
#include "Kit/System/FatalError.h"
#include "Kit/System/Thread.h"


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

void Server::pleaseStop() noexcept
{
    // Contract: all ITC activity for this thread must already be shut down
    // before pleaseStop() is requested.
    if ( Mailbox::isPendingMessage() )
    {
        auto *ownerThread = getThread();
        Kit::System::FatalError::logf( Kit::System::Shutdown::eITC,
                                       "There are pending ITC message(s) for thread=%s",
                                       ownerThread? ownerThread->getName() : "<unknown>");
    }
    EventLoop::pleaseStop();
}

void Server::postSync( Kit::Itc::IMessage& msg ) noexcept
{
    // Contract: a server cannot synchronously post to itself because it would
    // block the owner thread and deadlock request processing.
    auto* ownerThread   = getThread();
    auto* currentThread = Kit::System::Thread::tryGetCurrent();
    if ( ownerThread && currentThread && ownerThread == currentThread )
    {
        Kit::System::FatalError::logf( Kit::System::Shutdown::eITC,
                                       "Deadlock contract violation: postSync() called from owning thread=%s",
                                       ownerThread->getName() );
        return;
    }

    Mailbox::postSync( msg );
}

void Server::entry() noexcept
{
    startEventLoop();
    bool run = true;
    while ( run )
    {
        run = serverWaitAndProcessEvents();
        if ( run )
        {
            serverProcessEvents();
        }
    }
    stopEventLoop();
}


}  // end namespace
}
//------------------------------------------------------------------------------