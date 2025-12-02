/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "BootTime.h"

// TODO: MP Support
//static Cpl::Dm::Mp::Uint32* mpBootCounterPtr_;

#define MASK_ELAPSED_TIME  0x0000FFFFFFFFFFFFLL
#define SHIFT_ELAPSED_TIME ( 6 * 8 )

#ifdef USE_KIT_TIME_BOOTTIME_WITH_ABSOLUTETIME
#include "AbsoluteTime.h"
#define GET_TIME() Kit::Time::getAbsoluteTimeMilliseconds()
#else
#include "Kit/System/ElapsedTime.h"
#define GET_TIME() Kit::System::ElapsedTime::millisecondsEx()
#endif


uint64_t Kit::Time::getBootTime() noexcept
{
    uint64_t now  = GET_TIME();
    now          &= MASK_ELAPSED_TIME;

    // TODO: MP Support
    // if ( mpBootCounterPtr_ )
    // {
    //     uint32_t bootCount = 0;  // Default to zero if MP is not valid
    //     mpBootCounterPtr_->read( bootCount );
    //     now |= ( (uint64_t)( bootCount ) ) << SHIFT_ELAPSED_TIME;
    // }
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

// TODO: MP Support
// void Kit::Time::initializeBootTime( Cpl::Dm::Mp::Uint32& bootCounterMp ) noexcept
// {
//     mpBootCounterPtr_ = &bootCounterMp;
// }