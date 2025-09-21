/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "OpenCloseSync.h"
#include "SyncReturnHandler.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Itc {

///////////////////
OpenCloseSync::OpenCloseSync( Kit::EventQueue::Server& myEventLoop ) noexcept
    : OpenSync( myEventLoop )
{
}

///////////////////
bool OpenCloseSync::close( void* args ) noexcept
{
    ClosePayload_T          payload( args );
    SyncReturnHandler       srh;
    ICloseRequest::CloseMsg msg( *this, payload, srh );
    m_eventLoop.postSync( msg );
    return payload.success;
}

}  // end namespace
}
//------------------------------------------------------------------------------