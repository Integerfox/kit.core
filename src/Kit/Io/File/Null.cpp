/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Null.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace File {

//////////////////////
Null::Null()
    :m_opened( true )
{
}


//////////////////////
bool Null::read( void* buffer, Kit::Type::SSize_T numBytes, Kit::Type::SSize_T& bytesRead ) noexcept
{
    bytesRead = 0;
    return false;
}

bool Null::available() noexcept
{
    return false;
}


//////////////////////
bool Null::write( const void* buffer, Kit::Type::SSize_T maxBytes, Kit::Type::SSize_T& bytesWritten ) noexcept
{
    bytesWritten = maxBytes;
    return m_opened;
}


void Null::flush() noexcept
{
    // Nothing to do!
}

bool Null::isEos() noexcept
{
    return false;
}

void Null::close() noexcept
{
    m_opened = false;
}


//////////////////////
bool Null::isEof() noexcept
{
    return isEos();
}

bool Null::length( Kit::Type::SSize_T& len ) noexcept
{
    len = 0;
    return m_opened;
}

bool Null::currentPos( Kit::Type::SSize_T& curPos ) noexcept
{
    curPos = 0;
    return m_opened;
}

bool Null::setRelativePos( Kit::Type::SSize_T deltaOffset ) noexcept
{
    return m_opened;
}

bool Null::setAbsolutePos( Kit::Type::SSize_T newoffset ) noexcept
{
    return m_opened;
}

bool Null::setToEof() noexcept
{
    return m_opened;
}

} // end namespace
}
}
//------------------------------------------------------------------------------