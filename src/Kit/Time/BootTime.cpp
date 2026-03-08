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

static KitTimeBootCountMp_T* mpBootCounterPtr_;

#define SHIFT_ELAPSED_TIME ( ( sizeof( uint64_t ) - sizeof( KitTimeBootCount_T ) ) * 8 )
#define MASK_ELAPSED_TIME  ( ( UINT64_C( 1 ) << SHIFT_ELAPSED_TIME ) - 1 )

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

    if ( mpBootCounterPtr_ )
    {
        KitTimeBootCount_T bootCount = 0;  // Default to zero if MP is not valid
        mpBootCounterPtr_->read( bootCount );
        now |= ( (uint64_t)( bootCount ) ) << SHIFT_ELAPSED_TIME;
    }
    return now;
}

void Kit::Time::parseBootTime( uint64_t srcBootTime, KitTimeBootCount_T& dstBootCounter, uint64_t& dstElapsedTimeMs ) noexcept
{
    dstBootCounter   = (KitTimeBootCount_T)( srcBootTime >> SHIFT_ELAPSED_TIME );
    dstElapsedTimeMs = srcBootTime & MASK_ELAPSED_TIME;
}

uint64_t Kit::Time::constructBootTime( KitTimeBootCount_T srcBootCounter, uint64_t srcElapsedTimeMs ) noexcept
{
    uint64_t bt  = srcElapsedTimeMs & MASK_ELAPSED_TIME;
    bt          |= ( (uint64_t)srcBootCounter ) << SHIFT_ELAPSED_TIME;
    return bt;
}

void Kit::Time::initializeBootTime( KitTimeBootCountMp_T& bootCounterMp ) noexcept
{
    mpBootCounterPtr_ = &bootCounterMp;
}