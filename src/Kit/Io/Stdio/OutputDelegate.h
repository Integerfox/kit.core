#ifndef KIT_IO_STDIO_OUTPUT_DELEGATE_H_
#define KIT_IO_STDIO_OUTPUT_DELEGATE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_map.h"
#include "Kit/Io/IOutput.h"



///
namespace Kit {
///
namespace Io {
///
namespace Stdio {

/** This concrete class implements a Output stream using the underlying platform's
    native OS interface' sfor the C library's stdout and stderr streams.

    Design Note: It is important that this class be concrete class that can
    be created by application code that is target/platform independent.  This is
    why the data type for 'm_outFd' member variable is defined using the
    LHeader pattern.
 */
class OutputDelegate : public Kit::Io::IOutput
{
public:
    /** Constructor. 'fd' is a the 'file/stream descriptor' of a existing/opened
        stream.
     */
    OutputDelegate( KitIoStdioHandle_T fd ) noexcept;

public:
    /// Pull in overloaded methods from base class
    using Kit::Io::IOutput::write;

    /// See Cpl::Io::IOutput
    bool write( const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept override;

    /// See Cpl::Io::Output
    void flush() noexcept override;

    /// See Cpl::Io::IEos
    bool isEos()  noexcept override;

    /// See Cpl::Io::IClose
    void close() noexcept override;

protected:
    /// Stream Handle
    KitIoStdioHandle_T m_outFd;

    /// Cache end-of-stream status
    bool m_outEos;
};

}   // end namespaces
} 
} 
#endif  // end header latch
