/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/Shell.h"
#include "Kit/System/Trace.h"
#include "Kit/Text/DString.h"
#include <stdlib.h> 

#define MY_STDOUT_NULL_ " 1> /dev/null"
#define MY_STDERR_NULL_ " 2> /dev/null"

#define SECT_ "Shell"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

bool Shell::isAvailable() noexcept
{
    return system( 0 ) != 0; // The system() method returns non-zero when a valid command interpreter/shell is found/available
}


int Shell::execute( const char* cmdstring, bool noEchoStdOut, bool noEchoStdErr ) noexcept
{
    // Create a copy of command-string and allocate extra space for redirecting the output
    Kit::Text::DString cmd( cmdstring, strlen( cmdstring ) + strlen( MY_STDOUT_NULL_ ) + strlen( MY_STDERR_NULL_ ) );

    // Redirect STDOUT to the null device when requested (abort the shell command if there is an error)
    if ( noEchoStdOut )
    {
        cmd += MY_STDOUT_NULL_;
        if ( cmd.truncated() )
        {
            return -1;
        }
    }

    // Redirect STDERR to the null device when requested (abort the shell command if there is an error)
    if ( noEchoStdErr )
    {
        cmd += MY_STDERR_NULL_;
        if ( cmd.truncated() )
        {
            return -1;
        }
    }

    // Execute the command
    KIT_SYSTEM_TRACE_MSG( SECT_, "execute(\"%s\")", cmd.getString() );
    return system( cmd() );
}

} // end namespace
}
//------------------------------------------------------------------------------