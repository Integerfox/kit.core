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
#include "Kit/System/FatalError.h"
#include "Kit/System/Shutdown.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

//////////////////////////////////////////////////
BSemaphore::BSemaphore( bool initialStateIsOne ) noexcept
{
    m_sema = xSemaphoreCreateBinary();
    if ( m_sema == nullptr )
    {
        FatalError::logf( Shutdown::eOSAL, "Kit:System::BSemaphore::BSemaphore().  Failed to create semaphore" );
    }

    // A binary semaphore is always created 'empty'
    if ( initialStateIsOne )
    {
        xSemaphoreGive( m_sema );
    }
}

BSemaphore::~BSemaphore() noexcept
{
    vSemaphoreDelete( m_sema );
}


//////////////////////////////////////////////////
int BSemaphore::signal() noexcept
{
    // Note: A give() on an already-signaled binary semaphore fails - which is the desired 'saturate at one' behavior
    xSemaphoreGive( m_sema );
    return 0;
}


/** NOTE: This method returns the 'higherPriorityTaskWoken' instead of the
          defined 'return zero on success' semantics.  This is to overcome
          the oddities of FreeRTOS.
 */
int BSemaphore::su_signal() noexcept
{
    BaseType_t higherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR( m_sema, &higherPriorityTaskWoken );
    return higherPriorityTaskWoken;
}


//////////////////////////////////////////////////
void BSemaphore::wait() noexcept
{
    xSemaphoreTake( m_sema, portMAX_DELAY );
}


bool BSemaphore::timedWait( uint32_t timeoutMs ) noexcept
{
    return xSemaphoreTake( m_sema, KitSystemFreeRTOS_msecToTicks( timeoutMs ) ) == pdTRUE;
}


bool BSemaphore::tryWait() noexcept
{
    return xSemaphoreTake( m_sema, 0 ) == pdTRUE;
}


void BSemaphore::waitInRealTime() noexcept
{
    xSemaphoreTake( m_sema, portMAX_DELAY );
}


bool BSemaphore::timedWaitInRealTime( uint32_t timeoutMs ) noexcept
{
    return xSemaphoreTake( m_sema, KitSystemFreeRTOS_msecToTicks( timeoutMs ) ) == pdTRUE;
}

}  // end namespaces
}
//------------------------------------------------------------------------------
