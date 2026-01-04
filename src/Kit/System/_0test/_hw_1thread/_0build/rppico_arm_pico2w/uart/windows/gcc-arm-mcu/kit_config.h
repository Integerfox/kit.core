#ifndef KIT_CONFIG_H_
#define KIT_CONFIG_H_
/*-----------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file Project/build specific 'Options' (see LConfig Pattern) */


// USE a KIT stream for the BSP console (printf is not supported)
#define USE_BSP_KIT_IO_STREAM

// Because of CYW43 WiFi driver async interactions - we need to increase the time tolerance multiplier on start-up (basically turn off time checking)
#define OPTION_TEST_TIME_TOLERANCE_MULTIPLIER 10
#define USE_KIT_SYSTEM_TRACE
#define OPTION_BSP_CONSOLE_RX_FIFO_SIZE 1024
#define OPTION_BSP_CONSOLE_TX_FIFO_SIZE 1024

#endif
