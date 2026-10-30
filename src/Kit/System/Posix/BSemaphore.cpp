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
#include "Kit/System/Posix/semwait_.h"
#include <errno.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

/// Collapses any accumulated counts -->this is what provides the binary semantics
static void drain_( sem_t& sema ) noexcept
{
    while ( sem_trywait( &sema ) == 0 )
    {
    }
}


//////////////////////////////////////////////////
BSemaphore::BSemaphore( bool initialStateIsOne ) noexcept
{
    sem_init( &m_sema, 0, initialStateIsOne ? 1 : 0 );
}

BSemaphore::~BSemaphore() noexcept
{
    sem_destroy( &m_sema );
}

int BSemaphore::signal() noexcept
{
    // Note: the count is collapsed when it is CONSUMED, not here -->see su_signal()
    sem_post( &m_sema );
    return 0;
}

int BSemaphore::su_signal() noexcept
{
    // sem_post() is the ONLY semaphore operation that POSIX requires to be
    // async-signal-safe, so this method must do nothing else
    sem_post( &m_sema );
    return 0;
}

bool BSemaphore::tryWait() noexcept
{
    if ( sem_trywait( &m_sema ) != 0 )
    {
        return false;
    }
    drain_( m_sema );
    return true;
}

void BSemaphore::waitInRealTime() noexcept
{
    while ( sem_wait( &m_sema ) != 0 && errno == EINTR )
    {
    }
    drain_( m_sema );
}

bool BSemaphore::timedWaitInRealTime( uint32_t timeoutMs ) noexcept
{
    if ( !Posix::semTimedWait_( m_sema, timeoutMs ) )
    {
        return false;
    }
    drain_( m_sema );
    return true;
}

}  // end namespaces
}
//------------------------------------------------------------------------------
