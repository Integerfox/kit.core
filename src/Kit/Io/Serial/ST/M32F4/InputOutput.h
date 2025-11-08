#ifndef KIT_IO_SERIAL_ST_M32F4_INPUTOUTPUT_H_
#define KIT_IO_SERIAL_ST_M32F4_INPUTOUTPUT_H_
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
#include "Kit/Io/Serial/ST/M32F4/StreamDriver.h"

///
namespace Kit {
///
namespace Io {
///
namespace Serial {
///
namespace ST {
///
namespace M32F4 {


/** This concrete class implements the InputOutput stream interface
    built on top of ST HAL Layer/SDK.

    The implementation ASSUMES that at most there is only ONE client attempt to
    use the InputOutput stream at any given time.  It is okay to call the
    read(), write(), etc.from  different threads - but the calls CANNOT be
    concurrent.  It is the application's responsibility to provide additional
    thread-safety/concurrence protection.

    Note: The 'receiver' and the 'transmitter' methods MAY be called concurrently.
 */

class InputOutput : public Kit::Io::IInputOutput
{
public:
    /// Constructor
    InputOutput( Kit::Container::RingBuffer<uint8_t>& txBuffer,
                 Kit::Container::RingBuffer<uint8_t>& rxBuffer );

    /// Destructor
    ~InputOutput( void );


public:
    /** This method MUST be used to finish the initialization of the stream
        and start is underlying drivers.  The application is responsible
        for fully initializing 'uartHdlToUse' BEFORE calling this method.

        NOTE: The stream CAN be restarted (with a possibly different UART handle)
              if close() has been called.
     */
    void start( IRQn_Type           uartIrqNum,
                UART_HandleTypeDef* uartHdlToUse ) noexcept;


public:
    /// Pull in overloaded methods from base class
    using Kit::Io::IInputOutput::read;

    /// See Kit::Io::Input
    bool read( void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept override;

    /// See Kit::Io::Input
    bool available() noexcept override;

    /// This method is used to retrieve and optionally clear the RX Error counter
    size_t getRxErrorsCounts( bool clearCount = true ) noexcept;

public:
    /// Pull in overloaded methods from base class
    using Kit::Io::IInputOutput::write;

    /// See Kit::Io::Output
    bool write( const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept override;

    /// See Kit::Io::Output
    void flush() noexcept override;

    /// See Kit::Io::IsEos. Note: This method always returns false
    bool isEos() noexcept override;

    /// See Kit::Io::Output
    void close() noexcept override;

protected:
    /// Driver
    StreamDriver m_driver;
};

}  // end namespaces
}
}
}
}
#endif  // end header latch
