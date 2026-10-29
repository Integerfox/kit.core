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
#include "Kit/System/GlobalLock.h"
#include "Kit/System/ElapsedTime.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace System {

BSemaphore::BSemaphore( bool initialStateIsOne ) noexcept
{
    m_sema = initialStateIsOne;
}

BSemaphore::~BSemaphore() noexcept
{
    // Nothing needed
}

int BSemaphore::signal() noexcept
{
    GlobalLock::begin();
    su_signal();
    GlobalLock::end();
    return 0;
}

int BSemaphore::su_signal() noexcept
{
    // Binary semantics -->the signaled state saturates at 'one'
    m_sema = true;
    return 0;
}

bool BSemaphore::tryWait() noexcept
{
    bool signaled = false;
    GlobalLock::begin();
    if ( m_sema )
    {
        m_sema   = false;
        signaled = true;
    }
    GlobalLock::end();

    return signaled;
}

void BSemaphore::waitInRealTime() noexcept
{
    for ( ;; )
    {
        GlobalLock::begin();
        if ( m_sema )
        {
            m_sema = false;
            GlobalLock::end();
            return;
        }
        GlobalLock::end();
    }
}

bool BSemaphore::timedWaitInRealTime( uint32_t timeoutMs ) noexcept
{
    uint32_t start = ElapsedTime::milliseconds();

    for ( ;; )
    {
        GlobalLock::begin();
        if ( m_sema )
        {
            m_sema = false;
            GlobalLock::end();
            return true;
        }
        GlobalLock::end();

        if ( ElapsedTime::expiredMilliseconds( start, timeoutMs ) )
        {
            return false;
        }
    }
}

bool BSemaphore::timedWait( uint32_t timeoutMs ) noexcept
{
    return timedWaitInRealTime( timeoutMs );
}

void BSemaphore::wait() noexcept
{
    waitInRealTime();
}

}  // end namespaces
}
//------------------------------------------------------------------------------
