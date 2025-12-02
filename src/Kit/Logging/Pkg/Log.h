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

CLIENTS SHOULD NOT call the Kit::Logging::Pkg::logfXXX() method directly. Instead
the should use the KIT_LOGGING_LOG_XXX() macros.  This allows the application
to compile out the KIT library's usage of the Logging framework if desired.

The compile switch do 'disable' the Logging framework usage is:
    DISABLED_KIT_LOGGING_PKG_LOG_API

When this switch is defined the KIT_LOGGING_LOG_XXX() macros convert the logging
calls into KIT_SYSTEM_TRACE_RESTRICTED_MSG calls

NOTE: When the Application compiles out the Logging engine using the
      DISABLED_KIT_LOGGING_PKG_LOG_API switch - it must still provide the
      "logging configuration", i.e. the assignment absolute values for 
      classification and package IDs.
*/

#include "kit_config.h"
#include "Kit/Logging/Pkg/Package.h"
#include "Kit/Logging/Pkg/ClassificationId.h"
#include "Kit/Logging/Pkg/SubSystemId.h"
#include "Kit/Logging/Pkg/MsgId.h"
#include "Kit/Logging/Framework/Log.h"
#include "Kit/System/printfchecker.h"


// Support Conditionally compiling the Logging calls to Trace calls 
#ifdef DISABLED_KIT_LOGGING_PKG_LOG_API
#define KIT_LOGGING_LOG_SYSTEM( classificationId, messageId, ... ) KitLoggingFramework_logTracef( classificationId, ::Kit::Logging::Pkg::Package::PACKAGE_ID, ::Kit::Logging::Pkg::SubSystemId::SYSTEM, messageId, __VA_ARGS__ )
#define KIT_LOGGING_LOG_DRIVER( classificationId, messageId, ... ) KitLoggingFramework_logTracef( classificationId, ::Kit::Logging::Pkg::Package::PACKAGE_ID, ::Kit::Logging::Pkg::SubSystemId::DRIVER, messageId, __VA_ARGS__ )
#else

/** This method generates a SYSTEM Sub-system log entry
    @param classificationId               Classification ID of the log entry
    @param messageId                      Message ID of the log entry
    @param msgTextFormat                  Printf style format string for the log entry's info text
    @param ...                            Variable arguments for the format string

    @return Kit::Logging::Framework::LogResult_T
*/
#define KIT_LOGGING_LOG_SYSTEM( classificationId, messageId, ... ) ::Kit::Logging::Pkg::logfSystem( classificationId, messageId, __VA_ARGS__ )

/** This method generates a DRIVER Sub-system log entry
    @param classificationId               Classification ID of the log entry
    @param messageId                      Message ID of the log entry
    @param msgTextFormat                  Printf style format string for the log entry's info text
    @param ...                            Variable arguments for the format string

    @return Kit::Logging::Framework::LogResult_T
*/
#define KIT_LOGGING_LOG_DRIVER( classificationId, messageId, ... ) ::Kit::Logging::Pkg::logfDriver( classificationId, messageId, __VA_ARGS__ )

#endif  // end DISABLED_KIT_LOGGING_PKG_LOG_API

///
namespace Kit {
///
namespace Logging {
///
namespace Pkg {


/*---------------------------------------------------------------------------*/
/// This method generates a SYSTEM Sub-system log entry. Do not call directly, use the KIT_LOGGING_LOG_SYSTEM() macro
KIT_SYSTEM_PRINTF_CHECKER( 3, 4 )
inline Framework::LogResult_T logfSystem( ClassificationId catId, SystemMsgId msgId, const char* msgTextFormat, ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    auto result = Framework::vlogf( catId, Package::PACKAGE_ID, SubSystemId::SYSTEM, msgId, msgTextFormat, ap );
    va_end( ap );
    return result;
}

/// This method generates a DRIVER Sub-system log entry.Do not call directly, use the KIT_LOGGING_LOG_DRIVER() macro
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

#endif  // end header latch