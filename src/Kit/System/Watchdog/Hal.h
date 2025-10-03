#ifndef Kit_System_Watchdog_Hal_h_
#define Kit_System_Watchdog_Hal_h_
/*-----------------------------------------------------------------------------
* This file is part of the KIT C++ Class Library.  The KIT C++ Class Library
* is an open source project with a BSD type of licensing agreement.  See the 
* license agreement (license.txt) in the top/ directory or on the Internet at
* https://integerfox.com/kit/license.txt
*
* Copyright (c) 2025  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

// Use extern "C" linkage to accommodate implementation in C source files
#ifdef __cplusplus
extern "C" {
#endif

/** Hardware Abstraction Layer (HAL) for watchdog functionality.
    This interface provides platform-specific implementations for
    watchdog hardware control.
 */

/** Enables the watchdog hardware.
    
    @return 1 if watchdog was successfully enabled, 0 otherwise
 */
int kit_system_watchdog_hal_enableWdog(void);

/** Kicks (resets/feeds) the watchdog to prevent it from timing out.
    This should be called periodically when all monitored threads
    are healthy.
 */
void kit_system_watchdog_hal_kickWdog(void);

/** Trips the watchdog to force a system reset. This is called
    when a monitored thread has failed.
 */
void kit_system_watchdog_hal_tripWdog(void);

#ifdef __cplusplus
}

///
namespace Kit {
///
namespace System {
///
namespace Watchdog {

/** C++ wrapper functions for the HAL interface.
    These provide type-safe access to the HAL functions.
 */

/** Enables the watchdog hardware.
    
    @return true if watchdog was successfully enabled, false otherwise
 */
inline bool enableWdog() noexcept
{
    return kit_system_watchdog_hal_enableWdog() != 0;
}

/** Kicks (resets/feeds) the watchdog to prevent it from timing out.
    This should be called periodically when all monitored threads
    are healthy.
 */
inline void kickWdog() noexcept
{
    kit_system_watchdog_hal_kickWdog();
}

/** Trips the watchdog to force a system reset. This is called
    when a monitored thread has failed.
 */
inline void tripWdog() noexcept
{
    kit_system_watchdog_hal_tripWdog();
}

};      // end namespace Watchdog
};      // end namespace System
};      // end namespace Kit
#endif

#endif  // end header latch