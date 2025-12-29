/*-----------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file Project/build specific 'Mappings'  

          Note: Intentionally there is NO Header latch (see LHeader Pattern)
                for why.
*/

// Bare-Metal OSAL mappings
#include "Kit/System/BareMetal/mappings.h"

// User Space BSP
#include "Kit/Bsp/UserSpace/Api.h"

// strapi mapping
#include "Kit/Text/_mappings/_posix/strapi.h"

// IO mapping
#include "Kit/Io/_mappings/_posix/mappings.h"
