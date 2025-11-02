/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Output.h"
#include "Kit/Container/RingBufferAllocate.h"
#include "Kit/System/Trace.h"

static Kit::Container::RingBufferAllocate<uint8_t, OPTION_BSP_CONSOLE_TX_FIFO_SIZE+1> txFifo_;
static Kit::Container::RingBufferAllocate<uint8_t, OPTION_BSP_CONSOLE_RX_FIFO_SIZE+1> rxFifo_;

Kit::Io::Serial::ST::M32F4::InputOutput     g_bspConsoleStream( txFifo_, rxFifo_ );

// Have trace and the console share the same stream
Kit::Io::IOutput* Kit::System::Trace::getDefaultOutputStream_( void ) noexcept
{
    return &g_bspConsoleStream;
}
