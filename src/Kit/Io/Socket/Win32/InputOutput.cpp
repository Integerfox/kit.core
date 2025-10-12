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
    : m_fd( INVALID_SOCKET )
    , m_eos( false )
{
}

InputOutput::InputOutput( KitIoSocketHandle_T fd ) noexcept
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
    if ( fd != INVALID_SOCKET )
    {
        // Make sure that the current socket is closed first
        Win32::Fdio::close( m_fd );
        m_fd  = fd;
        m_eos = false;
    }
}

///////////////////
bool InputOutput::read( void* buffer, int numBytes, int& bytesRead ) noexcept
{
    return Win32::Fdio::read( m_fd, m_eos, buffer, numBytes, bytesRead );
}

bool InputOutput::available() noexcept
{
    return Win32::Fdio::available( m_fd );
}


//////////////////////
bool InputOutput::write( const void* buffer, int maxBytes, int& bytesWritten ) noexcept
{
    return Win32::Fdio::write( m_fd, m_eos, buffer, maxBytes, bytesWritten );
}

void InputOutput::flush() noexcept
{
    // I could use WSAIoctl() here with SIO_FLUSH - but according
    // to the Microsoft documentation - WSAIoctrl w/SIO_FLUSH could
    // block (unless using overlapped IO) - which is not the designed
    // behavior for this call -->so we will skip it for now (jtt 2-14-2015)
}

bool InputOutput::isEos() noexcept
{
    return m_eos;
}

void InputOutput::close() noexcept
{
    Win32::Fdio::close( m_fd );
}


}  // end namespace
}
}
//------------------------------------------------------------------------------