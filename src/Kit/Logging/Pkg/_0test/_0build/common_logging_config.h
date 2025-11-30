#ifndef KIT_COMMON_LOGGING_CONFIG_H_
#define KIT_COMMON_LOGGING_CONFIG_H_
/*-----------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file Project/build specific 'Options' (see LConfig Pattern) */


// Configuration for Logging Identifiers
#include "Kit/Logging/Framework/_0test/_3pkgs/Foo/Logging/App/foo_mapcfg.h"

// Set Host Endianess (actual endianess does not matter for the test)
#define USE_KIT_TYPE_ENDIAN_LE_HOST

// Make unique -->so we can verify no copy-paste errors
#define OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_CLASSIFICATION_ID_TEXT "UNKNOWN-CLASS"
#define OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_PACKAGE_ID_TEXT        "UNKNOWN-PKG"
#define OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_SUBSYSTEM_ID_TEXT      "UNKNOWN-SS"
#define OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_MESSAGE_ID_TEXT        "UNKNOWN-MSG"

#endif
