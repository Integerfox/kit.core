/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/Socket/InputOutput.h"
#include "Fdio.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace Socket {

/////////////////////
InputOutput::InputOutput() noexcept
    : m_fd( Posix::Fdio::INVALID_FD )
    , m_eos( false )
{
}

InputOutput::InputOutput( KitIoSocketHandle_T fd) noexcept
    : m_fd( fd )
    , m_eos( false )
{
}


InputOutput::~InputOutput() noexcept
{
    close();
}


///////////////////
void InputOutput::activate( KitIoSocketHandle_T fd ) noexcept
{
    // Only activate if a valid socket handle is provided
    if ( fd != Posix::Fdio::INVALID_FD )
    {
        // Make sure that the current socket is closed first
        Posix::Fdio::close( m_fd );
        m_fd  = fd;
        m_eos = false;
    }
}

///////////////////
bool InputOutput::read( void* buffer, int numBytes, int& bytesRead ) noexcept
{
    return Posix::Fdio::read( m_fd, m_eos, buffer, numBytes, bytesRead );
}

bool InputOutput::available() noexcept
{
    return Posix::Fdio::available( m_fd );
}


//////////////////////
bool InputOutput::write( const void* buffer, int maxBytes, int& bytesWritten ) noexcept
{
    return Posix::Fdio::write( m_fd, m_eos, buffer, maxBytes, bytesWritten );
}

void InputOutput::flush() noexcept
{
    // Do not know how to implement using only Posix  (jtt 2-14-2015)
}

bool InputOutput::isEos() noexcept
{
    return m_eos;
}

void InputOutput::close() noexcept
{
    Posix::Fdio::close( m_fd );
}


}  // end namespace
}
}
//------------------------------------------------------------------------------