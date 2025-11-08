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
#include <errno.h>


//------------------------------------------------------------------------------
namespace Kit {
namespace System {




//////////////////////////////////////////////////
Semaphore::Semaphore( unsigned initialCount ) noexcept
{
    sem_init( &m_sema, 0, initialCount );
}

Semaphore::~Semaphore() noexcept
{
    sem_destroy( &m_sema );
}

int Semaphore::signal() noexcept
{
    return sem_post( &m_sema );
}

int Semaphore::su_signal() noexcept
{
    // TODO: Add log unexpected error/event since this isn't really supported
    return sem_post( &m_sema );
}

bool Semaphore::tryWait() noexcept
{
    int result = sem_trywait( &m_sema );
    return result == -1 && errno == EAGAIN ? false : true;
}

void Semaphore::waitInRealTime() noexcept
{
    sem_wait( &m_sema );
}


bool Semaphore::timedWaitInRealTime( uint32_t timeout ) noexcept
{
    // Convert milliseconds to the nanosleep time spec
    static const long nsec2msec = 1000000;
    static const long msec2sec  = 1000;
    time_t            sec       = timeout / msec2sec;
    long              nsec      = (timeout % msec2sec)*nsec2msec;

    struct timespec ts;
    clock_gettime( CLOCK_REALTIME, &ts );
    ts.tv_sec  += sec;
    ts.tv_nsec += nsec;
    if ( ts.tv_nsec > 999999999 )
    {
        ts.tv_nsec -= 1000000000;
        ts.tv_sec++;
    }

    int result = sem_timedwait( &m_sema, &ts );
    return result == -1 && errno == ETIMEDOUT ? false : true;
}

} // end namespaces
}
//------------------------------------------------------------------------------
