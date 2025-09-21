#ifndef KIT_IO_FILE_INPUT_H_
#define KIT_IO_FILE_INPUT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/File/IInput.h"


///
namespace Kit {
///
namespace Io {
///
namespace File {


/** This concrete class provides a platform independent implementation of a 
    Kit::Io::File::Input object.
 */
class Input : public IInput
{
public:
    /// Constructor -->Opens the file
    Input( const char* fileName ) noexcept;

    /// Destructor -->Will insure the file gets closed
    ~Input() noexcept;

public:
    /** This method returns true if the file was successfully open and/or
        is still opened (i.e. close() has not been called). Note: it is okay
        to call other methods in the class if the file is not open - i.e.
        nothing 'bad' will happen and the method will return 'failed'
        status (when appropriate).
     */
    virtual bool isOpened() noexcept;

public:
    /// Pull in overloaded methods from a parent class
    using Kit::Io::IInput::read;

    /// See Kit::Io::IInput
    bool read( void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept override;

    /// See Kit::Io::IInput
    bool available() noexcept override;

    /// See Kit::Io::IEos (is equivalent to isEof())
    bool isEos() noexcept override;

    /// See Kit::Io::IClose
    void close() noexcept override;

public:
    /// See Kit::Io::File::IPosition (is equivalent to isEos())
    bool isEof() noexcept override;

    /// See Kit::Io::File::IPosition
    bool length( ByteCount_T& len) noexcept override;

    /// See Kit::Io::File::IPosition
    bool currentPos( ByteCount_T& curPos ) noexcept override;

    /// See Kit::Io::File::IPosition
    bool setRelativePos( ByteCount_T deltaOffset ) noexcept override;

    /// See Kit::Io::File::IPosition
    bool setAbsolutePos( ByteCount_T newoffset ) noexcept override;

    /// See Kit::Io::File::IPosition
    bool setToEof() noexcept override;

protected:
    /// File Handle
    KitIoFileHandle_T m_inFd;

    /// Cache end-of-stream status
    bool m_inEos;
};

}      // end namespaces
}
}
#endif  // end header latch

