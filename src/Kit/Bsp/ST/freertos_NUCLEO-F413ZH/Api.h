#ifndef BSP_STM32_FREERTOS_NUCLEO_F413ZH_API_H_
#define BSP_STM32_FREERTOS_NUCLEO_F413ZH_API_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file


    This BSP is developed/build with a 'just-in-time' approach.  This means
    as functional is added to the BSP incrementally as there is 'client'
    need for.  This BSP does not claim to provide full/complete functional
    and/or APIs for everything the board supports.


    DO NOT include this file directly! Instead include the generic BSP
    interface - src/Kit/Bsp/Api.h - and then configure your project's
    'kit_map.h' to include THIS file.

*----------------------------------------------------------------------------*/


#include "kit_config.h"
#include "Kit/Bsp/ST/freertos_NUCLEO-F413ZH/MX/Core/Inc/main.h"   // Access the PINs
#include "Kit/Bsp/ST/freertos_NUCLEO-F413ZH/MX/Core/Inc/usart.h"  // Access the UART handles/instances
#include "Kit/Bsp/ST/freertos_NUCLEO-F413ZH/MX/Core/Inc/tim.h"    // Access the PWMs/Timers
#include "Kit/Bsp/ST/freertos_NUCLEO-F413ZH/MX/Core/Inc/i2c.h"    // Access the I2C handles/instances
#include "Kit/Bsp/ST/freertos_NUCLEO-F413ZH/MX/Core/Inc/adc.h"    // Access the AIN handles/instances
#include "Kit/Bsp/ST/freertos_NUCLEO-F413ZH/MX/Core/Inc/spi.h"    // Access the SPI handles/instances
#include "Kit/Bsp/ST/freertos_NUCLEO-F413ZH/console.h"

#ifdef ENABLE_BSP_SEGGER_SYSVIEW
#include "SEGGER_SYSVIEW.h"  // Expose (to the application) the SYSVIEW APIs when enabled
#endif

//////////////////////////////////////////////////////////
/// ARM Specific APIs
//////////////////////////////////////////////////////////

/// Disable a specific interrupt (with memory barrier protection)
#define Bsp_NVIC_disable_irq( irqNum ) \
    do                                 \
    {                                  \
        HAL_NVIC_DisableIRQ( irqNum ); \
        __DSB();                       \
        __ISB();                       \
    }                                  \
    while ( 0 )

/// Enable a specific interrupt (with memory barrier protection)
#define Bsp_NVIC_enable_irq( irqNum ) \
    do                                \
    {                                 \
        HAL_NVIC_EnableIRQ( irqNum ); \
        __DSB();                      \
        __ISB();                      \
    }                                 \
    while ( 0 )

//////////////////////////////////////////////////////////
/// Generic APIs
//////////////////////////////////////////////////////////

/// Generic API
#define Bsp_nop_MAP() __asm( "nop" )

/// Generic API. This method only applies when there is actual RTOS
#define Bsp_yield_on_exit_MAP( r ) portYIELD_FROM_ISR( r )

/// Generic API
#define Bsp_disable_irqs_MAP() __disable_irq()

/// Generic API (with memory barrier protection)
#define Bsp_enable_irqs_MAP() \
    do                        \
    {                         \
        __enable_irq();       \
        __ISB();              \
    }                         \
    while ( 0 )

/// Generic API
#define Bsp_push_and_disable_irqs_MAP() Bsp_disable_irqs_MAP()  // FIXME: This really needs to PUSH the IRQ state!!!

/// Generic API
#define Bsp_pop_irqs_MAP() Bsp_enable_irqs_MAP()  // FIXME: This really needs to POP the IRQ state!!!!


/// Generic API
#define Bsp_turn_on_debug1_MAP() HAL_GPIO_WritePin( LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET )

/// Generic API
#define Bsp_turn_off_debug1_MAP() HAL_GPIO_WritePin( LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET )

/// Generic API
#define Bsp_toggle_debug1_MAP() HAL_GPIO_TogglePin( LD1_GPIO_Port, LD1_Pin )


/// Generic API
#define Bsp_turn_on_debug2_MAP() HAL_GPIO_WritePin( LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET )

/// Generic API
#define Bsp_turn_off_debug2_MAP() HAL_GPIO_WritePin( LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET )

/// Generic API
#define Bsp_toggle_debug2_MAP() HAL_GPIO_TogglePin( LD2_GPIO_Port, LD2_Pin )


#endif  // end header latch
