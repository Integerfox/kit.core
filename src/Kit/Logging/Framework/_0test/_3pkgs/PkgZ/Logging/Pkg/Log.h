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

#include "Kit/Logging/Framework/_0test/_3pkgs/PkgZ/Logging/Pkg/Package.h"
#include "Kit/Logging/Framework/_0test/_3pkgs/PkgZ/Logging/Pkg/ClassificationId.h"
#include "Kit/Logging/Framework/_0test/_3pkgs/PkgZ/Logging/Pkg/SubSystemId.h"
#include "Kit/Logging/Framework/_0test/_3pkgs/PkgZ/Logging/Pkg/MsgId.h"
#include "Kit/Logging/Framework/Log.h"
#include "Kit/System/printfchecker.h"


// Support Conditionally compiling the Logging calls to Trace calls. 
#ifdef DISABLED_KIT_LOGGING_PKG_LOG_API
#define PKGZ_LOGGING_LOG_CORE( classificationId, messageId, ... )    KitLoggingFramework_logTracef( classificationId, Package::PACKAGE_ID, SubSystemId::CORE, messageId, __VA_ARGS__ )
#define PKGZ_LOGGING_LOG_NETWORK( classificationId, messageId, ... ) KitLoggingFramework_logTracef( classificationId, Package::PACKAGE_ID, SubSystemId::NETWORK, messageId, __VA_ARGS__ )
#define PKGZ_LOGGING_LOG_STORAGE( classificationId, messageId, ... ) KitLoggingFramework_logTracef( classificationId, Package::PACKAGE_ID, SubSystemId::STORAGE, messageId, __VA_ARGS__ )
#else

/** This method generates a CORE Sub-system log entry
    @param classificationId               Classification ID of the log entry
    @param messageId                      Message ID of the log entry
    @param msgTextFormat                  Printf style format string for the log entry's info text
    @param ...                            Variable arguments for the format string

    @return Kit::Logging::Framework::LogResult_T
*/
#define PKGZ_LOGGING_LOG_CORE( classificationId, messageId, ... )    ::PkgZ::Logging::Pkg::logfCore( classificationId, messageId, __VA_ARGS__ )

/** This method generates a NETWORK Sub-system log entry
    @param classificationId               Classification ID of the log entry
    @param messageId                      Message ID of the log entry
    @param msgTextFormat                  Printf style format string for the log entry's info text
    @param ...                            Variable arguments for the format string

    @return Kit::Logging::Framework::LogResult_T
*/
#define PKGZ_LOGGING_LOG_NETWORK( classificationId, messageId, ... ) ::PkgZ::Logging::Pkg::logfNetwork( classificationId, messageId, __VA_ARGS__ )

/** This method generates a STORAGE Sub-system log entry
    @param classificationId               Classification ID of the log entry
    @param messageId                      Message ID of the log entry
    @param msgTextFormat                  Printf style format string for the log entry's info text
    @param ...                            Variable arguments for the format string

    @return Kit::Logging::Framework::LogResult_T
*/
#define PKGZ_LOGGING_LOG_STORAGE( classificationId, messageId, ... ) ::PkgZ::Logging::Pkg::logfStorage( classificationId, messageId, __VA_ARGS__

#endif  // end DISABLED_KIT_LOGGING_PKG_LOG_API

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