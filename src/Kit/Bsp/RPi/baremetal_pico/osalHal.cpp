/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file implements the Kit::System::BareMetal HAL services for the
    BSP
 */

#include "Kit/System/BareMetal/Hal.h"
#include "pico/time.h"

namespace Kit {
namespace System {
namespace BareMetal {


void initialize( void ) noexcept
{
    // Nothing needed (all done by Bsp_initialize())
}

void busyWait( uint32_t waitTimeMs ) noexcept
{
    sleep_ms( waitTimeMs );
}

uint32_t getElapsedTimeMs( void ) noexcept
{
    return  (uint32_t)( to_ms_since_boot( get_absolute_time() ) );
}

}  // end namespaces
}
}
