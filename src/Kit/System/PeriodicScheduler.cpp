/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "PeriodicScheduler.h"
#include "Assert.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

PeriodicScheduler::PeriodicScheduler( Interval_T           intervals[],
                                      Hook_T               beginThreadProcessing,
                                      Hook_T               endThreadProcessing,
                                      ReportSlippageFunc_T slippageFunc,
                                      NowFunc_T            nowFunc ) noexcept
    : m_intervals( intervals )
    , m_reportSlippage( slippageFunc )
    , m_nowFunc( nowFunc )
    , m_beginThreadFunc( beginThreadProcessing )
    , m_endThreadFunc( endThreadProcessing )
    , m_firstExecution( true )
{
    KIT_SYSTEM_ASSERT( intervals );
    KIT_SYSTEM_ASSERT( nowFunc );
}

void PeriodicScheduler::beginLoop() noexcept
{
    if ( m_beginThreadFunc )
    {
        ( m_beginThreadFunc )( (m_nowFunc)() );
    }
}

void PeriodicScheduler::endLoop() noexcept
{
    if ( m_endThreadFunc )
    {
        ( m_endThreadFunc )( (m_nowFunc)() );
    }
}

/////////////////////
bool PeriodicScheduler::executeScheduler() noexcept
{
    bool        atLeastOne = false;
    Interval_T* interval   = m_intervals;

    // Scan all intervals
    while ( interval && interval->callbackFunc != 0 )
    {
        // Get the current time
        uint64_t currentTick = (m_nowFunc)();

        // Initialize the interval (but only once)
        if ( m_firstExecution )
        {
            setTimeMarker( *interval, currentTick );
        }

        // Has the interval expired?
        if ( ElapsedTime::expiredMillisecondsEx( interval->timeMarker, interval->duration, currentTick ) )
        {
            atLeastOne            = true;
            interval->timeMarker += interval->duration;
            ( interval->callbackFunc )( currentTick, interval->timeMarker, interval->context );

            // Check for slippage
            if ( ElapsedTime::expiredMillisecondsEx( interval->timeMarker, interval->duration, currentTick ) )
            {
                // Report the slippage to the application
                if ( m_reportSlippage )
                {
                    ( m_reportSlippage )( *interval, currentTick, interval->timeMarker );
                }

                // Re-sync the most recent past interval boundary based on the actual time.
                // Note: This operation only has a 'effect' if the slipped time is greater than 2 duration times
                setTimeMarker( *interval, currentTick );
            }
        }

        // Get the next interval
        interval++;
    }

    // Clear flag now that we have properly initialized each interval
    m_firstExecution = false;

    // Return result;
    return atLeastOne;
}


void PeriodicScheduler::setTimeMarker( Interval_T& interval, uint64_t currentTick ) noexcept
{
    // Make sure there is no divide by zero error
    if ( interval.duration == 0 )
    {
        interval.timeMarker = currentTick;
        return;
    }

    // Round down to the nearest interval boundary
    interval.timeMarker = ( currentTick / interval.duration ) * interval.duration;
}


}  // end namespaces
}
//------------------------------------------------------------------------------