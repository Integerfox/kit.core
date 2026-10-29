/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/BSemaphore.h"
#include "Kit/System/Api.h"


//////////////////////////////////////////////////
//------------------------------------------------------------------------------
namespace Kit {
namespace System {

extern bool g_kitInitialized;


BSemaphore::BSemaphore( bool initialStateIsOne ) noexcept
    : m_sema( initialStateIsOne )
{
    // Note: Semaphores created (including static allocated instances) before
    // the Kit C++ library is initialized are initialized as part of the KIT
    // library's initialization process
    if ( g_kitInitialized )
    {
        sem_init( &( m_sema.m_sdkSema ), initialStateIsOne ? 1 : 0, 1 );
    }
}

BSemaphore::~BSemaphore() noexcept
{
    // Nothing needed
}

int BSemaphore::signal() noexcept
{
    // Note: sem_release() returns false when the semaphore is already at its max
    //       permit count of one - which is the desired 'saturate at one' behavior
    sem_release( &( m_sema.m_sdkSema ) );
    return 0;
}

int BSemaphore::su_signal() noexcept
{
    sem_release( &( m_sema.m_sdkSema ) );
    return 0;
}

bool BSemaphore::tryWait() noexcept
{
    return sem_try_acquire( &( m_sema.m_sdkSema ) );
}

void BSemaphore::waitInRealTime() noexcept
{
    sem_acquire_blocking( &( m_sema.m_sdkSema ) );
}

bool BSemaphore::timedWaitInRealTime( uint32_t timeoutMs ) noexcept
{
    return sem_acquire_timeout_ms( &( m_sema.m_sdkSema ), timeoutMs );
}

//////////////////////////////////////////////////
// Simulated time NOT supported
bool BSemaphore::timedWait( uint32_t timeoutMs ) noexcept
{
    return sem_acquire_timeout_ms( &( m_sema.m_sdkSema ), timeoutMs );
}

void BSemaphore::wait() noexcept
{
    sem_acquire_blocking( &( m_sema.m_sdkSema ) );
}

}  // end namespaces
}
//------------------------------------------------------------------------------
