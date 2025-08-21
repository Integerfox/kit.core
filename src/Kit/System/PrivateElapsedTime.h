#ifndef KIT_SYSTEM_PRIVATE_ELAPSED_TIME_H_
#define KIT_SYSTEM_PRIVATE_ELAPSED_TIME_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_map.h"

/** Platform specific function returns the elapsed time in milliseconds
   
    \b Prototype:
        uint32_t KitSystemElapsedTime_getTimeInMilliseconds();
 */
#define KitSystemElapsedTime_getTimeInMilliseconds          KitSystemElapsedTime_getTimeInMilliseconds_MAP


#endif  // end header latch

