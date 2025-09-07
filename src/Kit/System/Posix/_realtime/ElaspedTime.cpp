/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/ElapsedTime.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace System {


/////////////////////////////////////////////////////////////////
uint32_t ElapsedTime::milliseconds( void ) noexcept
{
    return millisecondsInRealTime();
}

uint64_t ElapsedTime::millisecondsEx( void ) noexcept
{
    return millisecondsInRealTimeEx();
}

} // end namespace
}
//------------------------------------------------------------------------------