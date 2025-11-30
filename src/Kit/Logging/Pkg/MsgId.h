#ifndef KIT_LOGGING_PKG_SYSTEMMSGID_H_
#define KIT_LOGGING_PKG_SYSTEMMSGID_H_
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

/** This enumeration defines Logging message codes for the Logging sub-system

        Note: Each symbol's length must be less than or equal to 32 characters
           123456789 123456789 123456789 12

    @param LoggingMsgId                      Enum

    @param OVERFLOW                          The logging FIFO overflowed, i.e. entries are being generated faster than they can be stored to persistent storage
    @param UNKNOWN_CLASSIFICATION_ID         Entry was generated with unknown/unsupported Classification ID.
    @param UNKNOWN_PACKAGE_ID                Entry was generated with unknown/unsupported Package ID.
    @param UNKNOWN_SUBSYSTEM_ID              Entry was generated with unknown/unsupported Sub-system ID.
    @param UNKNOWN_MESSAGE_ID                Entry was generated with unknown/unsupported Message ID.
 */
// clang-format off
BETTER_ENUM( LoggingMsgId, uint8_t
             , OVERFLOW
             , UNKNOWN_CLASSIFICATION_ID
             , UNKNOWN_PACKAGE_ID
             , UNKNOWN_SUBSYSTEM_ID
             , UNKNOWN_MESSAGE_ID
);


/** This enumeration defines Logging message codes for the OSAL sub-system

        Note: Each symbol's length must be less than or equal to 32 characters
           123456789 123456789 123456789 12

    @param SystemMsgId                       Enum

    @param FATAL_ERROR                       The application call the Fatal Error handler
    @param SHUTDOWN                          The application initiated an orderly shutdown
 */
// clang-format off
BETTER_ENUM( SystemMsgId, uint8_t
             , FATAL_ERROR
             , SHUTDOWN
);
// clang-format on


/** This enumeration defines Logging message codes for the Driver sub-system

        Note: Each symbol's length must be less than or equal to 32 characters
           123456789 123456789 123456789 12

    @param DriverMsgId                       Enum

    @param START_ERR                         Error occurred associated with Starting the driver
    @param STOP_ERR                          Error occurred associated with Stopping the driver
    @param ERROR                             General Purpose Error

 */
// clang-format off
BETTER_ENUM( DriverMsgId, uint8_t
             , START_ERR
             , STOP_ERR
             , ERROR
);
// clang-format on

}      // end namespaces
}
}
#endif  // end header latch
