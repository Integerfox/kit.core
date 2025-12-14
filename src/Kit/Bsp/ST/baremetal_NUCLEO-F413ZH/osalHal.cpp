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
#include "stm32f4xx_hal.h"

namespace Kit {
namespace System {
namespace BareMetal {


void initialize( void ) noexcept
{
    // Nothing needed (all done by Bsp_initialize())
}

void busyWait( uint32_t waitTimeMs ) noexcept
{
    HAL_Delay( waitTimeMs );
}

uint32_t getElapsedTimeMs( void ) noexcept
{
    return HAL_GetTick();
}

}  // end namespaces
}
}
