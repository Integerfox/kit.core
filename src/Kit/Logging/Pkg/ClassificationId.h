#ifndef KIT_LOGGING_PKG_CLASSIFICATIONID_H_
#define KIT_LOGGING_PKG_CLASSIFICATIONID_H_
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
/** @file

Note: The Application is responsible for defining the Classification values.  The
      valid range for a Classification ID is [1-32]. If the Application does not
      'support' a Package's Classification - the Application sets the Package's
      Classification value to zero.
*/


#include "kit_config.h"
#include "Kit/Type/BetterEnum.h"
#include <stdint.h>

/// Defer the actual values to the Application (via the kit_config.h file).
#define KIT_LOGGING_API_CLASSIFICATION_ID_FATAL KIT_LOGGING_API_CLASSIFICATION_ID_FATAL_MAPCFG

/// Defer the actual values to the Application (via the kit_config.h file).
#define KIT_LOGGING_API_CLASSIFICATION_ID_WARNING KIT_LOGGING_API_CLASSIFICATION_ID_WARNING_MAPCFG

/// Defer the actual values to the Application (via the kit_config.h file).
#define KIT_LOGGING_API_CLASSIFICATION_ID_EVENT KIT_LOGGING_API_CLASSIFICATION_ID_EVENT_MAPCFG

/// Defer the actual values to the Application (via the kit_config.h file).
#define KIT_LOGGING_API_CLASSIFICATION_ID_INFO KIT_LOGGING_API_CLASSIFICATION_ID_INFO_MAPCFG

///
namespace Kit {
///
namespace Logging {
///
namespace Pkg {

/** This enumeration defines the logging Classification IDs for the KIT Logging Package.
    NOTE: The valid range for a Classification ID is [1-32].

    Note: Each symbol's length must be less than or equal to 16 characters
           123456789 123456

    @param ClassificationId     Enum

    @param FATAL                A fatal unrecoverable error occurred.  The system will be 'shutdown'.  What 'shutdown' means is platform dependent.
    @param WARNING              A recoverable error and/or an unexpected condition occurred
    @param EVENT                A significant event occurred.
    @param INFO                 Informational.  Typically used for detailed troubleshooting and is not recommended to be enabled by default
 */
// clang-format off
BETTER_ENUM( ClassificationId, uint8_t,
             FATAL   = KIT_LOGGING_API_CLASSIFICATION_ID_FATAL,
             WARNING = KIT_LOGGING_API_CLASSIFICATION_ID_WARNING,
             EVENT   = KIT_LOGGING_API_CLASSIFICATION_ID_EVENT,
             INFO    = KIT_LOGGING_API_CLASSIFICATION_ID_INFO
);
// clang-format off


}  // end namespaces
}
}
#endif  // end header latch
