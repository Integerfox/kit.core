/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */
#if 0
#include "Kit/Bsp/Api.h"
#include "Kit/Bsp/ST/STM32-F413/NUCLEO-F413ZH/console/Output.h"


// Map printf to the UART3
#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

extern "C" PUTCHAR_PROTOTYPE;

PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART3 and Loop until the end of transmission */
    
    //HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFFFFFF);

    char byte = ch & 0xFF;
    g_bspConsoleStream.write( byte );

    return ch;
}
#endif