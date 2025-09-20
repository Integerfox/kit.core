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
#include "fdio.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace File {

InputOutput::InputOutput( const char* fileName, bool forceCreate, bool forceEmptyFile ) noexcept
    : m_fd( Win32FileIO::open( System::getNative(fileName), false, forceCreate, forceEmptyFile ) )
    , m_eos( false )
{
}


InputOutput::~InputOutput() noexcept
{
    Stdio::Win32IO::close( m_fd );
}


bool InputOutput::isOpened() noexcept
{
    return m_fd != INVALID_HANDLE_VALUE;
}


//////////////////////////
bool InputOutput::read( void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept
{
    m_eos = false;
    return Kit::Io::Stdio::Win32IO::read( m_fd, m_eos, buffer, numBytes, bytesRead );
}

bool InputOutput::available() noexcept
{
    return Kit::Io::Stdio::Win32IO::available( m_fd );
}

bool InputOutput::isEos() noexcept
{
    return isOpened() == false || m_eos;
}

void InputOutput::close() noexcept
{
    Kit::Io::Stdio::Win32IO::close( m_fd );
}


//////////////////////////
bool InputOutput::write( const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept
{
    m_eos = false;
    return Kit::Io::Stdio::Win32IO::write( m_fd, m_eos, buffer, maxBytes, bytesWritten );
}

void InputOutput::flush() noexcept
{
    Kit::Io::Stdio::Win32IO::flush( m_fd );
}


//////////////////////////
bool InputOutput::currentPos( ByteCount_T& curPos ) noexcept
{
    return Win32FileIO::currentPos( m_fd, curPos );
}

bool InputOutput::setRelativePos( ByteCount_T deltaOffset ) noexcept
{
    return Win32FileIO::setRelativePos( m_fd, deltaOffset );
}

bool InputOutput::setToEof() noexcept
{
    return Win32FileIO::setToEof( m_fd );
}

bool InputOutput::setAbsolutePos( ByteCount_T newoffset ) noexcept
{
    return Win32FileIO::setAbsolutePos( m_fd, newoffset );
}

bool InputOutput::isEof() noexcept
{
    return isEos();
}

bool InputOutput::length( ByteCount_T& len ) noexcept
{
    return Win32FileIO::length( m_fd, len );
}

}  // end namespace
}
}
//------------------------------------------------------------------------------