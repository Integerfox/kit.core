#if 0
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "MailboxServer.h"
#include "Cpl/System/Thread.h"
#include "Cpl/System/FatalError.h"
#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/SimTick.h"
#include "Cpl/System/Trace.h"


#define SECT_ "Cpl::Itc"


///
using namespace Cpl::Itc;

/////////////////////
MailboxServer::MailboxServer( unsigned long                       timingTickInMsec,
                              Cpl::System::SharedEventHandlerApi* eventHandler ) noexcept
    : Mailbox( *((Cpl::System::Signable*)this) )
    , Cpl::System::EventLoop( timingTickInMsec, eventHandler )
{
}


/////////////////////
void MailboxServer::appRun()
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

#endif