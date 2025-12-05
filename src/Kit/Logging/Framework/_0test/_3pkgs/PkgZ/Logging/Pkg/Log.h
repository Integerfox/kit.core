#ifndef PKGZ_LOGGING_PKG_LOG_H_
#define PKGZ_LOGGING_PKG_LOG_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

This file declares the Logging functions available to the KIT Logging Domain.

*/

#include "kit_config.h"
#include "Kit/Logging/Framework/types.h"
#include "Kit/System/printfchecker.h"
#include "Kit/System/printfchecker.h"


// Support Conditionally compiling the Logging calls to Trace calls. 
#ifndef USE_PKGZ_LOGGING_PKG_LOG_API
// Provide default mapping configuration values if not provided by the application
#ifndef PKGZ_LOGGING_PKG_PACKAGE_ID_MAPCFG
#define PKGZ_LOGGING_PKG_PACKAGE_ID_MAPCFG 2
#endif
#ifndef PKGZ_LOGGING_PKG_CLASSIFICATION_ID_EVENT_MAPCFG
#define PKGZ_LOGGING_PKG_CLASSIFICATION_ID_EVENT_MAPCFG 1
#endif
#ifndef PKGZ_LOGGING_PKG_CLASSIFICATION_ID_WARNING_MAPCFG
#define PKGZ_LOGGING_PKG_CLASSIFICATION_ID_WARNING_MAPCFG 2
#endif
#ifndef PKGZ_LOGGING_PKG_CLASSIFICATION_ID_DEBUG_MAPCFG
#define PKGZ_LOGGING_PKG_CLASSIFICATION_ID_DEBUG_MAPCFG 4
#endif
#ifndef PKGZ_LOGGING_PKG_CLASSIFICATION_ID_METRICS_MAPCFG
#define PKGZ_LOGGING_PKG_CLASSIFICATION_ID_METRICS_MAPCFG 3
#endif

#define PKGZ_LOGGING_LOG_CORE( classificationId, messageId, ... )    KitLoggingFramework_logTracef( classificationId, Package::PACKAGE_ID, SubSystemId::CORE, messageId, __VA_ARGS__ )
#define PKGZ_LOGGING_LOG_NETWORK( classificationId, messageId, ... ) KitLoggingFramework_logTracef( classificationId, Package::PACKAGE_ID, SubSystemId::NETWORK, messageId, __VA_ARGS__ )
#define PKGZ_LOGGING_LOG_STORAGE( classificationId, messageId, ... ) KitLoggingFramework_logTracef( classificationId, Package::PACKAGE_ID, SubSystemId::STORAGE, messageId, __VA_ARGS__ )
#else

/// This method generates a CORE Sub-system log entry.  See logfCore() for details
#define PKGZ_LOGGING_LOG_CORE( classificationId, messageId, ... )    ::PkgZ::Logging::Pkg::logfCore( classificationId, messageId, __VA_ARGS__ )

/// This method generates a NETWORK Sub-system log entry. See logfNetwork() for details
#define PKGZ_LOGGING_LOG_NETWORK( classificationId, messageId, ... ) ::PkgZ::Logging::Pkg::logfNetwork( classificationId, messageId, __VA_ARGS__ )

/// This method generates a STORAGE Sub-system log entry. See logfStorage() for details
#define PKGZ_LOGGING_LOG_STORAGE( classificationId, messageId, ... ) ::PkgZ::Logging::Pkg::logfStorage( classificationId, messageId, __VA_ARGS__ )

#endif  // end !USE_PKGZ_LOGGING_PKG_LOG_API

//
// The following includes MUST BE AFTER the macros above.  The 'weird' ordering is so that a default Logging IDs can be provided when NOT ENABLING logging
//
#include "Kit/Logging/Framework/_0test/_3pkgs/PkgZ/Logging/Pkg/Package.h"
#include "Kit/Logging/Framework/_0test/_3pkgs/PkgZ/Logging/Pkg/ClassificationId.h"
#include "Kit/Logging/Framework/_0test/_3pkgs/PkgZ/Logging/Pkg/SubSystemId.h"
#include "Kit/Logging/Framework/_0test/_3pkgs/PkgZ/Logging/Pkg/MsgId.h"
#include "Kit/Logging/Framework/Log.h"


///
namespace PkgZ {
///
namespace Logging {
///
namespace Pkg {


/*---------------------------------------------------------------------------*/
/// This method generates a CORE Sub-system log entry. Do not call directly, use the PKGZ_LOGGING_LOG_SYSTEM() macro
KIT_SYSTEM_PRINTF_CHECKER( 3, 4 )
inline Kit::Logging::Framework::LogResult_T logfCore( ClassificationId catId, CoreMsgId msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    auto result = Kit::Logging::Framework::vlogf( catId, Package::PACKAGE_ID, SubSystemId::CORE, msgId, msgTextFormat, ap );
    va_end( ap );
    return result;
}

/// This method generates a NETWORK Sub-system log entry. Do not call directly, use the PKGZ_LOGGING_LOG_NETWORK() macro
KIT_SYSTEM_PRINTF_CHECKER( 3, 4 )
inline Kit::Logging::Framework::LogResult_T logfNetwork( ClassificationId catId, NetworkMsgId msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    auto result = Kit::Logging::Framework::vlogf( catId, Package::PACKAGE_ID, SubSystemId::NETWORK, msgId, msgTextFormat, ap );
    va_end( ap );
    return result;
}

/// This method generates a STORAGE Sub-system log entry. Do not call directly, use the PKGZ_LOGGING_LOG_STORAGE() macro
KIT_SYSTEM_PRINTF_CHECKER( 3, 4 )
inline Kit::Logging::Framework::LogResult_T logfStorage( ClassificationId catId, StorageMsgId msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    auto result = Kit::Logging::Framework::vlogf( catId, Package::PACKAGE_ID, SubSystemId::STORAGE, msgId, msgTextFormat, ap );
    va_end( ap );
    return result;
}


}
}
}
#endif  // end header latch