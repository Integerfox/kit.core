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

///
namespace PkgZ {
///
namespace Logging {
///
namespace Pkg {


/*---------------------------------------------------------------------------*/
/// This method generates a CORE Sub-system log entry
KIT_SYSTEM_PRINTF_CHECKER(3, 4)
inline Kit::Logging::Framework::LogResult_T logfCore( ClassificationId catId, CoreMsgId msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    auto result = Kit::Logging::Framework::vlogf( catId, Package::PACKAGE_ID, SubSystemId::CORE, msgId, msgTextFormat, ap );
    va_end( ap );
    return result;
}

/// This method generates a NETWORK Sub-system log entry
KIT_SYSTEM_PRINTF_CHECKER(3, 4)
inline Kit::Logging::Framework::LogResult_T logfNetwork( ClassificationId catId, NetworkMsgId msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    auto result = Kit::Logging::Framework::vlogf( catId, Package::PACKAGE_ID, SubSystemId::NETWORK, msgId, msgTextFormat, ap );
    va_end( ap );
    return result;
}

/// This method generates a STORAGE Sub-system log entry
KIT_SYSTEM_PRINTF_CHECKER(3, 4)
inline Kit::Logging::Framework::LogResult_T logfStorage( ClassificationId catId, StorageMsgId msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    auto result = Kit::Logging::Framework::vlogf( catId, Package::PACKAGE_ID, SubSystemId::STORAGE, msgId, msgTextFormat, ap );
    va_end( ap );
    return result;
}

}      // end namespaces
}
}
#endif  // end header latch