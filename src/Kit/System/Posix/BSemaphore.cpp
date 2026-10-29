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
#include <errno.h>

/// Nanoseconds per second
#define NSEC_PER_SEC_ 1000000000L

//------------------------------------------------------------------------------
namespace Kit {
namespace System {


//////////////////////////////////////////////////
BSemaphore::BSemaphore( bool initialStateIsOne ) noexcept
{
    pthread_mutex_init( &m_sema.m_mutex, 0 );

    // Use a monotonic clock so that timedWait() is immune to wall-clock changes
    pthread_condattr_t condAttr;
    pthread_condattr_init( &condAttr );
    pthread_condattr_setclock( &condAttr, CLOCK_MONOTONIC );
    pthread_cond_init( &m_sema.m_cond, &condAttr );
    pthread_condattr_destroy( &condAttr );

    m_sema.m_signaled = initialStateIsOne;
}

BSemaphore::~BSemaphore() noexcept
{
    pthread_cond_destroy( &m_sema.m_cond );
    pthread_mutex_destroy( &m_sema.m_mutex );
}

int BSemaphore::signal() noexcept
{
    pthread_mutex_lock( &m_sema.m_mutex );

    // Binary semantics -->the signaled state saturates at 'one'
    m_sema.m_signaled = true;
    pthread_cond_signal( &m_sema.m_cond );
    pthread_mutex_unlock( &m_sema.m_mutex );
    return 0;
}

int BSemaphore::su_signal() noexcept
{
    // TODO: Add log unexpected error/event since this isn't really supported
    return signal();
}

bool BSemaphore::tryWait() noexcept
{
    pthread_mutex_lock( &m_sema.m_mutex );
    bool signaled     = m_sema.m_signaled;
    m_sema.m_signaled = false;
    pthread_mutex_unlock( &m_sema.m_mutex );
    return signaled;
}

void BSemaphore::waitInRealTime() noexcept
{
    pthread_mutex_lock( &m_sema.m_mutex );

    // Loop to protect against spurious wake-ups
    while ( !m_sema.m_signaled )
    {
        pthread_cond_wait( &m_sema.m_cond, &m_sema.m_mutex );
    }
    m_sema.m_signaled = false;
    pthread_mutex_unlock( &m_sema.m_mutex );
}

bool BSemaphore::timedWaitInRealTime( uint32_t timeoutMs ) noexcept
{
    // Convert the relative timeout to the absolute deadline required by pthreads
    static const long msec2sec  = 1000;
    static const long nsec2msec = 1000000;

    struct timespec deadline;
    clock_gettime( CLOCK_MONOTONIC, &deadline );
    deadline.tv_sec  += (time_t)( timeoutMs / msec2sec );
    deadline.tv_nsec += (long)( ( timeoutMs % msec2sec ) * nsec2msec );
    if ( deadline.tv_nsec >= NSEC_PER_SEC_ )
    {
        deadline.tv_nsec -= NSEC_PER_SEC_;
        deadline.tv_sec++;
    }

    pthread_mutex_lock( &m_sema.m_mutex );
    while ( !m_sema.m_signaled )
    {
        if ( pthread_cond_timedwait( &m_sema.m_cond, &m_sema.m_mutex, &deadline ) == ETIMEDOUT )
        {
            break;
        }
    }

    bool signaled     = m_sema.m_signaled;
    m_sema.m_signaled = false;
    pthread_mutex_unlock( &m_sema.m_mutex );
    return signaled;
}

}  // end namespaces
}
//------------------------------------------------------------------------------
