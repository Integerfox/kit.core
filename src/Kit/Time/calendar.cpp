/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Time/calendar.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Time {

void msecToCalendar( uint64_t utcEpochMilliseconds, struct tm& result, uint16_t& dstMilliseconds ) noexcept
{
    // Split into days since the Epoch and the time of day
    uint64_t secs      = utcEpochMilliseconds / 1000U;
    uint64_t days      = secs / 86400U;
    unsigned secsOfDay = static_cast<unsigned>( secs % 86400U );
    dstMilliseconds    = static_cast<uint16_t>( utcEpochMilliseconds % 1000U );

    // Convert days to a calendar date. Uses Howard Hinnant's (public domain)
    // 'civil_from_days' algorithm: http://howardhinnant.github.io/date_algorithms.html
    // The year is shifted to start on March 1st so that the leap day is the last
    // day of the shifted year.
    uint64_t z    = days + 719468U;                                               // days since 0000-03-01
    uint64_t era  = z / 146097U;                                                  // 400 year eras
    unsigned doe  = static_cast<unsigned>( z - era * 146097U );                   // day of era   [0, 146096]
    unsigned yoe  = ( doe - doe / 1460U + doe / 36524U - doe / 146096U ) / 365U;  // year of era  [0, 399]
    uint64_t year = yoe + era * 400U;
    unsigned doy  = doe - ( 365U * yoe + yoe / 4U - yoe / 100U );  // March based day of year  [0, 365]
    unsigned mp   = ( 5U * doy + 2U ) / 153U;                      // March based month [0, 11]
    unsigned day  = doy - ( 153U * mp + 2U ) / 5U + 1U;            // [1, 31]
    unsigned mon  = mp < 10U ? mp + 3U : mp - 9U;                  // [1, 12]
    if ( mon <= 2U )
    {
        year++;
    }

    // Day of the (January based) year: Jan/Feb are the last two months of the March based year
    bool     isLeap = ( year % 4U == 0U ) && ( year % 100U != 0U || year % 400U == 0U );
    unsigned yday   = mp >= 10U ? doy - 306U : doy + 59U + ( isLeap ? 1U : 0U );

    result          = tm();
    result.tm_sec   = static_cast<int>( secsOfDay % 60U );
    result.tm_min   = static_cast<int>( ( secsOfDay / 60U ) % 60U );
    result.tm_hour  = static_cast<int>( secsOfDay / 3600U );
    result.tm_mday  = static_cast<int>( day );
    result.tm_mon   = static_cast<int>( mon - 1U );
    result.tm_year  = static_cast<int>( year - 1900U );
    result.tm_wday  = static_cast<int>( ( days + 4U ) % 7U );  // 1970-01-01 was a Thursday
    result.tm_yday  = static_cast<int>( yday );
    result.tm_isdst = 0;
}

}  // end namespaces
}
//------------------------------------------------------------------------------
