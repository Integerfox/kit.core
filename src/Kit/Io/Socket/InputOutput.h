#ifndef KIT_IO_SOCKET_INPUT_OUTPUT_H_
#define KIT_IO_SOCKET_INPUT_OUTPUT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/Socket/IInputOutput.h"

///
namespace Kit {
///
namespace Io {
///
namespace Socket {


/** This concrete class provides a platform independent 'standard'
    implementation of an InputOutput stream object where the stream
    is Socket connection.
 */
class InputOutput : public IInputOutput
{
public:
    /// Constructor
    InputOutput() noexcept;

    /// Destructor
    ~InputOutput( void ) noexcept;


public:
    /// See Kit::Io::Socket::IInputOutput
    void activate( KitIoSocketHandle_T fd ) noexcept override;


public:
    /// Pull in overloaded methods from base class
    using Kit::Io::IInput::read;

    /// See Kit::Io::IInput
    bool read( void* buffer, int numBytes, int& bytesRead ) noexcept override;

    /// See Kit::Io::IInput
    bool available() noexcept override;


public:
    /// Pull in overloaded methods from base class
    using Kit::Io::IOutput::write;

    /// See Kit::Io::IOutput
    bool write( const void* buffer, int maxBytes, int& bytesWritten ) noexcept override;

    /// See Kit::Io::IOutput
    void flush() noexcept override;

    /// See Kit::Io::IEos
    bool isEos() noexcept override;

    /// See Kit::Io::IClose
    void close() noexcept override;

protected:
    /// Socket handle
    KitIoSocketHandle_T m_fd;

    /// End-of-Stream status
    bool m_eos;
};

}  // end namespaces
}
}
#endif  // end header latch
