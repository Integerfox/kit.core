/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "InputOutput.h"
#include "pico/critical_section.h"
#include <cstdint>
#include <hardware/gpio.h>
#include <hardware/irq.h>
#include <hardware/timer.h>
#include <hardware/regs/intctrl.h>
#include <pico/sync.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace Serial {
namespace RPi {
namespace RP2xxx {
namespace Uart {

InputOutput* InputOutput::m_uart0Instance;
InputOutput* InputOutput::m_uart1Instance;

static critical_section_t lockUart0_;
static critical_section_t lockUart1_;

// Fill the HW Transmit FIFO.  Assumed to be called from within a critical section
// static void su_fillHWTxFifo( uart_inst_t* uartHdl, Kit::Container::RingBuffer<uint8_t>& txBuffer ) noexcept
// {
//     while ( uart_is_writable( uartHdl ) )
//     {
//         uint8_t c;
//         if ( txBuffer.remove( c ) == false )
//         {
//             break;
//         }
//         uart_putc_raw( uartHdl, c );
//     }
// }

// Fill the HW Transmit FIFO - provides critical section protection
static void fillHWTxFifo( uart_inst_t* uartHdl, Kit::Container::RingBuffer<uint8_t>& txBuffer, critical_section_t* lock ) noexcept
{
    for ( ;; )
    {
        critical_section_enter_blocking( lock );
        bool hwFifoNotFull = uart_is_writable( uartHdl );
        if ( !hwFifoNotFull )
        {
            critical_section_exit( lock );
            return;
        }
        uint8_t c;
        bool    removed = txBuffer.remove( c );
        if ( removed == false )
        {
            critical_section_exit( lock );
            return;
        }
        uart_putc_raw( uartHdl, c );
        critical_section_exit( lock );
    }
}

// Drain the HW Receive FIFO - provides critical section protection
static void drainHWRxFifo( uart_inst_t* uartHdl, Kit::Container::RingBuffer<uint8_t>& rxBuffer, critical_section_t* lock ) noexcept
{
    for ( ;; )
    {
        critical_section_enter_blocking( lock );
        bool hwFifoNotEmpty = uart_is_readable( uartHdl );
        if ( !hwFifoNotEmpty )
        {
            critical_section_exit( lock );
            return;
        }
        uint8_t c     = uart_getc( uartHdl );
        bool    added = rxBuffer.add( c );  // If the FIFO if full, the byte will be silently discarded
        critical_section_exit( lock );
        if ( added == false )
        {
            return;
        }
    }
}

////////////////////////////////////
InputOutput::InputOutput( Kit::Container::RingBuffer<uint8_t>& txBuffer,
                          Kit::Container::RingBuffer<uint8_t>& rxBuffer,
                          uart_inst_t*                         uartHdl ) noexcept
    : m_txFifo( txBuffer )
    , m_rxFifo( rxBuffer )
    , m_uartHdl( uartHdl )
    , m_lock( nullptr )
    , m_started( false )
{
    if ( m_uartHdl == KIT_IO_SERIAL_RPI_RP2XXX_UART_HANDLE_UART0 )
    {
        m_lock = &lockUart0_;
        critical_section_init( m_lock );
    }
    else if ( m_uartHdl == KIT_IO_SERIAL_RPI_RP2XXX_UART_HANDLE_UART1 )
    {
        m_lock = &lockUart1_;
        critical_section_init( m_lock );
    }
}

InputOutput::~InputOutput( void ) noexcept
{
    stop();
}

void InputOutput::start( unsigned long baudRate,
                         unsigned      txPin,
                         unsigned      rxPin,
                         unsigned      dataBits,
                         unsigned      stopBits,
                         uart_parity_t parity ) noexcept
{
    // Ignore if already started
    if ( !m_started )
    {
        // Initialize the hardware
        m_started = true;
        m_txFifo.clearTheBuffer();
        m_rxFifo.clearTheBuffer();
        uart_init( m_uartHdl, baudRate );
        uart_set_format( m_uartHdl, dataBits, stopBits, parity );
        gpio_set_function( txPin, GPIO_FUNC_UART );
        gpio_set_function( rxPin, GPIO_FUNC_UART );
        uart_set_fifo_enabled( m_uartHdl, true );
        uart_set_hw_flow( m_uartHdl, false, false );  // Disable HW flow control

        // Set up IRQ Handlers
        if ( m_uartHdl == KIT_IO_SERIAL_RPI_RP2XXX_UART_HANDLE_UART0 )
        {
            m_uart0Instance = this;
            irq_clear( UART0_IRQ );
            irq_set_exclusive_handler( UART0_IRQ, su_uart0IrqHandler );
            irq_set_enabled( UART0_IRQ, true );
        }
        else if ( m_uartHdl == KIT_IO_SERIAL_RPI_RP2XXX_UART_HANDLE_UART1 )
        {
            m_uart1Instance = this;
            irq_clear( UART1_IRQ );
            irq_set_exclusive_handler( UART1_IRQ, su_uart1IrqHandler );
            irq_set_enabled( UART1_IRQ, true );
        }
        uart_set_irqs_enabled( m_uartHdl, true, false );
    }
}

void InputOutput::stop() noexcept
{
    // Ignore if already stopped
    if ( m_started )
    {
        // Disable the RX/TX interrupts
        m_started = false;
        uart_set_irqs_enabled( m_uartHdl, false, false );

        if ( m_uartHdl == KIT_IO_SERIAL_RPI_RP2XXX_UART_HANDLE_UART0 )
        {
            irq_set_enabled( UART0_IRQ, false );
            irq_remove_handler( UART0_IRQ, su_uart0IrqHandler );
            m_uart0Instance = 0;
        }
        else if ( m_uartHdl == KIT_IO_SERIAL_RPI_RP2XXX_UART_HANDLE_UART1 )
        {
            irq_set_enabled( UART1_IRQ, false );
            irq_remove_handler( UART1_IRQ, su_uart1IrqHandler );
            m_uart1Instance = 0;
        }

        uart_deinit( m_uartHdl );
    }
}


////////////////////////////////////
void InputOutput::su_uart0IrqHandler( void ) noexcept
{
    if ( m_uart0Instance )
    {
        m_uart0Instance->su_irqHandler();
    }
}

/// ISR handler for UART0
void InputOutput::su_uart1IrqHandler( void ) noexcept
{
    if ( m_uart1Instance )
    {
        m_uart1Instance->su_irqHandler();
    }
}

void InputOutput::su_irqHandler() noexcept
{
    // Get the IRQ flags
    uint32_t irqFlags = uart_get_hw( m_uartHdl )->mis;

    // Receive IRQ
    if ( irqFlags & ( UART_UARTMIS_RXMIS_BITS | UART_UARTMIS_RTMIS_BITS ) )
    {
        // Drain the HW Receive FIFO
        // NOTE: Need to use a critical section - because we CANNOT guaranteed that
        // the core where read/write is being called is the same core as where
        // the ISR run
        drainHWRxFifo( m_uartHdl, m_rxFifo, m_lock );
    }

    // Transmit IRQ
    if ( irqFlags & ( UART_UARTMIS_TXMIS_BITS ) )
    {
        // Fill the HW Transmit FIFO (See previous comment about why a Critical section is needed in the ISR)
        fillHWTxFifo( m_uartHdl, m_txFifo, m_lock );

        // Disable TX IRQ for now if we have nothing left to transmit
        if ( m_txFifo.isEmpty() )
        {
            uart_set_irqs_enabled( m_uartHdl, true, false );
        }
    }
}


////////////////////////////////////
bool InputOutput::available() noexcept
{
    if ( !m_started )
    {
        return false;
    }

    critical_section_enter_blocking( m_lock );
    bool avail = m_rxFifo.isEmpty() == false;
    critical_section_exit( m_lock );
    return avail;
}

bool InputOutput::read( void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept
{
    bytesRead       = 0;
    uint8_t* dstPtr = static_cast<uint8_t*>( buffer );

    // Fail if not started
    if ( !m_started )
    {
        return false;
    }

    // Ignore read of zero bytes
    if ( numBytes == 0 )
    {
        return true;
    }

    // Drain the SW RX FIFO
    while ( numBytes )
    {
        // Attempt to get the next byte from the SW RX FIFO
        critical_section_enter_blocking( m_lock );
        bool notEmpty = m_rxFifo.remove( *dstPtr );
        critical_section_exit( m_lock );
        if ( notEmpty == false )
        {
            break;
        }

        // Advance to next outgoing byte
        dstPtr++;
        numBytes--;
        bytesRead++;
    }

    // If I get here, the read operation succeeded
    return true;
}

bool InputOutput::write( const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept
{
    bytesWritten          = 0;
    const uint8_t* srcPtr = static_cast<const uint8_t*>( buffer );

    // Fail if not started
    if ( !m_started )
    {
        return false;
    }

    // Ignore write of zero bytes
    if ( maxBytes == 0 )
    {
        return true;
    }

    // Fill the SW TX FIFO
    while ( maxBytes )
    {
        // Attempt to add the next byte to SW TX FIFO
        critical_section_enter_blocking( m_lock );
        bool notFull = m_txFifo.add( *srcPtr );
        critical_section_exit( m_lock );
        if ( notFull == false )
        {
            break;
        }

        // Advance to next incoming byte
        srcPtr++;
        maxBytes--;
        bytesWritten++;
    }

    // Trigger a transmit if the Transmitter has gone had gone idle
    uart_set_irqs_enabled( m_uartHdl, true, true );
    fillHWTxFifo( m_uartHdl, m_txFifo, m_lock );

    // If I get here, the write operation succeeded
    return true;
}


void InputOutput::flush() noexcept
{
    // At best the SW TX FIFO could be 'flushed' to the HW TX FIFO, but that
    // doesn't guarantee the data was physically transmitted. So the
    // choice was made to do nothing.
}

bool InputOutput::isEos() noexcept
{
    // Does not really have meaning for serial port (assuming the serial port is opened/active)
    return false;
}

void InputOutput::close() noexcept
{
    // Does not really have meaning for serial port
    flush();
}

}  // end namespace
}
}
}
}
}
//------------------------------------------------------------------------------