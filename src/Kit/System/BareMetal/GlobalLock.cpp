/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Bsp/Api.h"
#include "Kit/System/GlobalLock.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

//////////////////////////////////////////////////////////////////////////////
void GlobalLock::begin()
{
    Bsp_disable_irqs();
}

void GlobalLock::end()
{
    Bsp_enable_irqs();
}

} // end namespace
}
//------------------------------------------------------------------------------