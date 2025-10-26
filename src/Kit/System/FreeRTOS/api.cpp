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

bool g_Kit_System_thread_freertos_schedulerStarted = false;

////////////////////////////////////////////////////////////////////////////////
void initialize( void )
{
    // Init the Colony.Core sub-systems
    IStartupHook::notifyStartupClients();
}


void enableScheduling( void )
{
    // This method should never return
    g_Kit_System_thread_freertos_schedulerStarted = true;            // Manually track the scheduler state since xTaskGetSchedulerState() is return 'taskSCHEDULER_RUNNING' BEFORE I have started the scheduler!!!!
    vTaskStartScheduler();

    // If I get here something is wrong!!
    Bsp_disable_irqs();
    for ( ;;)
    {
        Bsp_nop();
    }
}

bool isSchedulingEnabled( void )
{
    return g_Kit_System_thread_freertos_schedulerStarted;
}

void sleep( unsigned long milliseconds ) noexcept
{
    vTaskDelay( milliseconds * portTICK_PERIOD_MS );
}

void sleepInRealTime( unsigned long milliseconds ) noexcept
{
    vTaskDelay( milliseconds * portTICK_PERIOD_MS );
}


void suspendScheduling(void)
{
    vTaskSuspendAll();
}

void resumeScheduling(void)
{
    xTaskResumeAll();
}

////////////////////////////////////////////////////////////////////////////////
Mutex& PrivateLocks::system( void )
{
    return systemMutex_;
}


Mutex& PrivateLocks::tracing( void )
{
    return tracingMutex_;
}

Mutex& PrivateLocks::sysLists( void )
{
    return sysList_;
}

Mutex& PrivateLocks::tracingOutput( void )
{
    return tracingOutputMutex_;
}

} // end namespace
}
//------------------------------------------------------------------------------