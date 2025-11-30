#ifndef FOO_LOGGING_PKG_LOG_H_
#define FOO_LOGGING_PKG_LOG_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file 

This file declares the Logging functions available to the FOO Logging Domain.

*/

#include "Kit/Logging/Framework/_0test/_3pkgs/Foo/Logging/Pkg/Package.h"
#include "Kit/Logging/Framework/_0test/_3pkgs/Foo/Logging/Pkg/ClassificationId.h"
#include "Kit/Logging/Framework/_0test/_3pkgs/Foo/Logging/Pkg/SubSystemId.h"
#include "Kit/Logging/Framework/_0test/_3pkgs/Foo/Logging/Pkg/MsgId.h"
#include "Kit/Logging/Framework/Log.h"
#include "Kit/System/printfchecker.h"

///
namespace Foo {
///
namespace Logging {
///
namespace Pkg {


/*---------------------------------------------------------------------------*/
/// This method generates a UI Sub-system log entry
KIT_SYSTEM_PRINTF_CHECKER(3, 4)
inline Kit::Logging::Framework::LogResult_T logfUi( ClassificationId catId, UiMsgId msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    auto result = Kit::Logging::Framework::vlogf( catId, Package::PACKAGE_ID, SubSystemId::UI, msgId, msgTextFormat, ap );
    va_end( ap );
    return result;
}

/// This method generates a DATABASE Sub-system log entry
KIT_SYSTEM_PRINTF_CHECKER(3, 4)
inline Kit::Logging::Framework::LogResult_T logfDatabase( ClassificationId catId, DatabaseMsgId msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    auto result = Kit::Logging::Framework::vlogf( catId, Package::PACKAGE_ID, SubSystemId::DATABASE, msgId, msgTextFormat, ap );
    va_end( ap );
    return result;
}

/// This method generates a API Sub-system log entry
KIT_SYSTEM_PRINTF_CHECKER(3, 4)
inline Kit::Logging::Framework::LogResult_T logfApi( ClassificationId catId, ApiMsgId msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    auto result = Kit::Logging::Framework::vlogf( catId, Package::PACKAGE_ID, SubSystemId::API, msgId, msgTextFormat, ap );
    va_end( ap );
    return result;
}

}      // end namespaces
}
}
#endif  // end header latch