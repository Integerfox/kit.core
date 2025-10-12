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
bool Null::read( void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead ) noexcept
{
    bytesRead = 0;
    return false;
}

bool Null::available() noexcept
{
    return false;
}


//////////////////////
bool Null::write( const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept
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

bool Null::length( ByteCount_T& len ) noexcept
{
    len = 0;
    return m_opened;
}

bool Null::currentPos( ByteCount_T& curPos ) noexcept
{
    curPos = 0;
    return m_opened;
}

bool Null::setRelativePos( ByteCount_T deltaOffset ) noexcept
{
    return m_opened;
}

bool Null::setAbsolutePos( ByteCount_T newoffset ) noexcept
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