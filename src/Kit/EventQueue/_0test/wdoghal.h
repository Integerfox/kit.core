#ifndef KIT_EVENTQUEUE_0TEST_WDOGHAL_H_
#define KIT_EVENTQUEUE_0TEST_WDOGHAL_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

/// Expose mocking variables
extern bool g_watchdogEnabled;

/// Expose mocking variables
extern unsigned long g_kickCount;

/// Expose mocking variables
extern unsigned long g_tripCount;

/// Reset mocking variables
inline void resetWatchdogHalMocks()
{
    g_watchdogEnabled = false;
    g_kickCount       = 0;
    g_tripCount       = 0;
}

#endif  // end header latch