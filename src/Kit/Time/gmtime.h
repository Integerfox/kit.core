#ifndef KIT_TIME_GMTIME_H_
#define KIT_TIME_GMTIME_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include <time.h>

///
namespace Kit {
///
namespace Time {

/** This method is a guaranteed thread-safe alternative to the C/C++ library's
    gmtime() call.

    The method converts the calendar time 'timep' to a broken-down time 
    representation, expressed in Coordinated Universal Time (UTC). It may return
    nullptr when the year does not fit into an integer. The result is stored in
    the user-provided 'result' struct tm instance.

    NOTE: This method is needed because the KIT library assumes ONLY the C++11
          language standard.  The C++11 standard does NOT guarantee/provide a
          thread-safe version of gmtime()
 */
struct tm *gmtimeMT(const time_t *timep, struct tm *result) noexcept;


}  // end namespaces
}
#endif  // end header latch