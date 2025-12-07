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
#include "Kit/Logging/Pkg/Log.h"
#include "Kit/Text/FString.h"
#include <stdarg.h>
#include <stdio.h>


#define EXTRA_INFO "@@ Fatal Error"

using namespace Kit::Logging::Pkg;  // Helps with Log Enums

// Use Logging/Trace if available
#if defined( USE_KIT_SYSTEM_TRACE ) || defined( USE_KIT_SYSTEM_TRACE_RESTRICTED )
#define OUTPUT( text ) KIT_LOGGING_LOG_SYSTEM( ClassificationId::FATAL, SystemMsgId::FATAL_ERROR, "%s", text );
#else
#define OUTPUT( text ) fprintf( stderr, "\n%s\n", text )
#endif


//------------------------------------------------------------------------------
namespace Kit {
namespace System {

#ifndef KIT_SYSTEM_FATAL_ERROR_BUFSIZE
#define KIT_SYSTEM_FATAL_ERROR_BUFSIZE 256
#endif

static Kit::Text::FString<KIT_SYSTEM_FATAL_ERROR_BUFSIZE> buffer_;


//////////////////////////////
void FatalError::log( int exitCode, const char* message )
{
    buffer_.format( "%s [%d]: %s", EXTRA_INFO, exitCode, message );
    OUTPUT( buffer_.getString() );
    Shutdown::failure( exitCode );
}

void FatalError::log( int exitCode, const char* message, size_t value )
{
    buffer_.format( "%s [%d]: %s [%zu]", EXTRA_INFO, exitCode, message, value );
    OUTPUT( buffer_.getString() );
    Shutdown::failure( exitCode );
}


void FatalError::logf( int exitCode, const char* format, ... )
{
    va_list ap;
    va_start( ap, format );

    buffer_.format( "%s [%d]: ", EXTRA_INFO, exitCode );
    buffer_.vformatAppend( format, ap );
    va_end( ap );
    OUTPUT( buffer_.getString() );
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
