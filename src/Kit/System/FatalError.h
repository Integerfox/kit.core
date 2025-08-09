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

/** This class defines methods for handling fatal errors encountered by
    an application.  The implementation of the methods is platform
    dependent.
 */
class FatalError
{
public:
    /** This function is used to process/log a FATAL error.  The supplied error
        message will be logged to a "storage media" along with other useful
        info such as the current task, stack dump, etc. In addition, THE
        APPLICATION AND/OR SYSTEM WILL BE "STOPPED".  Stopped can mean the
        application/system is exited, restarted, paused forever, etc.  The
        type of "storage media", additional info, stopped behavior, etc.
        is defined by the selected/linked implementation.

        NOTE: Applications, in general should NOT call this method - the
        application should be DESIGNED to handle and recover from errors that it
        encounters/detects.
     */
    static void log( const char* message );

    /** Same as above, but "value" is also logged.  This method allows additional
        information to be logged without resulting to a string formating call
        (which may not work since something really bad just happen).
     */
    static void log( const char* message, size_t value );

    /// Printf style formatted message
    static void logf( const char* format, ... );


public:
    /** Same as log(..) method, except NO "...other useful info
        such as current task,..." is logged, AND the "storage media" is
        restricted to 'media' that is ALWAYS available.

        This allows routines that are supplying the extra info OR routines that
        write to media to be able to log fatal errors WITHOUT creating a
        recursive death loop.
     */
    static void logRaw( const char* message );

    /// Same as log(..) method, except NO 'extra info' and restricted media
    static void logRaw( const char* message, size_t value );
};


}  // end namespaces
}
#endif  // end header latch
