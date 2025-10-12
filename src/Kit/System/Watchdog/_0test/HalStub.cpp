/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file provides stub implementations of the watchdog HAL functions
    for testing and simulation purposes.
 */

#include <stdbool.h>
#include "Kit/System/Trace.h"
#include <stdlib.h>

#define SECT_ "_0test"

// HAL function declarations
extern "C"
{
    bool Kit_System_Watchdog_hal_enable_wdog( void );
    void Kit_System_Watchdog_hal_kick_wdog( void );
    void Kit_System_Watchdog_hal_trip_wdog( void );
}

// Use extern "C" linkage to accommodate implementation in C source files
extern "C"
{

    static bool          s_watchdogEnabled = false;
    static unsigned long s_kickCount       = 0;

    bool Kit_System_Watchdog_hal_enable_wdog( void )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "WATCHDOG_HAL: Enabling watchdog (stub implementation)" );
        s_watchdogEnabled = true;
        s_kickCount       = 0;
        return true;  // Success
    }

    void Kit_System_Watchdog_hal_kick_wdog( void )
    {
        if ( s_watchdogEnabled )
        {
            s_kickCount++;
            KIT_SYSTEM_TRACE_MSG( SECT_, "WATCHDOG_HAL: Kick #%lu", s_kickCount );
        }
    }

    void Kit_System_Watchdog_hal_trip_wdog( void )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "WATCHDOG_HAL: TRIP WATCHDOG - System would reset here!" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "WATCHDOG_HAL: (In stub mode - not actually resetting)" );

        // In a real implementation, this would trigger a hardware reset
        // For testing purposes, we'll just print a warning
        KIT_SYSTEM_TRACE_MSG( SECT_, "*** WATCHDOG TRIP: System would reset now! ***" );

        // Uncomment the following line if you want the stub to actually exit
        // exit(1);
    }

}  // extern "C"