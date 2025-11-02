#ifndef BSP_NUCLEO_F413ZH_CONSOLE_OUTPUT_H_
#define BSP_NUCLEO_F413ZH_CONSOLE_OUTPUT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

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



/// Expose the Console stream
extern Kit::Io::Serial::ST::M32F4::InputOutput   g_bspConsoleStream;


#endif  // end header latch
