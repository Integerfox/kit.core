/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    Implementation of the System::FatalError interface using the standard
    C library.

    Notes:
        o The log messages are sent to stderr
        o 'Extra Info' is limited to a '@@ Fatal Error:' prefix
        o The implementation is NOT thread safe.
*/

#include "Kit/System/FatalError.h"
#include "Kit/System/Shutdown.h"
#include <stdarg.h>
#include <stdio.h>


#define EXTRA_INFO "@@ Fatal Error"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {


//////////////////////////////
void FatalError::log( int exitCode, const char* message )
{
    fprintf( stderr, "\n%s [%d]: %s\n", EXTRA_INFO, exitCode, message );
    Shutdown::failure( exitCode );
}

void FatalError::log( int exitCode, const char* message, size_t value )
{
    fprintf( stderr, "\n%s [%d]: %s [%p]\n", EXTRA_INFO, exitCode, message, (void*)value );
    Shutdown::failure( exitCode );
}


void FatalError::logf( int exitCode, const char* format, ... )
{
    va_list ap;
    va_start( ap, format );

    fprintf( stderr, "\n%s [%d]: ", EXTRA_INFO, exitCode );
    vfprintf( stderr, format, ap );
    fprintf( stderr, "\n" );
    Shutdown::failure( exitCode );
}


//////////////////////////////
void FatalError::logRaw( int exitCode, const char* message )
{
    log( exitCode, message );
}

void FatalError::logRaw( int exitCode, const char* message, size_t value )
{
    log( exitCode, message, value );
}


}  // end namespaces
}
//------------------------------------------------------------------------------
