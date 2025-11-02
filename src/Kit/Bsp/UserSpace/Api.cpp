/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Bsp/Api.h"
#include "Cpl/System/Mutex.h"
#include "Cpl/System/Private_.h"     // Slight cheat here.  I use the 'System' mutex to better emulate the semantics of disable/enable interrupts



///////////////////////////////////////////////////////////
void Bsp_Api_initialize( void )
{
}


void Bsp_Api_disableIrqs_MAP( void )
{
    Cpl::System::Locks_::system().lock();
}


void Bsp_Api_enableIrqs_MAP( void )
{
    Cpl::System::Locks_::system().unlock();
}


void Bsp_Api_pushAndDisableIrqs_MAP( void )
{
    Cpl::System::Locks_::system().lock();
}


void Bsp_Api_popIrqs_MAP( void )
{
    Cpl::System::Locks_::system().unlock();
}


void Bsp_Api_nop_MAP( void )
{
}


