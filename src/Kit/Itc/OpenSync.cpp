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
OpenSync::OpenSync( Kit::EventQueue::IQueue& myEventQueue ) noexcept
    : m_eventQueue( myEventQueue )
{
}

///////////////////
bool OpenSync::open( void* args ) noexcept
{
    OpenPayload_T     payload( args );
    SyncReturnHandler srh;
    OpenMsg           msg( *this, payload, srh );
    m_eventQueue.postSync( msg );
    return payload.success;
}

}  // end namespace
}
//------------------------------------------------------------------------------