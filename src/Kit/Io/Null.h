#ifndef KIT_IO_NULL_H_
#define KIT_IO_NULL_H_
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

///
namespace Kit {
///
namespace Io {


/** This concrete class implements a NULL InputOutput stream - that all of its
    input operations return 'End-of-Stream' (i.e. return false) and all output
    is dropped in the proverbial 'bit-bucket' and goes no where!  Note: The
    write() methods always return true (i.e. no error).
 */
class Null : public IInputOutput
{
public:
    /// Constructor
    Null() noexcept;

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

public:
    /// See Kit::Io::IEos
    bool isEos()  noexcept override;

    /// See Kit::Io::IClose
    void close() noexcept override;

protected:
    /// Track my opened/closed state
    bool m_opened;
};

}  // end namespaces
}
#endif  // end header latch
