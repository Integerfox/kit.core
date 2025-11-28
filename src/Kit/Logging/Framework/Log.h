#ifndef KIT_LOGGING_FRAMEWORK_LOG_H_
#define KIT_LOGGING_FRAMEWORK_LOG_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file declares the public facing Logging interface for generating log
    entries. However, Application or Package code SHOULD NOT directly include
    this file. Instead it should include their PACKAGE'S SPECIFIC public
    interface for logging.  For example, the Kit Package source code should
    include: Kit/Logging/Pkg/Log.h

    Logging has two run-time filters - one for Classification ID and one for Package
    ID. Only log entries that 'pass' BOTH filters are placed into the Log Entry
    FIFO.

    The interface IS thread safe, i.e Clients can call the vlogf() from any
    thread.
*/

#include "Kit/Logging/Framework/types.h"
#include <stdarg.h>

///
namespace Kit {
///
namespace Logging {
///
namespace Framework {


/*----------------------------------------------------------------------------*/
/** This method creates a log entry and adds it to the log entry queue. Log
    clients should NOT call this method directly, instead they should use their
    Package specific, type-safe logfXxxx() methods (see file comments above for
    more details)

    NOTE: The result code is provided for diagnostic purposes only. Clients
          are NOT expected to take any action based on the result code. The
          logging framework handles queue full conditions internally by generating
          special 'overflow' log entries. And resuming normal logging when
          space is available in the log entry queue.

    NOTE: The expectation is that EACH Package provides a type-safe (with
          respect to IDs) wrapper function that then calls this method.
 */
LogResult_T vlogf( uint8_t     classificationId,
                   uint8_t     packageId,
                   uint8_t     subSystemId,
                   uint8_t     messageId,
                   const char* formatInfoText,
                   va_list     ap ) noexcept;


}  // end namespaces
}
}
#endif  // end header latch
