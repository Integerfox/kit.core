#ifndef KIT_IO_SERIAL_RPI_RP2XXX_UART_INPUTOUTPUT_H_
#define KIT_IO_SERIAL_RPI_RP2XXX_UART_INPUTOUTPUT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/IInputOutput.h"
#include "Kit/Bsp/Api.h"
#include "Kit/Io/Types.h"
#include "hardware/uart.h"
#include "Kit/Container/RingBuffer.h"
#include "Kit/System/Thread.h"
#include <stdint.h>

/// Handle for UART0
#define KIT_IO_SERIAL_RPI_RP2XXX_UART_HANDLE_UART0 uart0

/// Handle for UART1
#define KIT_IO_SERIAL_RPI_RP2XXX_UART_HANDLE_UART1 uart1

///
namespace Kit {
///
namespace Io {
///
namespace Serial {
///
namespace RPi {
///
namespace RP2xxx {
///
namespace Uart {


/** This concrete class implements a Input stream using Raspberry Pi UART
    interface/hardware. This implement provide a more robust stream interface
    than using 'stdio_uart' because it is interrupted driven and provides
    software TX/RX FIFOs

    Notes:
        1. The UART Interrupt service handlers execute on the core that
           executing start() method.
 */

class InputOutput : public Kit::Io::IInputOutput
{
public:
    /// Constructor.
    InputOutput( Kit::Container::RingBuffer<uint8_t>& txBuffer,                          //!< Software TX FIFO
                 Kit::Container::RingBuffer<uint8_t>& rxBuffer,                          //!< Software RX FIFO
                 uart_inst_t*                         uartHdl = BSP_DEFAULT_UART_HANDLE  //!< Which UART to use
                 ) noexcept;

    /// Destructor
    ~InputOutput( void ) noexcept;

public:
    /// This method is used to start the stream/driver
    void start( unsigned long baudRate = 115200,                   //!< Baud rate in hertz
                unsigned      txPin    = BSP_DEFAULT_UART_TX_PIN,  //!< Transmit Pin
                unsigned      rxPin    = BSP_DEFAULT_UART_RX_PIN,  //!< Receive Pin
                unsigned      dataBits = 8,                        //!< Number of data bits.  Range is [5..8]
                unsigned      stopBits = 1,                        //!< Number of stop bits.  Range is [1..2]
                uart_parity_t parity   = UART_PARITY_NONE          //!< Parity setting.  See hardware/uart.h for enumeration
                ) noexcept;

    /// This method is used to stop the stream/driver. The stream can be restarted after is has been stopped
    void stop() noexcept;

public:
    /// Pull in overloaded methods from base class
    using Kit::Io::IInputOutput::read;

    /// See Kit::Io::IInput
    bool read( void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept override;

    /// See Kit::Io::IInput
    bool available() noexcept override;


public:
    /// Pull in overloaded methods from base class
    using Kit::Io::IInputOutput::write;

    /// See Kit::Io::IOutput
    bool write( const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept override;

    /// See Kit::Io::IOutput
    void flush() noexcept override;

    /// See Kit::Io::IEos. Note: This method always returns false
    bool isEos() noexcept override;

    /// See Kit::Io::IOutput
    void close() noexcept override;

protected:
    /// ISR handler for UART0
    static void su_uart0IrqHandler( void );

    /// ISR handler for UART0
    static void su_uart1IrqHandler( void );

    /// Common ISR handler (does most of the ISR work)
    void su_irqHandler();

protected:
    /// Software TX FIFO
    Kit::Container::RingBuffer<uint8_t>& m_txFifo;

    /// Software RX FIFO
    Kit::Container::RingBuffer<uint8_t>& m_rxFifo;

    /// UART Handle
    uart_inst_t* m_uartHdl;

    /// Pointer to UART0 instance (if there is one)
    static InputOutput* m_uart0Instance;

    /// Pointer to UART1 instance (if there is one)
    static InputOutput* m_uart1Instance;

    /// Critical section for ISR/thread/Core exclusion
    critical_section_t* m_lock;

    /// Handle of the blocked TX client thread (if there is one)
    Kit::System::Thread* volatile m_txWaiterPtr;

    /// Handle of the blocked RX client thread (if there is one)
    Kit::System::Thread* volatile m_rxWaiterPtr;

    /// My started/stopped state
    bool m_started;
};

}  // end namespaces
}
}
}
}
}
#endif  // end header latch
