/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "ShutdownUnitTesting.h"
#include "Kit/System/Mutex.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Private.h"
#include "Kit/Logging/Pkg/Log.h"

using namespace Kit::Logging::Pkg;


///
static size_t counter_  = 0;
static bool   counting_ = true;
static bool   testing_  = true;
static int    exitCode_ = 0;

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

////////////////////////////////////////////////////////////////////////////////
void ShutdownUnitTesting::clearAndUseCounter() noexcept
{
    Mutex::ScopeLock criticalSection( PrivateLocks::system() );
    counter_  = 0;
    counting_ = true;
    testing_  = true;
}

size_t ShutdownUnitTesting::getAndClearCounter() noexcept
{
    Mutex::ScopeLock criticalSection( PrivateLocks::system() );
    size_t temp = counter_;
    counter_    = 0;
    return temp;
}


void ShutdownUnitTesting::setExitCode( int newExitCode ) noexcept
{
    Mutex::ScopeLock criticalSection( PrivateLocks::system() );
    exitCode_ = newExitCode;
    counting_ = false;
    testing_  = true;
}


void ShutdownUnitTesting::restore() noexcept
{
    Mutex::ScopeLock criticalSection( PrivateLocks::system() );
    testing_ = false;
}


////////////////////////////////////////////////////////////////////////////////
static int preprocess_shutdown_( int exitCode, bool& trueExit ) noexcept
{
    Mutex::ScopeLock criticalSection( PrivateLocks::system() );

    if ( !testing_ )
    {
        trueExit = true;
    }
    else
    {
        if ( counting_ )
        {
            counter_++;
            trueExit = false;
        }
        else
        {
            trueExit = true;
            exitCode = exitCode_;
        }
    }

    return exitCode;
}

int Shutdown::success() noexcept
{
    bool trueExit = true;
    int  exitCode = preprocess_shutdown_( eSUCCESS, trueExit );
    KIT_LOGGING_LOG_SYSTEM( ClassificationId::WARNING, SystemMsgId::SHUTDOWN , "Orderly shutdown initiated. True exit=%d", trueExit );
    if ( trueExit )
    {
        exitCode = notifyShutdownHandlers( exitCode );
        exit( exitCode );
    }

    return exitCode;
}

int Shutdown::failure( int exitCode ) noexcept
{
    bool trueExit = true;
    exitCode      = preprocess_shutdown_( exitCode, trueExit );
    if ( trueExit )
    {
        exitCode = notifyShutdownHandlers( exitCode );
        exit( exitCode );
    }

    return exitCode;
}

} // end namespaces
}
//------------------------------------------------------------------------------