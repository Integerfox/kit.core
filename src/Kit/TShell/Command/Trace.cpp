/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Trace.h"
#include "Kit/Text/Tokenizer/TextBlock.h"
#include "Kit/Text/Format.h"
#include "Kit/System/Trace.h"
#include "Kit/Text/Strip.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace TShell {
namespace Command {

static void dummy_( const char* f1, const char* f2, const char* f3, const char* f4 );

Result_T Trace::execute( IContext& context, char* cmdString ) noexcept
{
    Kit::Text::Tokenizer::TextBlock tokens( cmdString );
    Kit::Text::IString&             token    = context.getWorkBuffer();
    Kit::Text::IString&             outtext  = context.getOutputBuffer();
    unsigned                        numParms = tokens.numParameters();
    bool                            io       = true;

    // Do nothing if trace was not compiled in
    if ( KIT_SYSTEM_TRACE_IS_COMPILED() == false )
    {
        return writeLastFrame( context, "KIT Tracing was NOT ACTIVATED at Compiled time" );
    }

    // Output trace status
    if ( numParms == 1 )
    {
        // Output Enable sections
        io                &= context.writeFrame( " " );
        io                &= context.writeFrame( "TRACE: Currently Enabled Sections:" );
        io                &= context.writeFrame( "----------------------------------" );
        unsigned    count  = KIT_SYSTEM_TRACE_GET_SECTIONS( token );
        const char* ptr    = Kit::Text::Strip::space( token );
        while ( count-- )
        {
            const char* next  = Kit::Text::Strip::notSpace( ptr );
            io               &= context.writeFrame( ptr, next - ptr );
            ptr               = Kit::Text::Strip::space( next );
        }

        // Output Enable Thread Filters
        io    &= context.writeFrame( " " );
        io    &= context.writeFrame( "TRACE: Currently Enabled Thread Filters:" );
        io    &= context.writeFrame( "----------------------------------------" );
        count  = KIT_SYSTEM_TRACE_GET_THREAD_FILTERS( token );
        ptr    = Kit::Text::Strip::space( token );
        while ( count-- )
        {
            const char* next  = Kit::Text::Strip::notSpace( ptr );
            io               &= context.writeFrame( ptr, next - ptr );
            ptr               = Kit::Text::Strip::space( next );
        }

        // Runtime state
        outtext.format( "TRACE: Runtime state:= %s, Info Level:= %s", KIT_SYSTEM_TRACE_IS_ENABLED() ? "ENABLED" : "DISABLED", KIT_SYSTEM_TRACE_GET_INFO_LEVEL()._to_string() );
        io &= context.writeFrame( " " );
        return writeLastFrame( context, outtext, io );
    }


    // Global enable/disable
    if ( numParms == 2 && strcmp( tokens.getParameter( 1 ), "on" ) == 0 )
    {
        KIT_SYSTEM_TRACE_ENABLE();
        return Result_T::CMD_SUCCESS;
    }
    if ( numParms == 2 && strcmp( tokens.getParameter( 1 ), "off" ) == 0 )
    {
        KIT_SYSTEM_TRACE_DISABLE();
        return Result_T::CMD_SUCCESS;
    }


    // Relocate Trace output
    if ( numParms == 2 && strcmp( tokens.getParameter( 1 ), "here" ) == 0 )
    {
        // By definition if the trace command is being executed, then the TShell
        // is running and the Processor has been started.  Therefore, the returned
        // pointer is NOT null
        KIT_SYSTEM_TRACE_REDIRECT( *context.getOutputStream() );
        return Result_T::CMD_SUCCESS;
    }
    if ( numParms == 2 && strcmp( tokens.getParameter( 1 ), "revert" ) == 0 )
    {
        KIT_SYSTEM_TRACE_REVERT();
        return Result_T::CMD_SUCCESS;
    }


    // Change info level
    if ( numParms == 3 && strcmp( tokens.getParameter( 1 ), "level" ) == 0 )
    {
        auto maybe = Kit::System::TraceLevel::_from_string_nothrow( tokens.getParameter( 2 ) );
        if ( maybe )
        {
            KIT_SYSTEM_TRACE_SET_INFO_LEVEL( *maybe );
            return Result_T::CMD_SUCCESS;
        }
        else
        {
            return Result_T::CMD_ERR_BAD_SYNTAX;
        }
    }

    // Enable/Disable sections
    if ( numParms > 3 && strcmp( tokens.getParameter( 1 ), "section" ) == 0 )
    {
        numParms     -= 3;
        unsigned idx  = 3;
        if ( strcmp( tokens.getParameter( 2 ), "on" ) == 0 )
        {
            while ( numParms-- )
            {
                KIT_SYSTEM_TRACE_ENABLE_SECTION( tokens.getParameter( idx ) );
                idx++;
            }
        }
        else if ( strcmp( tokens.getParameter( 2 ), "off" ) == 0 )
        {
            while ( numParms-- )
            {
                KIT_SYSTEM_TRACE_DISABLE_SECTION( tokens.getParameter( idx ) );
                idx++;
            }
        }
        else
        {
            // If I get here -->the argument(s) where bad
            return Result_T::CMD_ERR_BAD_SYNTAX;
        }

        // Enable/disable trace sections WORKED!
        return Result_T::CMD_SUCCESS;
    }

    // Set/remove thread filters
    if ( numParms < 7 && strcmp( tokens.getParameter( 1 ), "threadfilters" ) == 0 )
    {
        const char* f1 = 0;
        const char* f2 = 0;
        const char* f3 = 0;
        const char* f4 = 0;

        // Get thread filters (max of 4)
        if ( numParms >= 3 )
        {
            f1 = tokens.getParameter( 2 );
        }
        if ( numParms >= 4 )
        {
            f2 = tokens.getParameter( 3 );
        }
        if ( numParms >= 5 )
        {
            f3 = tokens.getParameter( 4 );
        }
        if ( numParms >= 6 )
        {
            f4 = tokens.getParameter( 5 );
        }

        // Set thread filters
        if ( numParms != 2 )
        {
            KIT_SYSTEM_TRACE_SET_THREAD_4FILTERS( f1, f2, f3, f4 );
        }

        // Clear thread filters
        else
        {
            KIT_SYSTEM_TRACE_CLEAR_THREAD_FILTER();
        }

        // Klug:: Attempt to avoid unused-variable compiler warnings when TRACE is disable at compile time.
        dummy_( f1, f2, f3, f4 );
        return Result_T::CMD_SUCCESS;
    }


    // If I get here -->the argument(s) where bad
    return Result_T::CMD_ERR_BAD_SYNTAX;
}

void dummy_( const char* f1, const char* f2, const char* f3, const char* f4 )
{
}

}  // end namespace
}
}
//------------------------------------------------------------------------------