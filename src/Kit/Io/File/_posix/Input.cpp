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
    : m_inFd( PosixFileIO::open( System::getNative(fileName) ) )
    , m_inEos( false )
{
}


Input::~Input() noexcept
{
    Stdio::PosixIO::close( m_inFd );
}


bool Input::isOpened() noexcept
{
    return m_inFd != Stdio::PosixIO::INVALID_FD;
}


//////////////////////////
bool Input::read( void* buffer, int numBytes, int& bytesRead ) noexcept
{
    m_inEos = false;
    return Kit::Io::Stdio::PosixIO::read( m_inFd, m_inEos, buffer, numBytes, bytesRead );
}

bool Input::available() noexcept
{
    return Kit::Io::Stdio::PosixIO::available( m_inFd );
}

bool Input::isEos() noexcept
{
    return isOpened() == false || m_inEos;
}

void Input::close() noexcept
{
    Kit::Io::Stdio::PosixIO::close( m_inFd );
}


//////////////////////////
bool Input::currentPos( ByteCount_T& curPos ) noexcept
{
    return PosixFileIO::currentPos( m_inFd, curPos );
}

bool Input::setRelativePos( ByteCount_T deltaOffset ) noexcept
{
    return PosixFileIO::setRelativePos( m_inFd, deltaOffset );
}

bool Input::setToEof() noexcept
{
    return PosixFileIO::setToEof( m_inFd );
}

bool Input::setAbsolutePos( ByteCount_T newoffset ) noexcept
{
    return PosixFileIO::setAbsolutePos( m_inFd, newoffset );
}

bool Input::isEof() noexcept
{
    return isEos();
}

bool Input::length( ByteCount_T& len ) noexcept
{
    return PosixFileIO::length( m_inFd, len );
}


}  // end namespace
}
}
//------------------------------------------------------------------------------