#ifndef KIT_LOGGING_PKG_SUBSYSTEMID_H_
#define KIT_LOGGING_PKG_SUBSYSTEMID_H_
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

/** This enumeration defines Logging SubSystems Identifiers. NOTE: The valid range
    for a SubSystem ID is [0-255].

        Note: Each symbol's length must be less than or equal to 16 characters
           123456789 123456

    @param SubSystemId       Enum

    @param OSAL              Kit OSAL, i.e. the Kit::System namespace
    @param DRIVER            Kit Drivers, i.e. the Kit::Driver namespace
 */
// clang-format off
BETTER_ENUM( SubSystemId, uint8_t
             , OSAL = 1
             , DRIVER
);
// clang-format on

}      // end namespaces
}
}
#endif  // end header latch
