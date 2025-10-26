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
#include "Kit/System/Trace.h"
#include "Kit/Text/btoa.h"
#include "Kit/Text/FString.h"
#include "FreeRTOS.h"
#include "task.h"

using namespace Kit::System;

#define EXTRA_INFO      "\n@@ Fatal Error: "
#define SIZET_SIZE      ((sizeof(size_t) / 4 ) * 10 + 1)

#ifndef CPL_SYSTEM_FREERTOS_FATAL_ERROR_BUFSIZE
#define CPL_SYSTEM_FREERTOS_FATAL_ERROR_BUFSIZE     128
#endif

static Kit::Text::FString<CPL_SYSTEM_FREERTOS_FATAL_ERROR_BUFSIZE> buffer_;


////////////////////////////////////////////////////////////////////////////////
void FatalError::log( const char* message )
{
    if ( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING )
    {
        Kit::Io::Output* ptr = Kit::System::Trace::getDefaultOutputStream_();

        ptr->write( EXTRA_INFO );
        ptr->write( message );
        ptr->write( "\n" );

        // Allow time for the error message to be outputted
        Kit::System::Api::sleep( 250 );
    }

    Shutdown::failure( OPTION_CPL_SYSTEM_FATAL_ERROR_EXIT_CODE );
}

void FatalError::log( const char* message, size_t value )
{
    if ( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING )
    {
        int              dummy = 0;
        Kit::Io::Output* ptr   = Kit::System::Trace::getDefaultOutputStream_();

        ptr->write( EXTRA_INFO );
        ptr->write( message );
        ptr->write( ". v:= " );
        ptr->write( Kit::Text::sizetToStr( value, buffer_.getBuffer( dummy ), SIZET_SIZE, 16 ) );
        ptr->write( "\n" );

        // Allow time for the error message to be outputted
        Kit::System::Api::sleep( 150 );
    }

    Shutdown::failure( OPTION_CPL_SYSTEM_FATAL_ERROR_EXIT_CODE );
}


void FatalError::logf( const char* format, ... )
{
    va_list ap;
    va_start( ap, format );

    if ( xTaskGetSchedulerState() == taskSCHEDULER_RUNNING )
    {
        buffer_ = EXTRA_INFO;
        buffer_.vformatAppend( format, ap );
        Kit::System::Trace::getDefaultOutputStream_()->write( buffer_ );
    }

    Shutdown::failure( OPTION_CPL_SYSTEM_FATAL_ERROR_EXIT_CODE );
}


////////////////////////////////////////////////////////////////////////////////
void FatalError::logRaw( const char* message )
{
    log( message );
}

void FatalError::logRaw( const char* message, size_t value )
{
    log( message, value );
}



