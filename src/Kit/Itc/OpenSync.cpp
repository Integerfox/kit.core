/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "OpenSync.h"
#include "Kit/EventQueue/IQueue.h"
#include "SyncReturnHandler.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Itc {

///////////////////
OpenSync::OpenSync( Kit::EventQueue::Server& myEventLoop ) noexcept
    : m_eventLoop( myEventLoop )
{
}

///////////////////
bool OpenSync::open( void* args ) noexcept
{
    Payload_T         payload( args );
    SyncReturnHandler srh;
    OpenMsg           msg( *this, payload, srh );
    m_eventLoop.postSync( msg );
    return payload.success;
}

} // end namespace
}
//------------------------------------------------------------------------------