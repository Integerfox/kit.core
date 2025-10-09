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
    : m_inFd( Win32::Fdio::open( System::getNative(fileName) ) )
    , m_inEos( false )
{
}


Input::~Input() noexcept
{
    Win32::Fdio::close( m_inFd );
}


bool Input::isOpened() noexcept
{
    return m_inFd != INVALID_HANDLE_VALUE;
}


//////////////////////////
bool Input::read( void* buffer, int numBytes, int& bytesRead ) noexcept
{
    m_inEos = false;
    return Win32::Fdio::read( m_inFd, m_inEos, buffer, numBytes, bytesRead );
}

bool Input::available() noexcept
{
    return Win32::Fdio::available( m_inFd );
}

bool Input::isEos() noexcept
{
    return isOpened() == false || m_inEos;
}

void Input::close() noexcept
{
    Win32::Fdio::close( m_inFd );
}


//////////////////////////
bool Input::currentPos( ByteCount_T& curPos ) noexcept
{
    return Win32::Fdio::currentPos( m_inFd, curPos );
}

bool Input::setRelativePos( ByteCount_T deltaOffset ) noexcept
{
    return Win32::Fdio::setRelativePos( m_inFd, deltaOffset );
}

bool Input::setToEof() noexcept
{
    return Win32::Fdio::setToEof( m_inFd );
}

bool Input::setAbsolutePos( ByteCount_T newoffset ) noexcept
{
    return Win32::Fdio::setAbsolutePos( m_inFd, newoffset );
}

bool Input::isEof() noexcept
{
    return isEos();
}

bool Input::length( ByteCount_T& len ) noexcept
{
    return Win32::Fdio::length( m_inFd, len );
}


}  // end namespace
}
}
//------------------------------------------------------------------------------