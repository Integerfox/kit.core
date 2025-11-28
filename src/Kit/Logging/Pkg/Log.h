#ifndef KIT_LOGGING_PKG_LOG_H_
#define KIT_LOGGING_PKG_LOG_H_
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

#include "Kit/Logging/Pkg/Package.h"
#include "Kit/Logging/Pkg/ClassificationId.h"
#include "Kit/Logging/Pkg/SubSystemId.h"
#include "Kit/Logging/Pkg/SystemMsgId.h"
#include "Kit/Logging/Pkg/DriverMsgId.h"
#include "Kit/Logging/Framework/Log.h"
#include "Kit/System/printfchecker.h"

///
namespace Kit {
///
namespace Logging {
///
namespace Pkg {


/*---------------------------------------------------------------------------*/
/// This method generates a SYSTEM Sub-system log entry
KIT_SYSTEM_PRINTF_CHECKER(4, 5)
inline Framework::LogResult_T logfSystem( ClassificationId catId, SubSystemId subSysId, SystemMsgId msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    auto result = Framework::vlogf( catId, Package::PACKAGE_ID, subSysId, msgId, msgTextFormat, ap );
    va_end( ap );
    return result;
}

/// This method generates a DRIVER Sub-system log entry
KIT_SYSTEM_PRINTF_CHECKER(4, 5)
inline Framework::LogResult_T logfDriver( ClassificationId catId, SubSystemId subSysId, DriverMsgId msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    auto result = Framework::vlogf( catId, Package::PACKAGE_ID, subSysId, msgId, msgTextFormat, ap );
    va_end( ap );
    return result;
}


}      // end namespaces
}
}
#endif  // end header latch