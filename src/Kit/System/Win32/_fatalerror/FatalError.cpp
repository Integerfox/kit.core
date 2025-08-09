/*-----------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/*
    Implementation of the System::FatalError interface using the standard
    C library.

    Notes:
        o The log messages are sent to stderr
        o The application is exited with an errorcode of OPTION_KIT_SYSTEM_FATAL_ERROR_EXIT_CODE
        o 'Extra Info' is limited to a '@@ Fatal Error:' prefix
        o The implementation is NOT thread safe.
*/

#include "Kit/System/FatalError.h"
#include "Kit/System/Shutdown.h"
#include <stdarg.h>
#include <stdio.h>


#define EXTRA_INFO "@@ Fatal Error: "

//------------------------------------------------------------------------------
namespace Kit {
namespace System {


//////////////////////////////
void FatalError::log( const char* message )
{
    fprintf( stderr, "\n%s%s\n", EXTRA_INFO, message );
    Shutdown::failure( OPTION_KIT_SYSTEM_FATAL_ERROR_EXIT_CODE );
}

void FatalError::log( const char* message, size_t value )
{
    fprintf( stderr, "\n%s%s [%p]\n", EXTRA_INFO, message, (void*) value );
    Shutdown::failure( OPTION_KIT_SYSTEM_FATAL_ERROR_EXIT_CODE );
}


void FatalError::logf( const char* format, ... )
{
    va_list ap;
    va_start( ap, format );

    fprintf( stderr, "\n%s", EXTRA_INFO );
    vfprintf( stderr, format, ap );
    fprintf( stderr, "\n" );
    Shutdown::failure( OPTION_KIT_SYSTEM_FATAL_ERROR_EXIT_CODE );
}


//////////////////////////////
void FatalError::logRaw( const char* message )
{
    log( message );
}

void FatalError::logRaw( const char* message, size_t value )
{
    log( message, value );
}



} // end namespaces
} 
//------------------------------------------------------------------------------
