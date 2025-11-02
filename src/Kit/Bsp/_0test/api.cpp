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
#include <stdio.h>


/// Stub
#define Bsp_nop_MAP()

/// Stub
#define Bsp_disable_irqs_MAP()

/// Stub
#define Bsp_enable_irqs_MAP()

/// Stub
#define Bsp_push_and_disable_irqs_MAP()

/// Stub
#define Bsp_pop_irqs_MAP()

/// Stub
#define Bsp_turn_on_debug1_MAP()

/// Stub
#define Bsp_turn_off_debug1_MAP()

/// Stub
#define Bsp_toggle_debug1_MAP()

/// Stub
#define Bsp_turn_on_debug2_MAP()

/// Stub
#define Bsp_turn_off_debug2_MAP()

/// Stub
#define Bsp_toggle_debug2_MAP()


/// Stub
void Bsp_initialize( void )
{
    Bsp_nop();
    Bsp_disable_irqs();
    Bsp_enable_irqs();
    Bsp_push_and_disable_irqs();
    Bsp_pop_irqs();
    Bsp_turn_on_debug1();
    Bsp_turn_off_debug1();
    Bsp_toggle_debug1();
    Bsp_turn_on_debug2();
    Bsp_turn_off_debug2();
    Bsp_toggle_debug2();
}
