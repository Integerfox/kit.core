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
#include "Private.h"
#include "PrivateStartup.h"
#include <stdlib.h>


//------------------------------------------------------------------------------
namespace Kit {
namespace System {

static uint64_t elapsedMsec_;
static uint64_t lastMsec_;

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
uint64_t ElapsedTime::millisecondsInRealTimeEx() noexcept
{
    Kit::System::Mutex::ScopeLock lock( Kit::System::PrivateLocks::system() );

    uint64_t newTime  = KitSystemElapsedTime_getTimeInMilliseconds();
    uint64_t delta    = newTime - lastMsec_;
    lastMsec_         = newTime;
    elapsedMsec_     += delta;
    return elapsedMsec_;
}


} // end namespaces
}
//------------------------------------------------------------------------------