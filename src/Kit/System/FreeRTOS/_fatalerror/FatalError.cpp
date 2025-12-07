/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */
/*
    Implementation of the System::FatalError interface using the default Output
    stream from the Trace engine.  Side effect of this decision is even if
    Trace is not being used, the application must implement the
    Kit::System::Trace::getDefaultOutputStream_() method.

    Notes:
        o 'Extra Info' is limited to a '@@ Fatal Error:' prefix
        o The implementation is NOT thread safe.

*/

#include "Kit/System/FatalError.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"
#include "Kit/Text/ToString.h"
#include "Kit/Text/FString.h"
#include "Kit/Logging/Pkg/Log.h"
#include "FreeRTOS.h"
#include "task.h"

using namespace Kit::System;
using namespace Kit::Logging::Pkg;  // Helps with Log Enums

// Use Logging/Trace if available
#if defined( USE_KIT_SYSTEM_TRACE ) || defined( USE_KIT_SYSTEM_TRACE_RESTRICTED )
#define OUTPUT( text ) KIT_LOGGING_LOG_SYSTEM( ClassificationId::FATAL, SystemMsgId::FATAL_ERROR, "%s", text );
#else
#define OUTPUT( text ) Kit::System::Trace::getDefaultOutputStream_()->write( "\n" ); Kit::System::Trace::getDefaultOutputStream_()->write( text ); Kit::System::Trace::getDefaultOutputStream_()->write( "\n" );
#endif


#define EXTRA_INFO "@@ Fatal Error: "
#define SIZET_SIZE ( ( sizeof( size_t ) / 4 ) * 10 + 1 )

#ifndef KIT_SYSTEM_FREERTOS_FATAL_ERROR_BUFSIZE
#define KIT_SYSTEM_FREERTOS_FATAL_ERROR_BUFSIZE 128
#endif

static Kit::Text::FString<KIT_SYSTEM_FREERTOS_FATAL_ERROR_BUFSIZE> buffer_;

////////////////////////////////////////////////////////////////////////////////
void FatalError::log( int exitCode, const char* message )
{
    if ( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING )
    {
        buffer_  = EXTRA_INFO;
        buffer_ += message;
        OUTPUT( buffer_.getString() );

        // Allow time for the error message to be outputted
        Kit::System::sleep( 150 );
    }

    Shutdown::failure( exitCode );
}

void FatalError::log( int exitCode, const char* message, size_t value )
{
    if ( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING )
    {
        int dummy = 0;

        buffer_  = EXTRA_INFO;
        buffer_ += message;
        buffer_ += ". v:= ";
        buffer_ += Kit::Text::ToString::unsignedInt<size_t>( value, buffer_.getBuffer( dummy ), SIZET_SIZE, 16 );
        OUTPUT( buffer_.getString() );

        // Allow time for the error message to be outputted
        Kit::System::sleep( 150 );
    }

    Shutdown::failure( exitCode );
}


void FatalError::logf( int exitCode, const char* format, ... )
{
    va_list ap;
    va_start( ap, format );

    if ( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING )
    {
        buffer_ = EXTRA_INFO;
        buffer_.vformatAppend( format, ap );
        OUTPUT( buffer_.getString() );

        // Allow time for the error message to be outputted
        Kit::System::sleep( 150 );
    }
    va_end( ap );

    Shutdown::failure( exitCode );
}


////////////////////////////////////////////////////////////////////////////////
void FatalError::logRaw( int exitCode, const char* message )
{
    log( exitCode, message );
}

void FatalError::logRaw( int exitCode, const char* message, size_t value )
{
    log( exitCode, message, value );
}