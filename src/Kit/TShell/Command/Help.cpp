/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Help.h"
#include "Kit/Container/OrderedList.h"
#include "Kit/Text/Tokenizer/TextBlock.h"

using namespace Kit::Container;

//------------------------------------------------------------------------------
namespace Kit {
namespace TShell {
namespace Command {

static bool outputCmdHelp( Kit::TShell::IContext& context, Kit::TShell::ICommand& cmd, bool& io, bool includeDetails );
static bool outputLongText( Kit::TShell::IContext& context, bool& io, const char* text );

////////////////////////////////
Result_T Help::execute( IContext& context, char* cmdString ) noexcept
{
    Kit::Text::Tokenizer::TextBlock tokens( cmdString );

    // Error Checking
    if ( tokens.numParameters() > 2 )
    {
        return +Result_T::CMD_ERR_BAD_SYNTAX;
    }

    // Housekeeping
    bool        io     = true;
    ICommand*   cmdPtr = nullptr;
    const char* verb   = tokens.getParameter( 1 );

    // Command specific help
    if ( verb != nullptr && ( cmdPtr = context.findCommand( verb, strlen( verb ) ) ) )
    {
        if ( context.getSecurity().isAuthorized( cmdPtr->getRequiredPermissions(), cmdString, true ) )
        {
            outputCmdHelp( context, *cmdPtr, io, true );
        }
    }

    // List the commands
    else
    {
        // Housekeeping
        OrderedList<ICommand>& cmdList   = context.getCommands();
        bool                   extraHelp = ( tokens.numParameters() == 2 );

        // Walk the list of commands and output their help
        cmdPtr = cmdList.first();
        while ( cmdPtr && io == true )
        {
            if ( context.getSecurity().isAuthorized( cmdPtr->getRequiredPermissions(), cmdString, true ) )
            {
                outputCmdHelp( context, *cmdPtr, io, extraHelp );
            }
            cmdPtr = cmdList.next( *cmdPtr );
        }
    }

    return io ? Result_T::CMD_SUCCESS : Result_T::CMD_ERR_IO;
}


bool outputCmdHelp( Kit::TShell::IContext& context, Kit::TShell::ICommand& cmd, bool& io, bool includeDetails )
{
    outputLongText( context, io, cmd.getUsage() );
    if ( includeDetails )
    {
        const char* details = cmd.getHelp();
        if ( details && *details != '\0' )
        {
            outputLongText( context, io, details );
            io &= context.writeFrame( " " );
        }
    }
    return io;
}


bool outputLongText( Kit::TShell::IContext& context, bool& io, const char* text )
{
    Kit::Text::IString& singleLine = context.getOutputBuffer();
    singleLine.clear();

    // Output the text one line at a time because newline is the EOF framing character
    for ( ; text && *text != '\0' && io == true; text++ )
    {
        if ( *text == '\n' )
        {
            io &= context.writeFrame( singleLine );
            singleLine.clear();
        }
        else
        {
            singleLine += *text;
        }
    }

    if ( singleLine.length() > 0 )
    {
        io &= context.writeFrame( singleLine );
    }
    return io;
}

}  // end namespace
}
}
//------------------------------------------------------------------------------