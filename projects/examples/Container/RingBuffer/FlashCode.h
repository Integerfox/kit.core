#ifndef CONTAINER_RINGBUFFER_DATA_H_
#define CONTAINER_RINGBUFFER_DATA_H_
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
namespace Container {
///
namespace RingBuffer {


/** This struct defines a Flash code.  A Flash code consists of an On and Off
    LED periods that is repeated N times. A Flash code is executed by
    running the cycle defined by the ON period followed by the OFF period N times.
 */
struct FlashCode_T
{
    uint32_t onPeriodMs;   //!< The time - in milliseconds - of ON period of the flash code
    uint32_t offPeriodMs;  //!< The time - in milliseconds - of OFF period of the flash code
    unsigned numFlashes;   //!< The number of times to repeat the ON/OFF sequence
};

}  // end namespaces
}
#endif  // end header latch
