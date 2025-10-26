/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "kit_map.h"    // For FreeRTOS includes
#include "Kit/System/GlobalLock.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

//////////////////////////////////////////////////////////////////////////////
void GlobalLock::begin( void )
{
    taskDISABLE_INTERRUPTS();
}

void GlobalLock::end( void )
{
    taskENABLE_INTERRUPTS();
}

} // end namespace
}
//------------------------------------------------------------------------------