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
#include "Output.h"

static uint8_t                             txFifoMemory_[OPTION_BSP_CONSOLE_TX_FIFO_SIZE + 1];
static Cpl::Container::RingBuffer<uint8_t> txFifo_( sizeof( txFifoMemory_ ), txFifoMemory_ );

static uint8_t                             rxFifoMemory_[OPTION_BSP_CONSOLE_RX_FIFO_SIZE + 1];
static Cpl::Container::RingBuffer<uint8_t> rxFifo_( sizeof( rxFifoMemory_ ), rxFifoMemory_ );

Kit::Io::Serial::ST::M32F4::InputOutput     g_bspConsoleStream( txFifo_, rxFifo_ );

#endif