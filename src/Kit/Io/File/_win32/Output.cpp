/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/File/Output.h"
#include "Kit/Io/File/System.h"
#include "fdio.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace File {


//////////////////////////
Output::Output( const char* fileName, bool forceCreate, bool forceEmptyFile )
    : m_outFd( Win32FileIO::open( System::getNative(fileName), false, forceCreate, forceEmptyFile ) )
    , m_outEos( false )
{
}

Output::~Output()
{
    Kit::Io::Stdio::Win32IO::close( m_outFd );
}


bool Output::isOpened()
{
    return m_outFd != INVALID_HANDLE_VALUE;
}


//////////////////////////
bool Output::write( const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept
{
    m_outEos = false;
    return Kit::Io::Stdio::Win32IO::write( m_outFd, m_outEos, buffer, maxBytes, bytesWritten );
}

void Output::flush() noexcept
{
    Kit::Io::Stdio::Win32IO::flush( m_outFd );
}

bool Output::isEos() noexcept
{
    return isOpened() == false || m_outEos;
}

void Output::close() noexcept
{
    Kit::Io::Stdio::Win32IO::close( m_outFd );
}


//////////////////////////
bool Output::currentPos( ByteCount_T& curPos ) noexcept
{
    return Win32FileIO::currentPos( m_outFd, curPos );
}

bool Output::setRelativePos( ByteCount_T deltaOffset ) noexcept
{
    return Win32FileIO::setRelativePos( m_outFd, deltaOffset );
}

bool Output::setToEof() noexcept
{
    return Win32FileIO::setToEof( m_outFd );
}

bool Output::setAbsolutePos( ByteCount_T newoffset ) noexcept
{
    return Win32FileIO::setAbsolutePos( m_outFd, newoffset );
}

bool Output::isEof() noexcept
{
    return isEos();
}

bool Output::length( ByteCount_T& len ) noexcept
{
    return Win32FileIO::length( m_outFd, len );
}


}  // end namespace
}
}
//------------------------------------------------------------------------------