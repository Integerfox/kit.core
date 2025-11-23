#ifndef KIT_LOGGING_API_LOG_H_
#define KIT_LOGGING_API_LOG_H_
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

This file declares the Logging functions available to the KIT Logging Domain.

*/

#include "Kit/Logging/Api/CategoryId.h"
#include "Kit/Logging/Api/OsalMsg.h"
#include "Kit/System/printfchecker.h"


///
namespace Kit {
///
namespace Logging {
///
namespace Api {


/*---------------------------------------------------------------------------*/
/// This method generates a OSAL Sub-system log entry
KIT_SYSTEM_PRINTF_CHECKER(3, 4)
void logfOsal( CategoryId catId, OsalMsg msgCode, const char* msgTextFormat, ... ) noexcept;


}      // end namespaces
}
}
#endif  // end header latch
