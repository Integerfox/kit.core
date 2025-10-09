/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "SyncReturnHandler.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Itc {

SyncReturnHandler::SyncReturnHandler( Kit::System::ISignable& clientSemaphore ) noexcept
    : m_waiter( clientSemaphore )
{
}

void SyncReturnHandler::rts() noexcept
{
    m_waiter.signal();
}

}  // end namespace
}
//------------------------------------------------------------------------------