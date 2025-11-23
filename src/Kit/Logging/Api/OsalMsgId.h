#ifndef KIT_LOGGING_API_OSALMSGID_H_
#define KIT_LOGGING_API_OSALMSGID_H_
/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2014-2023  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Type/BetterEnum.h"
#include <stdint.h>

///
namespace Kit {
///
namespace Logging {
///
namespace Api {

/** This enumeration defines Logging message codes for the OSAL sub-system

        Note: Each symbol's length must be less than or equal to 32 characters
           123456789 123456789 123456789 12

    @param OsalMsgId                         Enum

    @param FATAL_ERROR                       The application call the Fatal Error handler
    @param SHUTDOWN                          The application initiated an orderly shutdown
 */
// clang-format off
BETTER_ENUM( OsalMsgId, uint8_t
             , FATAL_ERROR
             , SHUTDOWN
);
// clang-format on


}      // end namespaces
}
}
#endif  // end header latch
