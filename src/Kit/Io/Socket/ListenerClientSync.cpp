/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "ListenerClientSync.h"
#include "Kit/Io/Socket/IListenerClientRequest.h"
#include "Kit/Io/Types.h"
#include "Kit/Itc/SyncReturnHandler.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace Socket {

///////////////////////
ListenerClientSync::ListenerClientSync( Kit::EventQueue::IQueue& myEventQueue ) noexcept
    : m_eventQueue( myEventQueue )
{
}


///////////////////////
bool ListenerClientSync::newConnection( KitIoSocketHandle_T newFd, const char* rawConnectionInfo ) noexcept
{
    IListenerClientRequest::Payload_T payload( newFd, rawConnectionInfo );
    Kit::Itc::SyncReturnHandler       srh;
    NewConnectionMsg                  msg( *this, payload, srh );
    m_eventQueue.postSync( msg );
    return payload.m_accepted;
}

}  // end namespace
}
}
//------------------------------------------------------------------------------