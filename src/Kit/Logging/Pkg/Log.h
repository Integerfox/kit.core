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

#include "kit_config.h"
#include "Kit/Logging/Pkg/Package.h"
#include "Kit/Logging/Pkg/ClassificationId.h"
#include "Kit/Logging/Pkg/SubSystemId.h"
#include "Kit/Logging/Pkg/MsgId.h"
#include "Kit/Logging/Framework/Log.h"
#include "Kit/System/printfchecker.h"


/** Support Conditionally compiling out calls to the Logging framework.  This
    allows the KIT library to be used WITHOUT the Logging framework being
    implemented or mocked.  The default is to ENABLE the Logging APIs.
*/
#ifdef DISABLED_KIT_LOGGING_PKG_LOG_API
#define logfSystem( classificationId, messageId, ... ) ::Kit::Logging::Framework::LogResult_T::ADDED
#define logfDriver( classificationId, messageId, ... ) ::Kit::Logging::Framework::LogResult_T::ADDED
#else

///
namespace Kit {
///
namespace Logging {
///
namespace Pkg {


/*---------------------------------------------------------------------------*/
/// This method generates a SYSTEM Sub-system log entry
KIT_SYSTEM_PRINTF_CHECKER( 3, 4 )
inline Framework::LogResult_T logfSystem( ClassificationId catId, SystemMsgId msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    auto result = Framework::vlogf( catId, Package::PACKAGE_ID, SubSystemId::SYSTEM, msgId, msgTextFormat, ap );
    va_end( ap );
    return result;
}

/// This method generates a DRIVER Sub-system log entry
KIT_SYSTEM_PRINTF_CHECKER( 3, 4 )
inline Framework::LogResult_T logfDriver( ClassificationId catId, DriverMsgId msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    auto result = Framework::vlogf( catId, Package::PACKAGE_ID, SubSystemId::DRIVER, msgId, msgTextFormat, ap );
    va_end( ap );
    return result;
}

}  // end namespaces
}
}


#endif  // end DISABLED_KIT_LOGGING_PKG_LOG_API
#endif  // end header latch