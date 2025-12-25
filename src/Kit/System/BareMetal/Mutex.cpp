/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/System/Mutex.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

//////////////////////////////////////////////////////////////////////////////
Mutex::Mutex() noexcept
{
    // Nothing needed
}

Mutex::~Mutex() noexcept
{
    // Nothing needed
}

void Mutex::lock() noexcept
{
    // Do nothing - has no meaning for single threaded system
}


void Mutex::unlock() noexcept
{
    // Do nothing - has no meaning for single threaded system
}

}  // end namespace
}
//------------------------------------------------------------------------------