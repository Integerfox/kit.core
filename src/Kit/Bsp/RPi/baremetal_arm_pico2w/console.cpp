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
#ifdef USE_BSP_KIT_IO_STREAM    // Only has meaning when USE_BSP_KIT_IO_STREAM is defined

#include "Kit/Io/IInputOutput.h"
#include "Kit/Io/Serial/RPi/RP2xxx/Uart/InputOutput.h"
#include "Kit/Container/RingBufferAllocate.h"
#include "Kit/System/Trace.h"

static Kit::Container::RingBufferAllocate<uint8_t, OPTION_BSP_CONSOLE_TX_FIFO_SIZE + 1> txFifo_;
static Kit::Container::RingBufferAllocate<uint8_t, OPTION_BSP_CONSOLE_RX_FIFO_SIZE + 1> rxFifo_;

static Kit::Io::Serial::RPi::RP2xxx::Uart::InputOutput consolefd_( txFifo_, rxFifo_, BSP_DEFAULT_UART_HANDLE );

// Global console stream object
Kit::Io::IInputOutput& g_bspConsoleStream = consolefd_;
    
void Bsp_startConsoleStream( unsigned long baudRate,
                             unsigned      txPin,
                             unsigned      rxPin,
                             unsigned      dataBits,
                             unsigned      stopBits,
                             uart_parity_t parity ) noexcept
{
    consolefd_.start( baudRate, txPin, rxPin, dataBits, stopBits, parity );
}

// Have trace and the console share the same stream
Kit::Io::IOutput* Kit::System::Trace::getDefaultOutputStream_( void ) noexcept
{
    return &consolefd_;
}

#endif  // USE_BSP_KIT_IO_STREAM