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
#include "Fdio.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace File {

//////////////////////////
Input::Input( const char* fileName ) noexcept
    : m_inFd( Posix::Fdio::open( System::getNative(fileName) ) )
    , m_inEos( false )
{
}


Input::~Input() noexcept
{
    Posix::Fdio::close( m_inFd );
}


bool Input::isOpened() noexcept
{
    return m_inFd != Posix::Fdio::INVALID_FD;
}


//////////////////////////
bool Input::read( void* buffer, int numBytes, int& bytesRead ) noexcept
{
    m_inEos = false;
    return Posix::Fdio::read( m_inFd, m_inEos, buffer, numBytes, bytesRead );
}

bool Input::available() noexcept
{
    return Posix::Fdio::available( m_inFd );
}

bool Input::isEos() noexcept
{
    return isOpened() == false || m_inEos;
}

void Input::close() noexcept
{
    Posix::Fdio::close( m_inFd );
}


//////////////////////////
bool Input::currentPos( ByteCount_T& curPos ) noexcept
{
    return Posix::Fdio::currentPos( m_inFd, curPos );
}

bool Input::setRelativePos( ByteCount_T deltaOffset ) noexcept
{
    return Posix::Fdio::setRelativePos( m_inFd, deltaOffset );
}

bool Input::setToEof() noexcept
{
    return Posix::Fdio::setToEof( m_inFd );
}

bool Input::setAbsolutePos( ByteCount_T newoffset ) noexcept
{
    return Posix::Fdio::setAbsolutePos( m_inFd, newoffset );
}

bool Input::isEof() noexcept
{
    return isEos();
}

bool Input::length( ByteCount_T& len ) noexcept
{
    return Posix::Fdio::length( m_inFd, len );
}


}  // end namespace
}
}
//------------------------------------------------------------------------------