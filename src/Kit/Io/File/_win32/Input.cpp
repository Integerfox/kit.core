/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/File/Input.h"
#include "Kit/Io/File/System.h"
#include "fdio.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace File {

//////////////////////////
Input::Input( const char* fileName ) noexcept
    : m_inFd( Win32FileIO::open( System::getNative(fileName) ) )
{
}


Input::~Input() noexcept
{
    Stdio::Win32IO::close( m_inFd );
}


bool Input::isOpened() noexcept
{
    return m_inFd != INVALID_HANDLE_VALUE;
}


//////////////////////////
bool Input::read( void* buffer, int numBytes, int& bytesRead ) noexcept
{
    m_inEos = false;
    return Kit::Io::Stdio::Win32IO::read( m_inFd, m_inEos, buffer, numBytes, bytesRead );
}

bool Input::available() noexcept
{
    return Kit::Io::Stdio::Win32IO::available( m_inFd );
}

bool Input::isEos() noexcept
{
    return isOpened() == false || m_inEos;
}

void Input::close() noexcept
{
    Kit::Io::Stdio::Win32IO::close( m_inFd );
}


//////////////////////////
bool Input::currentPos( ByteCount_T& curPos ) noexcept
{
    return Win32FileIO::currentPos( m_inFd, curPos );
}

bool Input::setRelativePos( ByteCount_T deltaOffset ) noexcept
{
    return Win32FileIO::setRelativePos( m_inFd, deltaOffset );
}

bool Input::setToEof() noexcept
{
    return Win32FileIO::setToEof( m_inFd );
}

bool Input::setAbsolutePos( ByteCount_T newoffset ) noexcept
{
    return Win32FileIO::setAbsolutePos( m_inFd, newoffset );
}

bool Input::isEof() noexcept
{
    return isEos();
}

bool Input::length( ByteCount_T& len ) noexcept
{
    return Win32FileIO::length( m_inFd, len );
}


}  // end namespace
}
}
//------------------------------------------------------------------------------