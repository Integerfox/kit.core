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
#include "Kit/System/GlobalLock.h"
#include "Kit/System/ElapsedTime.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace System {

Semaphore::Semaphore( unsigned initialCount ) noexcept
{
    m_sema = initialCount;
}

Semaphore::~Semaphore() noexcept
{
    // Nothing needed
}

int Semaphore::signal( void ) noexcept
{
    GlobalLock::begin();
    su_signal();
    GlobalLock::end();
    return 0;
}

int Semaphore::su_signal( void ) noexcept
{
    // Increment the semaphore (but don't let it roll over)
    m_sema++;
    if ( m_sema == 0 )
    {
        m_sema--;
    }
    return 0;
}

bool Semaphore::tryWait( void ) noexcept
{
    bool signaled = false;
    GlobalLock::begin();
    if ( m_sema > 0 )
    {
        m_sema--;
        signaled = true;
    }
    GlobalLock::end();

    return signaled;
}

void Semaphore::waitInRealTime( void ) noexcept
{
    for(;;)
    {
        GlobalLock::begin();
        if ( m_sema > 0 )
        {
            m_sema--;
            GlobalLock::end();
            return;
        }
        GlobalLock::end();
    }
}

bool Semaphore::timedWaitInRealTime( uint32_t timeout ) noexcept
{
    uint32_t start = ElapsedTime::milliseconds();

    for ( ;;)
    {
        GlobalLock::begin();
        if ( m_sema > 0 )
        {
            m_sema--;
            GlobalLock::end();
            return true;
        }
        GlobalLock::end();

        if ( ElapsedTime::expiredMilliseconds( start, timeout ) )
        {
            return false;
        }
    }
}

bool Semaphore::timedWait( uint32_t timeout ) noexcept
{
    return timedWaitInRealTime( timeout );
}

void Semaphore::wait( void ) noexcept
{
    waitInRealTime();
}

} // end namespace
}
//------------------------------------------------------------------------------

