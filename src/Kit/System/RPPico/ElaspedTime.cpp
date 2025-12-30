/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/System/ElapsedTime.h"
#include "Cpl/System/Private_.h"
#include <pico/time.h>


/// 
using namespace Cpl::System;




///////////////////////////////////////////////////////////////
// Simulated time NOT supported
unsigned long ElapsedTime::milliseconds( void ) noexcept
{
    return millisecondsInRealTime();
}

unsigned long ElapsedTime::seconds( void ) noexcept
{
    return secondsInRealTime();
}

ElapsedTime::Precision_T ElapsedTime::precision( void ) noexcept
{
    return precisionInRealTime();
}
