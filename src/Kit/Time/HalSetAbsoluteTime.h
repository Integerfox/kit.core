#ifndef KIT_TIME_HAL_SET_ABSOLUTETIME_H_
#define KIT_TIME_HAL_SET_ABSOLUTETIME_H_
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
    
/** This platform independent method is used to set the target's current
    absolute time.  The new time format is: UTC time as milliseconds since the 
    EPOCH (Jan 1, 1970).

    NOTE: Whether or not the new time stays in effect or is lost on the next boot
          cycle - is platform specific.
 */
void halSetAbsoluteTime( uint64_t utcEpochMilliseconds ) noexcept;

}  // end namespaces
}
#endif  // end header latch