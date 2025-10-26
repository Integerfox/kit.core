/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "colony_map.h"
#include "Cpl/System/GlobalLock.h"



//////////////////////////////////////////////////////////////////////////////
void Cpl::System::GlobalLock::begin( void )
{
     taskDISABLE_INTERRUPTS();
}

void Cpl::System::GlobalLock::end( void )
{
     taskENABLE_INTERRUPTS();

}