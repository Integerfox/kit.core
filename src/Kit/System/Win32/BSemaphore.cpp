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

//------------------------------------------------------------------------------
namespace Kit {
namespace System {


//////////////////////////////////////////////////
BSemaphore::BSemaphore( bool initialStateIsOne ) noexcept
{
    // An auto-reset Event is used (instead of a max-count-of-one Semaphore) because
    // SetEvent() on an already-signaled Event is a no-op instead of an error
    m_sema = CreateEvent( 0, FALSE, initialStateIsOne ? TRUE : FALSE, 0 );
}

BSemaphore::~BSemaphore() noexcept
{
    CloseHandle( m_sema );
}

int BSemaphore::signal() noexcept
{
    return SetEvent( m_sema ) ? 0 : -1;
}

int BSemaphore::su_signal() noexcept
{
    return SetEvent( m_sema ) ? 0 : -1;
}

bool BSemaphore::tryWait() noexcept
{
    DWORD result = WaitForSingleObject( m_sema, 0L );
    return result == WAIT_OBJECT_0 ? true : false;
}

void BSemaphore::waitInRealTime() noexcept
{
    WaitForSingleObject( m_sema, INFINITE );
}

bool BSemaphore::timedWaitInRealTime( uint32_t timeoutMs ) noexcept
{
    DWORD result = WaitForSingleObject( m_sema, (DWORD)timeoutMs );
    return result == WAIT_OBJECT_0 ? true : false;
}

}  // end namespaces
}
//------------------------------------------------------------------------------
