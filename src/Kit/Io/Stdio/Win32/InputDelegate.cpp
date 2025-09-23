/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/Stdio/InputDelegate.h"
#include "Fdio.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace Stdio {

//////////////////////
InputDelegate::InputDelegate( KitIoStdioHandle_T fd )
    : m_inFd( fd )
    , m_inEos( false )
{
}

//////////////////////
bool InputDelegate::read( void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept
{
    return Win32::Fdio::read( m_inFd, m_inEos, buffer, numBytes, bytesRead );
}


bool InputDelegate::available() noexcept
{
    return Win32::Fdio::availableStdin( m_inFd );
}

bool InputDelegate::isEos() noexcept
{
    return m_inEos;
}

void InputDelegate::close() noexcept
{
    // Mark the stream as closed - but DON'T close the underlying STDIO file descriptor
    m_inFd = INVALID_HANDLE_VALUE;
}

} // end namespace
}
}
//------------------------------------------------------------------------------