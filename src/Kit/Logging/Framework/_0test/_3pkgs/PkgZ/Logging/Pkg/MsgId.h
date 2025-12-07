#ifndef PKGZ_LOGGING_PKG_MSGID_H_
#define PKGZ_LOGGING_PKG_MSGID_H_
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
namespace PkgZ {
///
namespace Logging {
///
namespace Pkg {

/** This enumeration defines Logging message codes for the PkgZ Core sub-system

        Note: Each symbol's length must be less than or equal to 32 characters
           123456789 123456789 123456789 12

    @param CoreMsgId                         Enum

    @param INIT_SUCCESS                      Core initialization successful
    @param INIT_FAILED                       Core initialization failed
    @param CONFIG_ERROR                      Configuration error detected
 */
// clang-format off
BETTER_ENUM( CoreMsgId, uint8_t
             , INIT_SUCCESS
             , INIT_FAILED
             , CONFIG_ERROR
);
// clang-format on

/** This enumeration defines Logging message codes for the PkgZ Network sub-system

        Note: Each symbol's length must be less than or equal to 32 characters
           123456789 123456789 123456789 12

    @param NetworkMsgId                      Enum

    @param CONNECTION_OPENED                 Network connection opened
    @param CONNECTION_CLOSED                 Network connection closed
    @param DATA_RECEIVED                     Data received on network
    @param SEND_ERROR                        Error sending data
 */
// clang-format off
BETTER_ENUM( NetworkMsgId, uint8_t
             , CONNECTION_OPENED
             , CONNECTION_CLOSED
             , DATA_RECEIVED
             , SEND_ERROR
);
// clang-format on

/** This enumeration defines Logging message codes for the PkgZ Storage sub-system

        Note: Each symbol's length must be less than or equal to 32 characters
           123456789 123456789 123456789 12

    @param StorageMsgId                      Enum

    @param READ_SUCCESS                      Storage read successful
    @param WRITE_SUCCESS                     Storage write successful
    @param READ_ERROR                        Storage read error
    @param WRITE_ERROR                       Storage write error
 */
// clang-format off
BETTER_ENUM( StorageMsgId, uint8_t
             , READ_SUCCESS
             , WRITE_SUCCESS
             , READ_ERROR
             , WRITE_ERROR
);
// clang-format on

}  // end namespaces
}
}
#endif  // end header latch
