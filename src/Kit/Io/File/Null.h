#ifndef KIT_IO_FILE_NULL_H_
#define KIT_IO_FILE_NULL_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/File/IInputOutput.h"


///
namespace Kit {
///
namespace Io {
///
namespace File {


/** This concrete class implements a NULL IInputOutput File that all of its
    input operations return 'End-of-File' (i.e. return false) and all output is
    dropped in the proverbial bit-bucket and goes no where!  Note: The write()
    methods always return true (i.e. no error).
 */
class Null : public IInputOutput
{
public:
    /// Constructor
    Null();

public:
    /// Pull in overloaded methods a parent class
    using Kit::Io::IInput::read;

    /// See Kit::Io::IInput
    bool read( void* buffer, Kit::Type::SSize_T numBytes, Kit::Type::SSize_T& bytesRead ) noexcept override;

    /// See Kit::Io::IInput
    bool available() noexcept override;


public:
    /// Pull in overloaded methods from a parent class
    using Kit::Io::IOutput::write;

    /// See Kit::Io::Output
    bool write( const void* buffer, Kit::Type::SSize_T maxBytes, Kit::Type::SSize_T& bytesWritten ) noexcept override;

    /// See Kit::Io::File::IOutput
    void flush() noexcept override;

    /// See Kit::Io::IsEos (is equivalent to isEof())
    bool isEos() noexcept override;
    
    /// See Kit::Io::Close. Note: Once closed() has been called, all of the IOutput and IPosition methods will return false
    void close() noexcept override;


public:
    /// See Kit::Io::File::IPosition
    bool isEof() noexcept override;

    /// See Kit::Io::File::IPosition
    bool length( Kit::Type::SSize_T& len ) noexcept override;

    /// See Kit::Io::File::IPosition
    bool currentPos( Kit::Type::SSize_T& curPos ) noexcept override;

    /// See Kit::Io::File::IPosition
    bool setRelativePos( Kit::Type::SSize_T deltaOffset ) noexcept override;

    /// See Kit::Io::File::IPosition
    bool setAbsolutePos( Kit::Type::SSize_T newoffset ) noexcept override;

    /// See Kit::Io::File::IPosition
    bool setToEof() noexcept override;

protected:
    /// Track my opened/closed state
    bool m_opened;
};

}       // end namespaces
}
}
#endif  // end header latch
