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
#include "Fdio.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace File {


//////////////////////////
Output::Output( const char* fileName, bool forceCreate, bool forceEmptyFile )
    : m_outFd( Posix::Fdio::open( System::getNative(fileName), false, forceCreate, forceEmptyFile ) )
    , m_outEos( false )
{
}

Output::~Output()
{
    Posix::Fdio::close( m_outFd );
}


bool Output::isOpened()
{
    return m_outFd != Posix::Fdio::INVALID_FD;
}


//////////////////////////
bool Output::write( const void* buffer, Kit::Type::SSize_T maxBytes, Kit::Type::SSize_T& bytesWritten ) noexcept
{
    m_outEos = false;
    return Posix::Fdio::write( m_outFd, m_outEos, buffer, maxBytes, bytesWritten );
}

void Output::flush() noexcept
{
    Posix::Fdio::flush( m_outFd );
}

bool Output::isEos() noexcept
{
    return isOpened() == false || m_outEos;
}

void Output::close() noexcept
{
    Posix::Fdio::close( m_outFd );
}


//////////////////////////
bool Output::currentPos( Kit::Type::SSize_T& curPos ) noexcept
{
    return Posix::Fdio::currentPos( m_outFd, curPos );
}

bool Output::setRelativePos( Kit::Type::SSize_T deltaOffset ) noexcept
{
    return Posix::Fdio::setRelativePos( m_outFd, deltaOffset );
}

bool Output::setToEof() noexcept
{
    return Posix::Fdio::setToEof( m_outFd );
}

bool Output::setAbsolutePos( Kit::Type::SSize_T newoffset ) noexcept
{
    return Posix::Fdio::setAbsolutePos( m_outFd, newoffset );
}

bool Output::isEof() noexcept
{
    return isEos();
}

bool Output::length( Kit::Type::SSize_T& len ) noexcept
{
    return Posix::Fdio::length( m_outFd, len );
}


}  // end namespace
}
}
//------------------------------------------------------------------------------