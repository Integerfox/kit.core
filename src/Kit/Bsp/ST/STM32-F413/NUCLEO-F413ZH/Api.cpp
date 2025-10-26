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
#include "stm32f4xx_hal.h"
#include "Kit/Bsp/ST/STM32-F413/NUCLEO-F413ZH/MX/Core/Inc/gpio.h"
//#include "Kit/Bsp/ST/STM32-F413/NUCLEO-F413ZH/console/Output.h"
//#include "Kit/System/Trace.h"

#ifdef ENABLE_BSP_SEGGER_SYSVIEW   
#define INIT_SEGGER_SYSVIEW()   SEGGER_SYSVIEW_Conf()
#else
#define INIT_SEGGER_SYSVIEW()   
#endif

#define SECT_   "bsp"

///////////////////////////////////////////
void Bsp_initialize( void )
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_TIM4_Init();
    MX_USART3_UART_Init();
    MX_I2C2_Init();
    MX_SPI3_Init();

    // Initialize System View (asap after the basic board initialization has completed)
    INIT_SEGGER_SYSVIEW();

    // Start the Console/Trace UART
//    g_bspConsoleStream.start( USART3_IRQn, &huart3 );
}

