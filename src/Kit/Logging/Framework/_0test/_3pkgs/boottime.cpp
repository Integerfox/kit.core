/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Time/BootTime.h"
#include "Kit/System/ElapsedTime.h"

uint16_t g_bootCount;  // Accessible but the unit test code

#define MASK_ELAPSED_TIME  0x0000FFFFFFFFFFFFLL
#define SHIFT_ELAPSED_TIME ( 6 * 8 )

uint64_t Kit::Time::getBootTime() noexcept
{
    uint64_t now  = Kit::System::ElapsedTime::millisecondsEx();
    now          &= MASK_ELAPSED_TIME;
    now          |= ( (uint64_t)( g_bootCount ) ) << SHIFT_ELAPSED_TIME;
    return now;
}

void Kit::Time::parseBootTime( uint64_t srcBootTime, uint16_t& dstBootCounter, uint64_t& dstElapsedTimeMs ) noexcept
{
    dstBootCounter   = (uint16_t)( srcBootTime >> SHIFT_ELAPSED_TIME );
    dstElapsedTimeMs = srcBootTime & MASK_ELAPSED_TIME;
}

uint64_t Kit::Time::constructBootTime( uint16_t srcBootCounter, uint64_t srcElapsedTimeMs ) noexcept
{
    uint64_t bt  = srcElapsedTimeMs & MASK_ELAPSED_TIME;
    bt          |= ( (uint64_t)srcBootCounter ) << SHIFT_ELAPSED_TIME;
    return bt;
}