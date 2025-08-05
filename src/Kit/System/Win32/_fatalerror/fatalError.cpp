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
        o 'Extra Info' is limited to a '@@ Fatal Error:' prefix
*/

#include "Kit/System/fatalError.h"
#include "Kit/System/Shutdown.h"
#include <stdarg.h>
#include <stdio.h>


#define EXTRA_INFO "@@ Fatal Error: "

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

void fatalError( const char* message, size_t value1, size_t value2, size_t value3 ) noexcept
{
    // Log the error message
    fprintf( stderr, "\n%s%s [%zu, %zu, %zu]\n", EXTRA_INFO, message, value1, value2, value3 );

    // Shutdown the application
    Shutdown::failure( OPTION_KIT_SYSTEM_FATAL_ERROR_EXIT_CODE );
}


} // end namespaces
} 
//------------------------------------------------------------------------------
