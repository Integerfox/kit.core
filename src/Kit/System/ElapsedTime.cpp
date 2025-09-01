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
#include "Private.h"
#include "PrivateStartup.h"
#include <stdlib.h>


//------------------------------------------------------------------------------
namespace Kit {
namespace System {

static uint32_t elapsedMsec_;
static uint32_t lastMsec_;

// an anonymous namespace for internal linkage
namespace {

/// This class is to 'zero' the elapsed to the start of the application
class RegisterInitHandler_ : public Kit::System::StartupHook
{
public:
    ///
    RegisterInitHandler_()
        : StartupHook( StartupHook::SYSTEM ) {}


protected:
    ///
    void notify( InitLevel_e init_level ) override
    {
        elapsedMsec_ = 0;
        lastMsec_    = KitSystemElapsedTime_getTimeInMilliseconds();
    }
};
}  // end anonymous namespace

///
static RegisterInitHandler_ autoRegister_systemInit_hook_;


///////////////////////////////////////////////////////////////
uint32_t ElapsedTime::millisecondsInRealTime( void ) noexcept
{
    Kit::System::Mutex::ScopeLock lock( Kit::System::PrivateLocks::system() );

    uint32_t newTime  = KitSystemElapsedTime_getTimeInMilliseconds();
    uint32_t delta    = newTime - lastMsec_;
    lastMsec_         = newTime;
    elapsedMsec_     += delta;
    return elapsedMsec_;
}

} // end namespaces
}
//------------------------------------------------------------------------------