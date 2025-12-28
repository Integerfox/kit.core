#ifndef BSP_RPI_BAREMETAL_ARM_PICO2W_API_H_
#define BSP_RPI_BAREMETAL_ARM_PICO2W_API_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This BSP is for the Raspberry Pi Pico 2 W board running bare-metal code on
    the RP2350 ARM Cortex-M0+ microcontroller.

    This BSP is developed/build with a 'just-in-time' approach.  This means
    as functional is added to the BSP incrementally as there is 'client'
    need for.  This BSP does not claim to provide full/complete functional
    and/or APIs for everything the board supports.

    DO NOT include this file directly! Instead include the generic BSP
    interface - src/Kit/Bsp/Api.h - and then configure your project's
    'kit_map.h' to include THIS file.

    OPTIONAL Compile switches:

        USE_BSP_NO_STDIO    - Defining this symbol will prevent initializing
                              SDK's stdio sub-system.  This is for when the
                              application is supplying its own driver for
                              UART0
*----------------------------------------------------------------------------*/


#include "kit_config.h"
#include "pico/sync.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/sync.h"

#ifdef __cplusplus
#include "Kit/Bsp/RPi/baremetal_arm_pico2w/console.h"
#endif

//////////////////////////////////////////////////////////
/// Mappings for Generic APIs
//////////////////////////////////////////////////////////

/// Generic API
#define Bsp_nop_MAP() __asm( "nop" )

/// Generic API
#define Bsp_disable_irqs_MAP disable_interrupts

/// Generic API
#define Bsp_enable_irqs_MAP enable_interrupts

/// Generic API
#define Bsp_push_and_disable_irqs_MAP() uint32_t bspIrqStatus_unique_name = save_and_disable_interrupts()

/// Generic API
#define Bsp_pop_irqs_MAP() restore_interrupts_from_disabled( bspIrqStatus_unique_name )


/// Generic API
#define Bsp_turn_on_debug1_MAP Bsp_turnOnDebug1

/// Generic API
#define Bsp_turn_off_debug1_MAP Bsp_turnOffDebug1

/// Generic API
#define Bsp_toggle_debug1_MAP Bsp_toggleDebug1


/// Generic API
#define Bsp_turn_on_debug2_MAP()  // Not supported. There is one LED on the PICO board

/// Generic API
#define Bsp_turn_off_debug2_MAP()  // Not supported. There is one LED on the PICO board

/// Generic API
#define Bsp_toggle_debug2_MAP()  // Not supported. There is one LED on the PICO board


/// Function to operate on Debug LED1
void Bsp_turnOnDebug1();

/// Function to operate on Debug LED1
void Bsp_turnOffDebug1();

/// Function to operate on Debug LED1
void Bsp_toggleDebug1();

//////////////////////////////////////////////////////////
/// Target/Board specific Functions
//////////////////////////////////////////////////////////

/// Default UART Settings
#define BSP_DEFAULT_UART_HANDLE uart0

/// Default UART Settings
#define BSP_DEFAULT_UART_TX_PIN PICO_DEFAULT_UART_TX_PIN

/// Default UART Settings
#define BSP_DEFAULT_UART_RX_PIN PICO_DEFAULT_UART_RX_PIN

/// Depth of the UART's HW RX FIFO
#define BSP_UART_RX_FIFO_DEPTH 32

/// Depth of the UART's HW TX FIFO
#define BSP_UART_TX_FIFO_DEPTH 32


#endif  // end header latch
