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
#include <semaphore.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace System {


//////////////////////////////////////////////////
void Semaphore::wait() noexcept
{
    sem_wait( &m_sema );
}


bool Semaphore::timedWait( uint32_t timeout ) noexcept
{
    return timedWaitInRealTime( timeout );
}

} // end namespace
}
//------------------------------------------------------------------------------