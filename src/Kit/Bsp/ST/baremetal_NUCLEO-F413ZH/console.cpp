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
#include "console.h"
#include "Kit/Container/RingBufferAllocate.h"
#include "Kit/Io/Serial/ST/M32F4/InputOutput.h"
#include "Kit/System/Trace.h"

#ifndef USE_BSP_USE_PRINTF
static Kit::Container::RingBufferAllocate<uint8_t, OPTION_BSP_CONSOLE_TX_FIFO_SIZE + 1> txFifo_;
static Kit::Container::RingBufferAllocate<uint8_t, OPTION_BSP_CONSOLE_RX_FIFO_SIZE + 1> rxFifo_;

static Kit::Io::Serial::ST::M32F4::InputOutput console_( txFifo_, rxFifo_ );
Kit::Io::IInputOutput&                         g_bspConsoleStream = console_;

void Bsp_startConsoleStream( IRQn_Type           uartIrqNum,
                             UART_HandleTypeDef* uartHdlToUse ) noexcept
{
    console_.start( uartIrqNum, uartHdlToUse );
}

size_t Bsp_getConsoleStreamErrorCounts( bool clearCounts )
{
    return console_.getRxErrorsCounts( clearCounts );
}

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
#define PUTCHAR_PROTOTYPE int __io_putchar( int ch )
#else
#define PUTCHAR_PROTOTYPE int fputc( int ch, FILE* f )
#endif /* __GNUC__ */

extern "C" PUTCHAR_PROTOTYPE;

PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART3 and Loop until the end of transmission */

    HAL_UART_Transmit( &huart3, (uint8_t*)&ch, 1, 0xFFFFFFFF );

    // char byte = ch & 0xFF;
    // g_bspConsoleStream.write( byte );

    return ch;
}
#endif