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
#include "Kit/System/Assert.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {

/////////////////////
    Null::Null() noexcept
    : m_opened( true )
{
}

/////////////////////
bool Null::read( void* buffer, Kit::Type::SSize_T numBytes, Kit::Type::SSize_T& bytesRead ) noexcept 
{
    KIT_SYSTEM_ASSERT( buffer != nullptr );

    bytesRead = 0;
    return false;
}

/// See Kit::Io::IInput
bool Null::available() noexcept 
{
    return false;
}

bool Null::write( const void* buffer, Kit::Type::SSize_T maxBytes, Kit::Type::SSize_T& bytesWritten ) noexcept 
{
    KIT_SYSTEM_ASSERT( buffer != nullptr );

    bytesWritten = maxBytes;
    return m_opened;
}

void Null::flush() noexcept 
{
}

bool Null::isEos() noexcept 
{
    return !m_opened;
}

void Null::close() noexcept 
{
    m_opened = false;
}

} // end namespace
}
//------------------------------------------------------------------------------