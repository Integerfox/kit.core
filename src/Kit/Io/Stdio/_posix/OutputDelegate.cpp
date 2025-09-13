/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/Stdio/OutputDelegate.h"
#include "fdio.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace Stdio {

//////////////////////
OutputDelegate::OutputDelegate( KitIoStdioHandle_T fd ) noexcept
    : m_outFd( fd )
    , m_outEos( false )
{
}

//////////////////////
bool OutputDelegate::write( const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept
{
    return PosixIO::write( m_outFd, m_outEos, buffer, maxBytes, bytesWritten );
}

void OutputDelegate::flush() noexcept
{
    PosixIO::flush( m_outFd );
}

bool OutputDelegate::isEos()  noexcept
{
    return m_outEos;
}

void OutputDelegate::close() noexcept
{
    // Mark the stream as closed - but DON'T close the underlying STDIO file descriptor
    m_outFd = PosixIO::INVALID_FD;
}

} // end namespace
}
}
//------------------------------------------------------------------------------