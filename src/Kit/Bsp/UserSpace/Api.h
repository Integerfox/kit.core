#ifndef Bsp_UserSpace_Api_h_
#define Bsp_UserSpace_Api_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This BSP is a null/empty BSP to be used when compiling applications that
    are dependent on "Bsp/Api.h", by the compile target is a desktop OS running
    in user space.


    DO NOT include this file directly! Instead include the generic BSP
    interface - src/Bsp/Api.h - and then configure your project's
    'colony_map.h' to include THIS file.

*----------------------------------------------------------------------------*/


// Start C++
#ifdef __cplusplus
extern "C"
{
#endif


    /*---------------------------------------------------------------------------*/

    /// Generic API - map to a single/global mutex to simulate EI/DI
    void Bsp_disable_irqs_MAP( void );

    /// Generic API - map to a single/global mutex to simulate EI/DI
    void Bsp_enable_irqs_MAP( void );

    /// Generic API - map to a single/global mutex to simulate EI/DI
    void Bsp_push_and_disable_irqs_MAP( void );

    /// Generic API - map to a single/global mutex to simulate EI/DI
    void Bsp_pop_irqs_MAP( void );

    /// Generic API -->use a empty function to simulate a nop
    void Bsp_nop_MAP( void );


/// Generic API -->do nothing
#define Bsp_turn_on_debug1_MAP()

/// Generic API -->do nothing
#define Bsp_turn_off_debug1_MAP()

/// Generic API -->do nothing
#define Bsp_toggle_debug1_MAP()


/// Generic API -->do nothing
#define Bsp_turn_on_debug2_MAP()

/// Generic API -->do nothing
#define Bsp_turn_off_debug2_MAP()

/// Generic API -->do nothing
#define Bsp_toggle_debug2_MAP()


// End C++
#ifdef __cplusplus
};
#endif


#endif
