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
#include <stdlib.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace System {


static int shutdownApplication_( int exitCode ) noexcept
{
    exit( exitCode );
    return exitCode;
}

int Shutdown::success() noexcept
{
    return shutdownApplication_( notifyShutdownHandlers( OPTION_KIT_SYSTEM_SHUTDOWN_SUCCESS_ERROR_CODE ) );
}

int Shutdown::failure( int exitCode ) noexcept
{
    return shutdownApplication_( notifyShutdownHandlers( exitCode ) );
}


} // end namespaces
}
//------------------------------------------------------------------------------