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
#include "Kit/System/Shutdown.h"



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
    // TODO: FIX ME
    //Locks_::system().lock();
    counter_  = 0;
    counting_ = true;
    testing_  = true;
    //Locks_::system().unlock();
}

size_t ShutdownUnitTesting::getAndClearCounter() noexcept
{
    // TODO: FIX ME
    //Locks_::system().lock();
    size_t temp = counter_;
    counter_    = 0;
    //Locks_::system().unlock();

    return temp;
}


void ShutdownUnitTesting::setExitCode( int newExitCode ) noexcept
{
    // TODO: FIX ME
    //Locks_::system().lock();
    exitCode_ = newExitCode;
    counting_ = false;
    testing_  = true;
    //Locks_::system().unlock();
}


void ShutdownUnitTesting::restore() noexcept
{
    // TODO: FIX ME
    //Locks_::system().lock();
    testing_ = false;
    //Locks_::system().unlock();
}


////////////////////////////////////////////////////////////////////////////////
static int preprocess_shutdown_( int exitCode, bool& trueExit ) noexcept
{
    // TODO: FIX ME
    //Locks_::system().lock();

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

    //Locks_::system().unlock();
    return exitCode;
}

int Shutdown::success() noexcept
{
    bool trueExit = true;
    int  exitCode = preprocess_shutdown_( OPTION_KIT_SYSTEM_SHUTDOWN_SUCCESS_ERROR_CODE, trueExit );
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