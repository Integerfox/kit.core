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
#include "hardware/watchdog.h"
#include "Kit/Logging/Pkg/Log.h"

using namespace Kit::Logging::Pkg;

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

////////////////////////////////////////////////////////////////////////////////
static int shutdown_application_( int exit_code )
{
    watchdog_enable( 1, 1 );
    while ( 1 );
    return exit_code;   // make compiler happy
}

int Shutdown::success() noexcept
{
    KIT_LOGGING_LOG_SYSTEM( ClassificationId::WARNING, SystemMsgId::SHUTDOWN , "Orderly shutdown initiated..." );
    return shutdown_application_( notifyShutdownHandlers( Shutdown::eSUCCESS ) );
}

int Shutdown::failure( int exit_code ) noexcept
{
    return shutdown_application_( notifyShutdownHandlers( exit_code ) );
}

} // end namespace
}
//------------------------------------------------------------------------------