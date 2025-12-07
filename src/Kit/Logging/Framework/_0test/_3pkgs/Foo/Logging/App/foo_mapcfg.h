#ifndef FOO_LOGGING_APP_FOO_MAPCFG_H_
#define FOO_LOGGING_APP_FOO_MAPCFG_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file provides the Application level Logging Identifiers mappings for
    the contained Package(s).  This file needs to included by the "application's"
    kit_config.h file.

    NOTE: Do not include OTHER HEADER FILES in this file!  It will create
          circular dependencies issues.  Unfortunately, this means that contents
          of this file must be 'hand-crafted'.  The good news is that after the
          initial creation - the contents rarely change
*/

// Note: Classification ID Valid Range: 1-32
// Note: Package ID Valid Range: 1-32

//
// Foo Package Logging Identifiers, AKA the Application's Classification IDs
//
#define FOO_LOGGING_PKG_CLASSIFICATION_ID_WARNING_MAPCFG 1
#define FOO_LOGGING_PKG_CLASSIFICATION_ID_EVENT_MAPCFG   2
#define FOO_LOGGING_PKG_CLASSIFICATION_ID_DEBUG_MAPCFG   3

#define FOO_LOGGING_PKG_PACKAGE_ID_MAPCFG                1

//
// Kit Package Logging Identifiers
//
#define KIT_LOGGING_PKG_CLASSIFICATION_ID_FATAL_MAPCFG   FOO_LOGGING_PKG_CLASSIFICATION_ID_WARNING_MAPCFG
#define KIT_LOGGING_PKG_CLASSIFICATION_ID_WARNING_MAPCFG FOO_LOGGING_PKG_CLASSIFICATION_ID_WARNING_MAPCFG
#define KIT_LOGGING_PKG_CLASSIFICATION_ID_EVENT_MAPCFG   FOO_LOGGING_PKG_CLASSIFICATION_ID_EVENT_MAPCFG
#define KIT_LOGGING_PKG_CLASSIFICATION_ID_INFO_MAPCFG    FOO_LOGGING_PKG_CLASSIFICATION_ID_DEBUG_MAPCFG

#define KIT_LOGGING_PKG_PACKAGE_ID_MAPCFG                ( FOO_LOGGING_PKG_PACKAGE_ID_MAPCFG + 1 )

//
// Z Package Logging Identifiers
//
#define PKGZ_LOGGING_PKG_CLASSIFICATION_ID_EVENT_MAPCFG   FOO_LOGGING_PKG_CLASSIFICATION_ID_EVENT_MAPCFG
#define PKGZ_LOGGING_PKG_CLASSIFICATION_ID_WARNING_MAPCFG FOO_LOGGING_PKG_CLASSIFICATION_ID_WARNING_MAPCFG
#define PKGZ_LOGGING_PKG_CLASSIFICATION_ID_DEBUG_MAPCFG   FOO_LOGGING_PKG_CLASSIFICATION_ID_DEBUG_MAPCFG
#define PKGZ_LOGGING_PKG_CLASSIFICATION_ID_METRICS_MAPCFG FOO_LOGGING_PKG_CLASSIFICATION_ID_EVENT_MAPCFG

#define PKGZ_LOGGING_PKG_PACKAGE_ID_MAPCFG                ( KIT_LOGGING_PKG_PACKAGE_ID_MAPCFG + 1 )

#endif  // end header latch