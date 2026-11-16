/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    Test application for Kit::System::ShutdownUnitTesting.  The interface's
    'forced exit' behavior can only be verified by an external process, i.e.
    the a.py script runs this application (once per scenario) and compares the
    application's actual exit code with the expected exit code that the
    application prints to stdout.

    usage: b.out failure|success|setexitcode
 */

#include "Kit/System/Api.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include <stdio.h>
#include <string.h>

#define MAX_ERRORS_ OPTION_KIT_SYSTEM_SHUTDOWN_UNI_TESTING_MAX_FATAL_ERRORS

// Exit codes for test-sequence errors (i.e. the application did not exit when expected)
#define ERR_BAD_ARGS_         200
#define ERR_UNEXPECTED_COUNT_ 201
#define ERR_DID_NOT_EXIT_     202

static void printExpectedExitCode( int exitCode )
{
    printf( "EXPECTED_EXIT_CODE=%d\n", exitCode );
    fflush( stdout );
}

static int shutdown( bool useSuccess )
{
    return useSuccess ? Kit::System::Shutdown::success() : Kit::System::Shutdown::failure( Kit::System::Shutdown::eDATA_MODEL );
}

int main( int argc, char* argv[] )
{
    Kit::System::initialize();
    if ( argc != 2 )
    {
        return ERR_BAD_ARGS_;
    }

    // An explicit exit code is used for the first shutdown call
    if ( strcmp( argv[1], "setexitcode" ) == 0 )
    {
        Kit::System::ShutdownUnitTesting::setExitCode( 7 );
        printExpectedExitCode( 7 );
        Kit::System::Shutdown::failure( Kit::System::Shutdown::eDATA_MODEL );
        return ERR_DID_NOT_EXIT_;
    }

    bool useSuccess = strcmp( argv[1], "success" ) == 0;
    if ( !useSuccess && strcmp( argv[1], "failure" ) != 0 )
    {
        return ERR_BAD_ARGS_;
    }

    // Below the limit -->the calls are counted, i.e. the application does NOT exit
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    for ( int i = 1; i < MAX_ERRORS_; i++ )
    {
        shutdown( useSuccess );
    }
    if ( Kit::System::ShutdownUnitTesting::getAndClearCounter() != MAX_ERRORS_ - 1 )
    {
        return ERR_UNEXPECTED_COUNT_;
    }

    // Reaching the limit -->forced exit with a FAILURE code
    printExpectedExitCode( useSuccess ? Kit::System::Shutdown::eFAILURE : Kit::System::Shutdown::eDATA_MODEL );
    for ( int i = 0; i < MAX_ERRORS_; i++ )
    {
        shutdown( useSuccess );
    }
    return ERR_DID_NOT_EXIT_;
}
