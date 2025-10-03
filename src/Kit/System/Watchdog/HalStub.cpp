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
/** @file 

    This file provides stub implementations of the watchdog HAL functions
    for testing and simulation purposes.
 */

#include "Hal.h"
#include <stdio.h>
#include <stdlib.h>

// Use extern "C" linkage to accommodate implementation in C source files
extern "C" {

static bool s_watchdogEnabled = false;
static unsigned long s_kickCount = 0;

int kit_system_watchdog_hal_enableWdog(void)
{
    printf("WATCHDOG_HAL: Enabling watchdog (stub implementation)\n");
    s_watchdogEnabled = true;
    s_kickCount = 0;
    return 1; // Success
}

void kit_system_watchdog_hal_kickWdog(void)
{
    if (s_watchdogEnabled) {
        s_kickCount++;
        printf("WATCHDOG_HAL: Kick #%lu\n", s_kickCount);
    }
}

void kit_system_watchdog_hal_tripWdog(void)
{
    printf("WATCHDOG_HAL: TRIP WATCHDOG - System would reset here!\n");
    printf("WATCHDOG_HAL: (In stub mode - not actually resetting)\n");

    // In a real implementation, this would trigger a hardware reset
    // For testing purposes, we'll just print a warning
    printf("*** WATCHDOG TRIP: System would reset now! ***\n");
    fflush(stdout);

    // Uncomment the following line if you want the stub to actually exit
    // exit(1);
}

} // extern "C"