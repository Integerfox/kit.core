#ifndef Cpl_System_x_testsupport_Shutdown_TS_h_
#define Cpl_System_x_testsupport_Shutdown_TS_h_
/*-----------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include <stdlib.h>

/** Maximum number of allowed fatal errors before the application is
    forced to exit.  This is a safety mechanism to prevent the unit test
    from getting into a 'runaway' error condition.

    Note: The fatal error count is reset everytime getAndClearCounter()
          is called.
 */
#ifndef OPTION_KIT_SYSTEM_SHUTDOWN_TS_MAX_FATAL_ERRORS
#define OPTION_KIT_SYSTEM_SHUTDOWN_TS_MAX_FATAL_ERRORS 10
#endif


///
namespace Kit {
///
namespace System {


/** This class provides methods to access the unit testing support
    for the Kit::System::Shutdown interface.

    This class provides an implementation of Shutdown interface that
    has the ability to count the number times the Shutdown interface
    is/was called instead of exiting.  It can also alter the application's
    exit code.

    Notes:
        o The Shutdown implementation assumes/uses the C Library 'exit()'
          method.
 */
class ShutdownUnitTesting
{
public:
    /** This method zero's the exit counter and sets the shutdown behavior
        to NOT exit when called - only count the calls.  Note: This
        interface starts in the 'counter mode' with the count set to
        zero.
     */
    static void clearAndUseCounter() noexcept;

    /** This method returns the current call count AND clears the
        call counter.
     */
    static size_t getAndClearCounter() noexcept;


public:
    /** This method allows the application to exit - but always with
        the specified 'new_exit_code' (i.e. ignoring the application
        supplied exit code).  This method is helpful during unit testing
        for 'passing' and exit/failure use case.
     */
    static void setExitCode( int new_exit_code ) noexcept;

    /** This method restores the exit/shutdown behavior to its 'non-test'
        paradigm.
     */
    static void restore() noexcept;
};


}  // end namespaces
}
#endif  // end header latch
