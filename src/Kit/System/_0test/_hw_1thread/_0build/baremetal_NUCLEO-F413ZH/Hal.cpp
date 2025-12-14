/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file implements the Bare-metal on top of Desktop OS (Linux, Window, etc.)
    that support the ANSI C standard library.  Typically the use case for this is
    unit tests or a functional simulator where the targeted HW platform is a
    bare-metal platform.
 */

#include "Kit/System/BareMetal/Hal.h"
#include <time.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace System {
namespace BareMetal {


void initialize( void ) noexcept
{
    // Nothing needed
}

void busyWait( uint32_t waitTimeMs ) noexcept
{
    // // NOTE: busy-wait loops can cause high CPU usage
    // uint32_t startTime = getElapsedTimeMs();
    // while ( ( getElapsedTimeMs() - startTime ) < waitTimeMs )
    // {
    //     // Do nothing
    // }
}

uint32_t getElapsedTimeMs( void ) noexcept
{
    return 0;
    //return (uint32_t)( clock() * 1000 / CLOCKS_PER_SEC );
}

}  // end namespace
}
}
