#ifndef BSP_BAREMETAL_NUCLEO_F413ZH_STDIO_H_
#define BSP_BAREMETAL_NUCLEO_F413ZH_STDIO_H_
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

    NOTE: Since the BSP is for a bare-metal platform, there are no threads,
          which means blocking-wait semantics become BUSY-WAIT semantics.
          Use the console IO stream with care! Always call available() before
          reading data.
*/

#include "kit_config.h"
#include "Kit/Io/Serial/ST/M32F4/InputOutput.h"

/** Default size of the software TX FIFO used by the console UART Stream
 */
#ifndef OPTION_BSP_CONSOLE_TX_FIFO_SIZE
#define OPTION_BSP_CONSOLE_TX_FIFO_SIZE     1024
#endif

 /** Default size of the software RX FIFO used by the console UART Stream
  */
#ifndef OPTION_BSP_CONSOLE_RX_FIFO_SIZE
#define OPTION_BSP_CONSOLE_RX_FIFO_SIZE     1024
#endif



#ifndef USE_BSP_USE_PRINTF
/// Expose the Console stream
extern Kit::Io::Serial::ST::M32F4::InputOutput   g_bspConsoleStream;
#endif

#endif  // end header latch
