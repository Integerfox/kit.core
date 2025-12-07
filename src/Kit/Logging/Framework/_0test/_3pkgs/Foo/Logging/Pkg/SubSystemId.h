#ifndef FOO_LOGGING_PKG_SUBSYSTEMID_H_
#define FOO_LOGGING_PKG_SUBSYSTEMID_H_
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
#include "Kit/Type/BetterEnum.h"
#include <stdint.h>

///
namespace Foo {
///
namespace Logging {
///
namespace Pkg {

/** This enumeration defines Logging SubSystems Identifiers for the Foo test package.
    NOTE: The valid range for a SubSystem ID is [0-255].

        Note: Each symbol's length must be less than or equal to 16 characters
           123456789 123456

    @param SubSystemId       Enum

    @param UI                Foo User Interface sub-system
    @param DATABASE          Foo Database sub-system
    @param API               Foo API sub-system
 */
// clang-format off
BETTER_ENUM( SubSystemId, uint8_t
             , UI
             , DATABASE
             , API
);
// clang-format on

}  // end namespaces
}
}
#endif  // end header latch
