#ifndef KIT_IO_FILE_OUTPUT_H_
#define KIT_IO_FILE_OUTPUT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/File/IOutput.h"
#include "Kit/Io/Types.h"

///
namespace Kit {
///
namespace Io {
///
namespace File {


/** This concrete class provides a platform independent implementation of a
    Kit::Io::File::Output object.

    NOTE: All the write operations return 'false' if an error occurred, this
          INCLUDES the end-of-file condition (which is error when dealing
          with streams). To differentiate between a true error and EOF, the
          client must call isEof().
 */
class Output : public IOutput
{
public:
    /// Constructor -->Opens the file
    Output( const char* fileName, bool forceCreate = true, bool forceEmptyFile = false );

    /// Destructor -->Will insure the file gets closed
    ~Output();


public:
    /** This method returns true if the file was successfully open and/or
        is still opened (i.e. close() has not been called). Note: it is okay
        to call other methods in the class if the file is not open - i.e.
        nothing 'bad' will happen and the method will return 'failed'
        status (when appropriate).
     */
    bool isOpened();


public:
    /// Pull in overloaded methods from base class
    using Kit::Io::IOutput::write;

    /// See Kit::Io::IOutput
    bool write( const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept override;

    /// See Kit::Io::IOutput
    void flush() noexcept override;

    /// See Kit::Io::IEos (is equivalent to isEof())
    bool isEos() noexcept override;

    /// See Kit::Io::IClose
    void close() noexcept override;

public:
    /// See Kit::Io::File::IPosition (is equivalent to isEos())
    bool isEof() noexcept override;

    /// See Kit::Io::File::IPosition
    bool length( ByteCount_T& len ) noexcept override;

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
    KitIoFileHandle_T m_outFd;

    /// Cache end-of-stream status
    bool m_outEos;
};

}  // end namespaces
}
}
#endif  // end header latch
