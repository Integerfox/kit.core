#ifndef KIT_LOGGING_PKG_DRIVERMSGID_H_
#define KIT_LOGGING_PKG_DRIVERMSGID_H_
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
namespace Pkg {

/** This enumeration defines Logging message codes for the Driver sub-system

        Note: Each symbol's length must be less than or equal to 32 characters
           123456789 123456789 123456789 12

    @param DriverMsgId                       Enum

    @param START_ERR                         Error occurred while Starting the driver
    @param STOP_ERR                          Error occurred while Stopping the driver
    @param NOT_STARTED_ERR                   Error occurred when the Start was not called
    @param ERROR                             General Purpose Error

 */
// clang-format off
BETTER_ENUM( DriverMsgId, uint8_t
             , START_ERR
             , STOP_ERR
             , NOT_STARTED_ERR
             , ERROR
);
// clang-format on


}      // end namespaces
}
}
#endif  // end header latch
