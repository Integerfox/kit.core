#ifndef KIT_SYSTEM_WATCHDOG_HAL_H_
#define KIT_SYSTEM_WATCHDOG_HAL_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include <stdbool.h>

// Hardware Abstraction Layer (HAL) for watchdog functionality
extern "C"
{
    /** Enables the watchdog hardware.
        @return true if watchdog was successfully enabled, false otherwise
     */
    bool Kit_System_Watchdog_hal_enable_wdog( void );

    /** Kicks (resets/feeds) the watchdog to prevent it from timing out.
        This should be called periodically when all monitored threads are healthy.
     */
    void Kit_System_Watchdog_hal_kick_wdog( void );

    /** Trips the watchdog to force a system reset. This is called
        when a monitored thread has failed.
     */
    void Kit_System_Watchdog_hal_trip_wdog( void );
}

#endif  // end header latch