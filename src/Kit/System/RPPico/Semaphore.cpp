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
#include "Kit/System/Api.h"


//////////////////////////////////////////////////
//------------------------------------------------------------------------------
namespace Kit {
namespace System {

extern bool g_kitInitialized;


Semaphore::Semaphore( unsigned initialCount )
    : m_sema( initialCount )
{
    // Note: Semaphores created (including static allocated instances) before
    // the Kit C++ library is initialized are initialized as part of the KIT
    // library's initialization process
    if ( g_kitInitialized )
    {
        sem_init( &(m_sema.m_sdkSema), initialCount, 0x7FFF );
    }
}

Semaphore::~Semaphore()
{
    // Nothing needed
}

int Semaphore::signal( void ) noexcept
{
    return sem_release( &(m_sema.m_sdkSema) )? 0: 1;    // Return zero on success
}

int Semaphore::su_signal( void ) noexcept
{
    return sem_release( &(m_sema.m_sdkSema) ) ? 0 : 1;  // Return zero on success
}

bool Semaphore::tryWait( void ) noexcept
{
    return sem_try_acquire( &(m_sema.m_sdkSema) );
}

void Semaphore::waitInRealTime( void ) noexcept
{
    sem_acquire_blocking( &(m_sema.m_sdkSema) );
}

bool Semaphore::timedWaitInRealTime( uint32_t timeoutMs ) noexcept
{
    return sem_acquire_timeout_ms( &(m_sema.m_sdkSema), timeoutMs );
}

//////////////////////////////////////////////////
// Simulated time NOT supported
bool Semaphore::timedWait( uint32_t timeoutMs ) noexcept
{
    return sem_acquire_timeout_ms( &(m_sema.m_sdkSema), timeoutMs );
}

void Semaphore::wait( void ) noexcept
{
    sem_acquire_blocking( &(m_sema.m_sdkSema) );
}

} // end namespace
}
//------------------------------------------------------------------------------