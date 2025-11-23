#ifndef KIT_LOGGING_FRAMEWORK_TIMEAPI_H_
#define KIT_LOGGING_FRAMEWORK_TIMEAPI_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "kit_config.h"
#include <stdint.h>

/** Default data type for storing a time-stamp value.  The units of the value
    are application specific.
 */
#ifndef KitLoggingTime_T
#define KitLoggingTime_T     uint64_t   // Compatible with the Kit::Time::BootTime interface
#endif

///
namespace Kit {
///
namespace Logging {
///
namespace Framework {

/** This method returns the application current 'time' value that will
    be used to time-stamp each individual log entry. The units of the value
    are application specific.
 */
KitLoggingTime_T now() noexcept;

}      // end namespace
}
}
#endif  // end header latch
