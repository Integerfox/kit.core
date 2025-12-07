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
#include "Kit/Logging/Pkg/Log.h"
#include <stdlib.h>

using namespace Kit::Logging::Pkg;

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
    KIT_LOGGING_LOG_SYSTEM( ClassificationId::WARNING, SystemMsgId::SHUTDOWN , "Orderly shutdown initiated..." );
    return shutdownApplication_( notifyShutdownHandlers( Shutdown::eSUCCESS ) );
}

int Shutdown::failure( int exitCode ) noexcept
{
    return shutdownApplication_( notifyShutdownHandlers( exitCode ) );
}


} // end namespaces
}
//------------------------------------------------------------------------------