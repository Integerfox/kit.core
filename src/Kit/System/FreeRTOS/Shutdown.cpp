/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/System/Shutdown.h"
#include "Kit/Bsp/Api.h"
#include "FreeRTOS.h"
#include "task.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace System {

////////////////////////////////////////////////////////////////////////////////
static int shutdown_application_( int exit_code )
{
    // Check if the scheduler has even started (or is still running)
    if ( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING )
    {
        vTaskEndScheduler(); // Note: This only works a very few platforms (at this time only x86 Real Mode PC)!
    }

    // If the scheduler is not running (typical case is it hasn't been started) -->then lock up in forever loop
    else
    {
        Bsp_disable_irqs();
        for ( ;;);
    }

    return exit_code;
}

int Shutdown::success() noexcept
{
    return shutdown_application_( notifyShutdownHandlers( Shutdown::eSUCCESS ) );
}

int Shutdown::failure( int exit_code ) noexcept
{
    return shutdown_application_( notifyShutdownHandlers( exit_code ) );
}

} // end namespace
}
//------------------------------------------------------------------------------