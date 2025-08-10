/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/api.h"
#include "Windows.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {


////////////////////
void initialize() noexcept
{
    // TODO: IMPLEMENT ME - this is a placeholder for the actual implementation
}

void enableScheduling() noexcept
{
    // Do nothing since by definition we are running in user space
}

bool isSchedulingEnabled() noexcept
{
    return true;  // Always return true since scheduling is always enabled by definition of running in user space
}

void sleepInRealTime( uint32_t milliseconds ) noexcept
{
    Sleep( (DWORD) milliseconds );
}


void suspendScheduling() noexcept
{
    // DO NOT Nothing.  No native Win32 API for this - and one could argue that suspending scheduling on Desktop OS is bad thing
}

void resumeScheduling() noexcept
{
    // DO NOT Nothing.  See above
}

}
}
//------------------------------------------------------------------------------
