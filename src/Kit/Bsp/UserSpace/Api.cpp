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
#include "Kit/System/Mutex.h"
#include "Kit/System/Private.h"  // Slight cheat here.  I use the 'System' mutex to better emulate the semantics of disable/enable interrupts


///////////////////////////////////////////////////////////
void Bsp_initialize( void )
{
}

void Bsp_disable_irqs_MAP( void )
{
    Kit::System::PrivateLocks::system().lock();
}

void Bsp_enable_irqs_MAP( void )
{
    Kit::System::PrivateLocks::system().unlock();
}

void Bsp_push_and_disable_irqs_MAP( void )
{
    Kit::System::PrivateLocks::system().lock();
}

void Bsp_pop_irqs_MAP( void )
{
    Kit::System::PrivateLocks::system().unlock();
}

void Bsp_nop_MAP( void )
{
}
