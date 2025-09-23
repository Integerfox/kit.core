/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/File/InputOutput.h"
#include "Kit/Io/File/System.h"
#include "Fdio.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace File {

InputOutput::InputOutput( const char* fileName, bool forceCreate, bool forceEmptyFile ) noexcept
    : m_fd( Win32::Fdio::open( System::getNative(fileName), false, forceCreate, forceEmptyFile ) )
    , m_eos( false )
{
}


InputOutput::~InputOutput() noexcept
{
    Win32::Fdio::close( m_fd );
}


bool InputOutput::isOpened() noexcept
{
    return m_fd != INVALID_HANDLE_VALUE;
}


//////////////////////////
bool InputOutput::read( void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept
{
    m_eos = false;
    return Win32::Fdio::read( m_fd, m_eos, buffer, numBytes, bytesRead );
}

bool InputOutput::available() noexcept
{
    return Win32::Fdio::available( m_fd );
}

bool InputOutput::isEos() noexcept
{
    return isOpened() == false || m_eos;
}

void InputOutput::close() noexcept
{
    Win32::Fdio::close( m_fd );
}


//////////////////////////
bool InputOutput::write( const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept
{
    m_eos = false;
    return Win32::Fdio::write( m_fd, m_eos, buffer, maxBytes, bytesWritten );
}

void InputOutput::flush() noexcept
{
    Win32::Fdio::flush( m_fd );
}


//////////////////////////
bool InputOutput::currentPos( ByteCount_T& curPos ) noexcept
{
    return Win32::Fdio::currentPos( m_fd, curPos );
}

bool InputOutput::setRelativePos( ByteCount_T deltaOffset ) noexcept
{
    return Win32::Fdio::setRelativePos( m_fd, deltaOffset );
}

bool InputOutput::setToEof() noexcept
{
    return Win32::Fdio::setToEof( m_fd );
}

bool InputOutput::setAbsolutePos( ByteCount_T newoffset ) noexcept
{
    return Win32::Fdio::setAbsolutePos( m_fd, newoffset );
}

bool InputOutput::isEof() noexcept
{
    return isEos();
}

bool InputOutput::length( ByteCount_T& len ) noexcept
{
    return Win32::Fdio::length( m_fd, len );
}

}  // end namespace
}
}
//------------------------------------------------------------------------------