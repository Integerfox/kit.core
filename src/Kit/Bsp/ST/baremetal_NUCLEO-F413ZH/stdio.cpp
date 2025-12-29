/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Api.h"
#include "stdio.h"
#include "Kit/Container/RingBufferAllocate.h"
#include "Kit/System/Trace.h"

#ifndef USE_BSP_USE_PRINTF
static Kit::Container::RingBufferAllocate<uint8_t, OPTION_BSP_CONSOLE_TX_FIFO_SIZE+1> txFifo_;
static Kit::Container::RingBufferAllocate<uint8_t, OPTION_BSP_CONSOLE_RX_FIFO_SIZE+1> rxFifo_;

Kit::Io::Serial::ST::M32F4::InputOutput     g_bspConsoleStream( txFifo_, rxFifo_ );

// Have trace and the console share the same stream
Kit::Io::IOutput* Kit::System::Trace::getDefaultOutputStream_( void ) noexcept
{
    return &g_bspConsoleStream;
}


// PRINTF SUPPORT 
#else

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
    
    HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFFFFFF);

    // char byte = ch & 0xFF;
    // g_bspConsoleStream.write( byte );

    return ch;
}
#endif