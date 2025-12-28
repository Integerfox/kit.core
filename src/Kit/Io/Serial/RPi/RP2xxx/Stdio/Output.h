#ifndef KIT_IO_SERIAL_RPI_RP2XXX_STDIO_OUTPUT_H
#define KIT_IO_SERIAL_RPI_RP2XXX_STDIO_OUTPUT_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/IOutput.h"


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
namespace Stdio {


/** This concrete class implements a Output stream using Raspberry Pi RP2xxx's
    SDK.  Multiple instances of this class can be created because there is
    only actual stdout stream.

    Notes:
        1. The implement does NOT support the blocking semantics of the Kit::Io
           streams.  The design decision was for the implementation to be
           compatible/useful on bare-metal systems, i.e. no threads required.
        2. The Application is RESPONSIBLE for calling stdio_init_all() on
           start-up of the application BEFORE any calls to this class (other
           than the constructor).
 */
class Output : public Kit::Io::IOutput
{
public:
    /// Constructor
    Output() noexcept = default;

public:
    /// Pull in overloaded methods from base class
    using Kit::Io::IOutput::write;

    /// See Kit::Io::IOutput
    bool write( const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept override;

    /// See Kit::Io::IOutput
    void flush() noexcept override;

    /// See Kit::Io::IEos
    bool isEos() noexcept override;

    /// See Kit::Io::IOutput
    void close() noexcept override;
};

}  // end namespaces
}
}
}
}
}
#endif  // end header latch
