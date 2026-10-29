/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/BSemaMutex.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {


//////////////////////////////////////////////////
BSemaMutex::BSemaMutex() noexcept
    : m_sema( true )  // Start 'signaled' -->the mutex is initially unlocked
{
}

void BSemaMutex::lock() noexcept
{
    // Bypass wait() so that taking the lock is not treated as a simulated-tick 'application wait'
    m_sema.waitInRealTime();
}

void BSemaMutex::unlock() noexcept
{
    m_sema.signal();
}

}  // end namespaces
}
//------------------------------------------------------------------------------
