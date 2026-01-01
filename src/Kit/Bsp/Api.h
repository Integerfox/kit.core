#ifndef KIT_BSP_API_H_
#define KIT_BSP_API_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file defines a common/generic Board Support Package (BSP) Interface that 
    provides an indirect reference to a concrete BSP (See the LHeader Pattern).

    Note: Many of the common interfaces are defined as C preprocessor macros - 
    this provides the maximum flexibility in implementation for a concrete BSP.  
    Only functions that 'should be' or need to be true function calls are 
    defined in this class.
 */


#include "kit_config.h"
#include "kit_map.h"        // Provides the concrete BSP instance/header files/interface mappings

// clang-format off

// Start C++
#ifdef __cplusplus
extern "C"
{
#endif

/////////////////////////////////////////////////////////////////////////////
//

/** This method will initialize the board/low level hardware such that BASIC
    board operation is possible. Additional initialization of peripherals may
    be required - see your specific BSP header file for details.

    NOTE: Since this method is intended to be called by the Application - it
            is 'run' AFTER main() has been entered.
    */
void Bsp_initialize( void );


/** This method inserts a single 'nop' instruction

    \b Prototype:
        void Bsp_nop(void);
    */
#define Bsp_nop Bsp_nop_MAP

/** RTOS helper: this method is to informs the scheduler that a context switch
    is required on exit from the ISR.  The 'result' argument is the return value
    from a call to su_signal()  

    NOTE: If you are not using an RTOS, or your RTOS does not require this
          functionality, you can define this macro as a NOP.

    \b Prototype:
        void Bsp_yield_on_exit( int result );

    */
#define Bsp_yield_on_exit   Bsp_yield_on_exit_MAP


/////////////////////////////////////////////////////////////////////////////

//
/** This method will disable ALL interrupts, i.e manipulate the global
    enable/disable flag in the PSW.

    \b Prototype:
        void Bsp_disable_irqs( void );
    */
#define Bsp_disable_irqs Bsp_disable_irqs_MAP


/** This method will enable ALL interrupts, i.e manipulate the global
    enable/disable flag in the PSW.

    \b Prototype:
        void Bsp_enable_irqs( void );
    */
#define Bsp_enable_irqs Bsp_enable_irqs_MAP


/** This method will push the current IRQ enabled/disabled flags onto
    the stack and then disable ALL interrupts. This method should all
    be called in 'pairs' with the Bsp_popIrqs() method.

    NOTE: This method MAY push the CPU's entire Program Status Word (PSW) onto
            the stack and the corresponding Bsp_popIrq() will restore the
            ENTIRE PSW word.  What does this mean - usually nothing but things
            like the zero, carry, overflow flags, etc. are stored in the PSW and
            we be reverted (when 'pop' is called) to the state of when the 'push'
            was done.

    \b Prototype:
        void Bsp_push_and_disable_irqs( void );
    */
#define Bsp_push_and_disable_irqs Bsp_push_and_disable_irqs_MAP


/** This method will pop/update the IRQ enabled/disabled flags from
    a previously push IRQ state value.  This method should all
    be called in 'pairs' with the Bsp_pushAndDisableIrqs() method.

    \b Prototype:
        void Bsp_pop_irqs( void );
    */
#define Bsp_pop_irqs Bsp_pop_irqs_MAP


/////////////////////////////////////////////////////////////////////////////
//
/** This method will turn on "Debug LED 1".  The concept is that every board
    has one or more LEDs, IO Pins, etc. that can be used for debugging purposes.

    \b Prototype:
        void Bsp_turn_on_debug1( void );
 */
#define Bsp_turn_on_debug1 Bsp_turn_on_debug1_MAP


/** This method will turn off "Debug LED 1".  The concept is that every board
    has one or more LEDs, IO Pins, etc. that can be used for debugging purposes.

    \b Prototype:
        void Bsp_turn_off_debug1( void );
 */
#define Bsp_turn_off_debug1 Bsp_turn_off_debug1_MAP

/** This method will toggle "Debug LED 1".  The concept is that every board
    has one or more LEDs, IO Pins, etc. that can be used for debugging purposes.

    \b Prototype:
        void Bsp_toggle_debug1( void );
 */
#define Bsp_toggle_debug1 Bsp_toggle_debug1_MAP


/** This method will turn on "Debug LED 2".  The concept is that every board
    has one or more LEDs, IO Pins, etc. that can be used for debugging purposes.

    \b Prototype:
        void Bsp_turn_on_debug2( void );
 */
#define Bsp_turn_on_debug2 Bsp_turn_on_debug2_MAP


/** This method will turn off "Debug LED 2".  The concept is that every board
    has one or more LEDs, IO Pins, etc. that can be used for debugging purposes.

    \b Prototype:
        void Bsp_turn_off_debug2( void );
 */
#define Bsp_turn_off_debug2 Bsp_turn_off_debug2_MAP

/** This method will toggle "Debug LED 2".  The concept is that every board
    has one or more LEDs, IO Pins, etc. that can be used for debugging purposes.

    \b Prototype:
        void Bsp_toggle_debug2( void );
 */
#define Bsp_toggle_debug2 Bsp_toggle_debug2_MAP


// End C++
#ifdef __cplusplus
};
#endif
// clang-format on

#endif  // end header latch
