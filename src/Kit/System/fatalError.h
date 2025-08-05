#ifndef KIT_SYSTEM_FATAL_ERROR_H_
#define KIT_SYSTEM_FATAL_ERROR_H_
/*-----------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

This file defines methods for handling fatal errors encountered by an
application.  The implementation of the methods is platform dependent.

*/


#include "kit_config.h"
#include <stdlib.h>

/** Specifies the default value used for the application exit code when
    terminating due to a fatal error.
 */
#ifndef OPTION_KIT_SYSTEM_FATAL_ERROR_EXIT_CODE
#define OPTION_KIT_SYSTEM_FATAL_ERROR_EXIT_CODE 2
#endif


///
namespace Kit {
///
namespace System {

/** This function is used to process a FATAL error.  The supplied error
    message (with optional values) will be logged to a "storage media" along
    with other useful info such as the current task, stack dump, etc. In
    addition, THE APPLICATION AND/OR SYSTEM WILL BE "STOPPED".  Stopped can
    mean the application/system is exited, restarted, paused forever, etc.
    The type of "storage media", additional info, stopped behavior, etc.
    is defined by the selected/linked implementation.

    NOTE: Applications, in general should NOT call this method - the
    application should be DESIGNED to handle and recover from errors that it
    encounters/detects.
 */
void fatalError( const char* message, size_t val1 = 0, size_t val2 = 0, size_t val3 = 0 ) noexcept;


}  // end namespaces
}
#endif  // end header latch
