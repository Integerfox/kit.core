/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file 

    This file provides the implementation for the "realTime" elapsed time
    interface.  Typically only "simulator platforms" (e.g. Linux) implement
    these methods, i.e. it does not make sense to implement simulated time on
    microcontroller platforms.
*/

#include "PrivateElapsedTime.h"
#include "ElapsedTime.h"
#include "PrivateStartup.h"
#include <stdlib.h>


//------------------------------------------------------------------------------
namespace Kit {
namespace System {

static uint32_t startMsec_;

// an anonymous namespace for internal linkage
namespace {

/// This class is to 'zero' the elapsed to the start of the application
class RegisterInitHandler_ : public Kit::System::IStartupHook
{
public:
    ///
    RegisterInitHandler_() noexcept
        : IStartupHook( IStartupHook::SYSTEM ) {}


protected:
    /** No lock is needed because 'startMsec_' is written once, during start-up,
        before any other thread exists
     */
    void notify( InitLevel init_level ) noexcept override
    {
        startMsec_ = KitSystemElapsedTime_getTimeInMilliseconds();
    }
};
}  // end anonymous namespace

///
static RegisterInitHandler_ autoRegister_systemInit_hook_;


///////////////////////////////////////////////////////////////
/* NO LOCK. This method is called on every pass of every event loop, including
   by threads that already hold an application mutex.  Nesting the KIT system
   mutex inside an application mutex chains the two priority-inheritance scopes
   together, which boosts/holds priorities far longer than the application
   intended.

    The method is a read plus an unsigned subtraction.  The 32 bit roll-over is
    by design: deltaMilliseconds() relies on it.
 */
uint32_t ElapsedTime::millisecondsInRealTime() noexcept
{
    return KitSystemElapsedTime_getTimeInMilliseconds() - startMsec_;
}

} // end namespaces
}
//------------------------------------------------------------------------------