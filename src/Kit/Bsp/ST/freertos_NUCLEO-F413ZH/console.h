#ifndef BSP_NUCLEO_F413ZH_CONSOLE_H_
#define BSP_NUCLEO_F413ZH_CONSOLE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    By default the BSP provides a KIT IO Stream for application' console.  The
    application can disabled the IO Stream by defining the macro
    USE_BSP_USE_PRINTF.  When the macro is used the C's library's printf()
    output will be send to the UART directly and the KIT IO stream (g_bspConsoleStream)
    will not be available.

    NOTE: When USE_BSP_USE_PRINTF, the C library buffers the printf output, i.e.
          output is not sent until a newline is encountered the output stream

    The underlying KIT IO Stream is indirectly exposed/available in the global
    namespace as g_bspConsoleStream.  To get access, you code must include the
    following statement:
    \code
        extern Kit::Io::IInputOutput& g_bspConsoleStream;
    \endcode

    NOTE: Exposing the console stream avoids the BSP header files have a C++ code
          in them and prevents circular dependencies issues.
 */

#include "kit_config.h"
#include "Kit/Bsp/Api.h"

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
void Bsp_startConsoleStream( IRQn_Type           uartIrqNum,
                             UART_HandleTypeDef* uartHdlToUse ) noexcept;


/// Get the console stream's RX error counts and optionally clear the counts
size_t Bsp_getConsoleStreamErrorCounts( bool clearCounts );

#endif  // end header latch
