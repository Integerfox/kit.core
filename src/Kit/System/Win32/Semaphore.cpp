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

//------------------------------------------------------------------------------
namespace Kit {
namespace System {


//////////////////////////////////////////////////
Semaphore::Semaphore( unsigned initialCount ) noexcept
{
    m_sema = CreateSemaphore( 0, initialCount, 0x7FFF, 0 ); // Create in the "EMPTY" state
}

Semaphore::~Semaphore() noexcept
{
    CloseHandle( m_sema );
}

int Semaphore::signal() noexcept
{
    return ReleaseSemaphore( m_sema, 1, 0 ) ? 0 : -1;
}

int Semaphore::su_signal() noexcept
{
    return ReleaseSemaphore( m_sema, 1, 0 ) ? 0 : -1;
}

bool Semaphore::tryWait() noexcept
{
    DWORD result = WaitForSingleObject( m_sema, 0L );
    return result == WAIT_OBJECT_0 ? true : false;
}

void Semaphore::waitInRealTime() noexcept
{
    WaitForSingleObject( m_sema, INFINITE );
}

bool Semaphore::timedWaitInRealTime( uint32_t timeout ) noexcept
{
    DWORD result = WaitForSingleObject( m_sema, (DWORD) timeout );
    return result == WAIT_OBJECT_0 ? true : false;
}

} // end namespace
}
//------------------------------------------------------------------------------