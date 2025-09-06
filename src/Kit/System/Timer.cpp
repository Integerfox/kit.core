/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Timer.h"
#include "Assert.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

/////////////////////////////
Timer::Timer( TimerManager& timingSource ) noexcept
    : m_timingSource( &timingSource )
    , m_count( 0 )
{
}

Timer::Timer() noexcept
    : m_timingSource( nullptr )
    , m_count( 0 )
{
}

void Timer::setTimingSource( TimerManager& timingSource ) noexcept
{
    m_timingSource = &timingSource;
}

/////////////////////////////
void Timer::start( uint32_t timerDurationInMilliseconds ) noexcept
{
    KIT_SYSTEM_ASSERT( m_timingSource );
    m_timingSource->detach( *this );
    m_count = m_timingSource->msecToCounts( timerDurationInMilliseconds );
    m_timingSource->attach( *this );
}

void Timer::stop() noexcept
{
    if ( m_timingSource )
    {
        m_timingSource->detach( *this );
    }
    m_timingSource = nullptr;
}

void Timer::decrement( uint32_t milliseconds ) noexcept
{
    m_count -= milliseconds;
}

void Timer::increment( uint32_t milliseconds ) noexcept
{
    m_count += milliseconds;
}

uint32_t Timer::count() const noexcept
{
    return m_count;
}


}  // end namespace
}
//------------------------------------------------------------------------------