#ifndef KIT_TIME_CALENDAR_H_
#define KIT_TIME_CALENDAR_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file provides thread-safe methods for converting time values to
    broken-down calendar time (struct tm), expressed in Coordinated Universal
    Time (UTC).
 */

#include <time.h>
#include <stdint.h>

///
namespace Kit {
///
namespace Time {

/** This method converts 'utcEpochMilliseconds' - UTC time as milliseconds
    since the Epoch (1970-01-01 00:00:00) - to a broken-down calendar time
    representation, expressed in Coordinated Universal Time (UTC).  The result
    is stored in the user-provided 'result' struct tm instance.  All of the
    standard struct tm fields are set (tm_isdst is always zero).  The
    sub-second portion [0-999] of 'utcEpochMilliseconds' is returned via
    'dstMilliseconds'.  The conversion can not fail.

    The conversion is done arithmetically using Howard Hinnant's (public domain)
    date algorithms (http://howardhinnant.github.io/date_algorithms.html),
    i.e. it does not use the C library's gmtime() and it is thread-safe without
    requiring a mutex.  The results are identical to gmtime() - proleptic
    Gregorian calendar, no leap seconds - for every 'utcEpochMilliseconds'
    value, and the conversion is not limited by the size of time_t (e.g. it is
    valid after 2038 on platforms with a 32-bit time_t).

    NOTE: This method is preferred over gmtimeMT().
 */
void msecToCalendar( uint64_t utcEpochMilliseconds, struct tm& result, uint16_t& dstMilliseconds ) noexcept;

/** This method is a guaranteed thread-safe alternative to the C/C++ library's
    gmtime() call.

    The method converts the calendar time 'timep' to a broken-down time
    representation, expressed in Coordinated Universal Time (UTC). It may return
    nullptr when the year does not fit into an integer. The result is stored in
    the user-provided 'result' struct tm instance.

    NOTE: msecToCalendar() is preferred over this method.  msecToCalendar() is
          thread-safe WITHOUT a mutex, whereas this method's implementation
          for platforms without gmtime_r()/gmtime_s() (the _ansi variant)
          serializes calls to gmtime() with a mutex.  A mutex is undesirable on
          platforms that only provide partial priority inheritance (e.g.
          FreeRTOS).

    NOTE: This method is needed because the KIT library assumes ONLY the C++11
          language standard.  The C++11 standard does NOT guarantee/provide a
          thread-safe version of gmtime()
 */
struct tm* gmtimeMT( const time_t* timep, struct tm* result ) noexcept;


}  // end namespaces
}
#endif  // end header latch
