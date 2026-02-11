/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */
#include "Kit/System/GlobalLock.h"
#include "Kit/System/Private.h"
#include "Kit/System/PrivateStartup.h"
#include "pico/sync.h"


static critical_section_t globalCritSec_;

namespace {

/// This class is to 'zero' the elapsed to the start of the application
class RegisterInitHandler_ : public Kit::System::IStartupHook
{
public:
    ///
    RegisterInitHandler_() :IStartupHook( SYSTEM ) {}


protected:
    ///
    void notify( InitLevel init_level ) noexcept override
    {
        critical_section_init( &globalCritSec_ );
    }
};

} // end namespace

///
static RegisterInitHandler_ autoRegister_systemInit_hook_;

//////////////////////////////////////////////////////////////////////////////
void Kit::System::GlobalLock::begin( void )
{
    critical_section_enter_blocking( &globalCritSec_ );
}

void Kit::System::GlobalLock::end( void )
{
    critical_section_exit( &globalCritSec_ );
}



