#ifndef Cpl_Logging_Private_h_
#define Cpl_Logging_Private_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file contains private (i.e. the application SHOULD never call this
    method directory for the logging framework
*/

#include <stdint.h>
#include <stdarg.h>

///
namespace Cpl {
///
namespace Logging {


/** This method is used to create the log entry and insert into the entry queue
 */
void createAndAddLogEntry_( uint32_t    category, 
                            const char* catIdText, 
                            uint16_t    msgId, 
                            const char* msgIdText, 
                            const char* format, 
                            va_list     ap ) noexcept;



};      // end namespaces
};
#endif  // end header latch
