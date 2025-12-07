/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/System/Shutdown.h"
#include "Kit/Bsp/Api.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace System {

////////////////////////////////////////////////////////////////////////////////
static int shutdown_application_( int exit_code )
{
    // Perform a 'hard' shutdown by entering an infinite loop
    Bsp_disable_irqs();
    for ( ;; )
        ;

    // Never actually returns -->needed to satisfy the compiler
    return exit_code;
}

int Shutdown::success() noexcept
{
    return shutdown_application_( notifyShutdownHandlers( Shutdown::eSUCCESS ) );
}

int Shutdown::failure( int exit_code ) noexcept
{
    return shutdown_application_( notifyShutdownHandlers( exit_code ) );
}

}  // end namespace
}
//------------------------------------------------------------------------------