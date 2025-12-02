#ifndef KIT_TIME_ABSOLUTETIME_H_
#define KIT_TIME_ABSOLUTETIME_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include <stdint.h>


///
namespace Kit {
///
namespace Time {

/// This method returns absolute UTC time as seconds since the EPOCH (Jan 1, 1970).
uint64_t getAbsoluteTimeSeconds() noexcept;

/** This method returns absolute UTC time as milliseconds since the EPOCH (Jan 1,
    1970). If the platform does not support millisecond resolution, the returned
    value will be: seconds X 1000.
 */
uint64_t getAbsoluteTimeMilliseconds() noexcept;

}  // end namespaces
}
#endif  // end header latch