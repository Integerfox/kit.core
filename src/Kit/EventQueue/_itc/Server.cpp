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
#include "Kit/System/Thread.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/SimTick.h"
#include "Kit/System/Trace.h"


#define SECT_ "Kit::Itc"


//------------------------------------------------------------------------------
namespace Kit {
namespace EventQueue {


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