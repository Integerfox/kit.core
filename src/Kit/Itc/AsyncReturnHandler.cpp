/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "AsyncReturnHandler.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Itc {

AsyncReturnHandler::AsyncReturnHandler( Kit::EventQueue::IQueue& clientEventQueue, IMessage& msg ) noexcept
    : m_clientEventQueue( clientEventQueue )
    , m_responseMsg( msg )
{
}

void AsyncReturnHandler::rts() noexcept
{
    m_clientEventQueue.post( m_responseMsg );
}

}  // end namespace
}
//------------------------------------------------------------------------------