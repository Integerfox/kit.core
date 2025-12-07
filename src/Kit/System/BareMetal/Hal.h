#ifndef KIT_SYSTEM_BAREMETAL_HAL_H_
#define KIT_SYSTEM_BAREMETAL_HAL_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file defines the HAL services a platform is required to provided when
    running KIT on a bare-metal platform. 
 */

#include <stdint.h>

/// 
namespace Kit {
/// 
namespace System {
///
namespace BareMetal {


/** This method is used to initialize the HAL/Target layer.  What needs to be
    an/or is initialized is platform specific.
 */
void initialize( void ) noexcept;

/** This method performs a busy-wait for the specified number of milliseconds
 */
void busyWait( uint32_t waitTimeMs ) noexcept;

/** This method returns the current elapsed time since power-up in milliseconds.
    It is okay if the elapsed time wraps around.
 */
uint32_t getElapsedTimeMs( void ) noexcept;

}       // end namespaces
}
}

#endif  // end header latch

