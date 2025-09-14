/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/Semaphore.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {


//////////////////////////////////////////////////
bool Semaphore::timedWait( uint32_t timeout ) noexcept
{
    DWORD result = WaitForSingleObject( m_sema, (DWORD) timeout );
    return result == WAIT_OBJECT_0 ? true : false;
}

void Semaphore::wait() noexcept
{
    WaitForSingleObject( m_sema, INFINITE );
}

} // end namespace
}
//------------------------------------------------------------------------------