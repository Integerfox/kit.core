#ifndef BSP_BAREMETAL_ARM_PICO2W_CONSOLE_H_
#define BSP_BAREMETAL_ARM_PICO2W_CONSOLE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    By default the BSP does NOT provide a KIT IO Stream for application' console.
    The application can enabled the IO Stream by defining the macro
    USE_BSP_KIT_IO_STREAM.  When the macro is NOT used the C's library's printf()
    output will be send to the UART directly and the KIT IO stream (g_bspConsoleStream)
    will not be available.

    NOTE: When USE_BSP_KIT_IO_STREAM is NOT defined, the C library buffers the printf
          output, i.e. output is not sent until a newline is encountered the output stream

    NOTE: Since the BSP is for a bare-metal platform, there are no threads,
          which means blocking-wait semantics become BUSY-WAIT semantics.
          Use the console IO stream with care! Always call available() before
          reading data.

    The underlying KIT IO Stream is indirectly exposed/available in the global
    namespace as g_bspConsoleStream.  To get access, you code must include the
    following statement:
    \code
        extern Kit::Io::IInputOutput& g_bspConsoleStream;
    \endcode

    NOTE: Exposing the console stream avoids the BSP header files have a C++ code
          in them and prevents circular dependencies issues.
*/

// Only has meaning when USE_BSP_KIT_IO_STREAM is defined
#ifdef USE_BSP_KIT_IO_STREAM
#include "kit_config.h"
#include "hardware/uart.h"

/** Default size of the software TX FIFO used by the console UART Stream
 */
#ifndef OPTION_BSP_CONSOLE_TX_FIFO_SIZE
#define OPTION_BSP_CONSOLE_TX_FIFO_SIZE 1024
#endif

/** Default size of the software RX FIFO used by the console UART Stream
 */
#ifndef OPTION_BSP_CONSOLE_RX_FIFO_SIZE
#define OPTION_BSP_CONSOLE_RX_FIFO_SIZE 1024
#endif


/// Starts the console stream (must be called before using the console stream)
void Bsp_startConsoleStream( unsigned long baudRate = 115200,                    //!< Baud rate in hertz
                             unsigned      txPin    = PICO_DEFAULT_UART_TX_PIN,  //!< Transmit Pin
                             unsigned      rxPin    = PICO_DEFAULT_UART_RX_PIN,  //!< Receive Pin
                             unsigned      dataBits = 8,                         //!< Number of data bits.  Range is [5..8]
                             unsigned      stopBits = 1,                         //!< Number of stop bits.  Range is [1..2]
                             uart_parity_t parity   = UART_PARITY_NONE           //!< Parity setting.  See hardware/uart.h for enumeration
                             ) noexcept;
                             
/*----------------------------------------------------------------------------*/
#endif  // USE_BSP_KIT_IO_STREAM
#endif  // end header latch
