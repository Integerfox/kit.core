#ifndef KIT_LOGGING_FRAMEWORK_TYPES_H_
#define KIT_LOGGING_FRAMEWORK_TYPES_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file declares various Logging specific types

*/

#include "kit_config.h"
#include <stdint.h>

/** This data type determines the range of Classification IDs.  The range will
    be 1 to the number-of-bits in KitLoggingClassificationMask_T.

    Yes I am lazy here and use the LConfig pattern, instead of the LHeader pattern
    to define this type.
 */
#ifndef KitLoggingClassificationMask_T
#define KitLoggingClassificationMask_T uint32_t
#endif

/** This data type determines the range of Package IDs.  The range will
    be 1 to the number-of-bits in KitLoggingPackageMask_T.
 */
#ifndef KitLoggingPackageMask_T
#define KitLoggingPackageMask_T uint32_t
#endif

///
namespace Kit {
///
namespace Logging {
///
namespace Framework {

/// Result codes for the Log::vlogf() method
enum LogResult_T
{
    ADDED      = 0,   //!< Log entry was successfully created and placed into the log entry queue,
    FILTERED   = 1,   //!< Log entry was filtered out by the Classification/Package filters
    QUEUE_FULL = -1,  //!< Log entry queue was full - log entry was NOT added
};


}  // end namespaces
}
}
#endif  // end header latch
