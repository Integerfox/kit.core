/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file 

    This file provides the implementation for the "extended/64bit" elapsed
    time interface.  It use the 32bit interface/counter to derive the 64 bit 
    counter
*/

#include "PrivateElapsedTime.h"
#include "ElapsedTime.h"
#include "GlobalLock.h"
#include "PrivateStartup.h"
#include <stdlib.h>


//------------------------------------------------------------------------------
namespace Kit {
namespace System {

static uint64_t elapsedMsec_;
static uint32_t lastMsec_;

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
    ///
    void notify( InitLevel init_level ) noexcept override
    {
        elapsedMsec_ = 0;
        lastMsec_    = KitSystemElapsedTime_getTimeInMilliseconds();
    }
};

}  // end anonymous namespace

///
static RegisterInitHandler_ autoRegister_systemInit_hook_;


///////////////////////////////////////////////////////////////
/* A GlobalLock - instead of the KIT system mutex - is used to guard the 32-to-64
   bit extension.  On an RTOS/bare-metal target a GlobalLock is a critical section:
   a handful of instructions that never block and never alter thread priorities.
   The system mutex here would nest inside whatever application mutex the caller
   already holds, chaining their priority-inheritance scopes together.  On a Host
   platform a GlobalLock is a recursive Mutex -->this method is re-entrant there.

   See Kit/System/ElapsedTime.h for the full set of caller constraints.
 */
uint64_t ElapsedTime::millisecondsInRealTimeEx() noexcept
{
    GlobalLock::begin();

    // The delta is computed in 32 bits so that the unsigned roll-over yields the true delta
    uint32_t newTime  = KitSystemElapsedTime_getTimeInMilliseconds();
    elapsedMsec_     += newTime - lastMsec_;
    lastMsec_         = newTime;
    uint64_t result   = elapsedMsec_;

    GlobalLock::end();
    return result;
}


} // end namespaces
}
//------------------------------------------------------------------------------