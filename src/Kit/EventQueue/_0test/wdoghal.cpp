/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/Watchdog/Hal.h"
#include "Kit/System/Trace.h"

#define SECT_ "_0test"

//------------------------------------------------------------------------------
// HAL Stub Implementation for Testing
//------------------------------------------------------------------------------

bool          g_watchdogEnabled = false;
unsigned long g_kickCount       = 0;
unsigned long g_tripCount       = 0;

bool Kit_System_Watchdog_hal_enable_wdog( void )
{
    KIT_SYSTEM_TRACE_MSG( SECT_, "Enabling watchdog" );
    g_watchdogEnabled = true;
    g_kickCount       = 0;
    g_tripCount       = 0;
    return true;  // Success
}

void Kit_System_Watchdog_hal_kick_wdog( void )
{
    if ( g_watchdogEnabled )
    {
        g_kickCount++;
    }
}

void Kit_System_Watchdog_hal_trip_wdog( void )
{
    g_tripCount++;
    KIT_SYSTEM_TRACE_MSG( SECT_, "TRIP WATCHDOG #%lu", g_tripCount );
}
