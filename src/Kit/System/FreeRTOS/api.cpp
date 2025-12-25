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
///
static Mutex systemMutex_;
static Mutex tracingMutex_;
static Mutex tracingOutputMutex_;
static Mutex sysList_;

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
    vTaskDelay( milliseconds * portTICK_PERIOD_MS );
}

void sleepInRealTime( uint32_t milliseconds ) noexcept
{
    vTaskDelay( milliseconds * portTICK_PERIOD_MS );
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
Mutex& PrivateLocks::system() noexcept
{
    return systemMutex_;
}


Mutex& PrivateLocks::tracing() noexcept
{
    return tracingMutex_;
}

Mutex& PrivateLocks::sysLists() noexcept
{
    return sysList_;
}

Mutex& PrivateLocks::tracingOutput() noexcept
{
    return tracingOutputMutex_;
}

}  // end namespace
}
//------------------------------------------------------------------------------