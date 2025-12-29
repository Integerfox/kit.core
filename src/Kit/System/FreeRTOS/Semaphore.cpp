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
#include "Kit/System/FatalError.h"
#include "Kit/System/Shutdown.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace System {

//////////////////////////////////////////////////
Semaphore::Semaphore( unsigned initialCount ) noexcept
{
    m_sema = xSemaphoreCreateCounting( 0x7FFF, initialCount );
    if ( m_sema == nullptr )
    {
        FatalError::logf( Shutdown::eOSAL, "Kit:System::Semaphore::Semaphore().  Failed to create semaphore" );
    }
}

Semaphore::~Semaphore() noexcept
{
    vSemaphoreDelete( &m_sema );
}


//////////////////////////////////////////////////
int Semaphore::signal() noexcept
{
    xSemaphoreGive( m_sema );
    return 0;
}


/** NOTE: This method returns the 'higherPriorityTaskWoken' instead of the
          defined 'return zero on success' semantics.  This is to overcome
          the oddities of FreeRTOS.
 */
int Semaphore::su_signal() noexcept
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR( m_sema, &higherPriorityTaskWoken );
    return higherPriorityTaskWoken;
}


//////////////////////////////////////////////////
void Semaphore::wait() noexcept
{
    xSemaphoreTake( m_sema, portMAX_DELAY );
}


bool Semaphore::timedWait( uint32_t timeout ) noexcept
{
    return xSemaphoreTake( m_sema, timeout * portTICK_PERIOD_MS ) == pdTRUE;
}


bool Semaphore::tryWait() noexcept
{
    return xSemaphoreTake( m_sema, 0 ) == pdTRUE;
}


void Semaphore::waitInRealTime() noexcept
{
    xSemaphoreTake( m_sema, portMAX_DELAY );
}


bool Semaphore::timedWaitInRealTime( uint32_t timeout ) noexcept
{
    return xSemaphoreTake( m_sema, timeout * portTICK_PERIOD_MS ) == pdTRUE;
}

} // end namespace
}
//------------------------------------------------------------------------------

