/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This unit test is used to verify that the 'UserSpace' BSP compiles and links
    correctly.  It does NOT perform any functional testing.
*/


#include "Kit/Bsp/Api.h"


/// Test that all BSP APIs compile and link
void bsp_test_compile_and_link( void )
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
