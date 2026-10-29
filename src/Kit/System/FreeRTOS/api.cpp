/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/Api.h"
#include "Kit/System/Private.h"
#include "Kit/System/FatalError.h"
#include "Kit/Bsp/Api.h"
#include "FreeRTOS.h"
#include "Kit/System/PrivateStartup.h"
#include "task.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace System {

///
static bool schedulerStarted_ = false;

////////////////////////////////////////////////////////////////////////////////
void initialize() noexcept
{
    // Init the Colony.Core sub-systems
    IStartupHook::notifyStartupClients();
}


// This method should never return
void enableScheduling() noexcept
{
    schedulerStarted_ = true;  // Manually track the scheduler state since xTaskGetSchedulerState() returns 'taskSCHEDULER_RUNNING' BEFORE I have started the scheduler!!!!
    vTaskStartScheduler();

    // If I get here something is wrong!!
    Bsp_disable_irqs();
    for ( ;; )
    {
        Bsp_nop();
    }
}

bool isSchedulingEnabled() noexcept
{
    return schedulerStarted_;
}

void sleep( uint32_t milliseconds ) noexcept
{
    vTaskDelay( pdMS_TO_TICKS( milliseconds ) );
}

void sleepInRealTime( uint32_t milliseconds ) noexcept
{
    vTaskDelay( pdMS_TO_TICKS( milliseconds ) );
}


void suspendScheduling() noexcept
{
    vTaskSuspendAll();
}

void resumeScheduling() noexcept
{
    xTaskResumeAll();
}

////////////////////////////////////////////////////////////////////////////////
BSemaMutex& PrivateLocks::system() noexcept
{
    static NeverDestroyed<BSemaMutex> systemMutex_;
    return systemMutex_.get();
}


BSemaMutex& PrivateLocks::tracing() noexcept
{
    static NeverDestroyed<BSemaMutex> tracingMutex_;
    return tracingMutex_.get();
}

Mutex& PrivateLocks::systemRecursive() noexcept
{
    static NeverDestroyed<Mutex> systemRecursiveMutex_;
    return systemRecursiveMutex_.get();
}

Mutex& PrivateLocks::tracingOutput() noexcept
{
    static NeverDestroyed<Mutex> tracingOutputMutex_;
    return tracingOutputMutex_.get();
}

}  // end namespace
}
//------------------------------------------------------------------------------