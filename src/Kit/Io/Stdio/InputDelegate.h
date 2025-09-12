#ifndef KIT_IO_STDIO_INPUT_DELEGATE_H_
#define KIT_IO_STDIO_INPUT_DELEGATE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/IInput.h"



///
namespace Kit {
///
namespace Io {
///
namespace Stdio {


/** This concrete class implements a Input stream using the underlying
    platform's native OS interface for the C library's stdin streams. This class
    is intended to be a helper class and/or implementation inheritance parent
    class than an class used directly by the Application.
 */
class InputDelegate : public Kit::Io::IInput
{
public:
    /** Constructor. 'fd' is a the 'file/stream descriptor' of a existing/opened
        stream.
     */
    InputDelegate( KitIoStdioHandle_T fd );

public:
    /// Pull in overloaded methods from base class
    using Kit::Io::IInput::read;

    /// See Kit::Io::IInput
    bool read( void* buffer, int numBytes, int& bytesRead ) noexcept override;

    /// See Kit::Io::IInput
    bool available() noexcept override;

    /// See Kit::Io::IEos
    bool isEos() const noexcept override;

    /// See Kit::Io::IClose
    void close() noexcept override;

protected:
    /// Stream Handle
    KitIoStdioHandle_T m_inFd;

    /// Cache end-of-stream status
    bool m_inEos;
};


};      // end namespaces
};
};
#endif  // end header latch
