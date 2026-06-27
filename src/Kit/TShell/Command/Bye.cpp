/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Bye.h"
#include "Kit/Text/Tokenizer/TextBlock.h"
#include "Kit/Text/StringTo.h"
#include "Kit/System/Shutdown.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace TShell {
namespace Command {


Result_T Bye::execute( IContext& context, char* cmdString ) noexcept
{
    Kit::Text::Tokenizer::TextBlock tokens( cmdString,
                                            context.getDelimiterChar(),
                                            context.getTerminatorChar(),
                                            context.getQuoteChar(),
                                            context.getEscapeChar() );
    Kit::Text::IString&             outtext = context.getOutputBuffer();

    // Basic Error Checking
    if ( tokens.numParameters() > 2 )
    {
        return +Result_T::CMD_ERR_BAD_SYNTAX;
    }

    // Exit the TShell only
    if ( tokens.numParameters() == 2 )
    {
        if ( tokens.getParameter( 1 )[0] == 't' )
        {
            context.requestTShellExit();
            return Result_T::CMD_SUCCESS;
        }
    }

    // Exit the application
    int exitCode = Kit::System::Shutdown::eSUCCESS;
    if ( tokens.numParameters() == 2 )
    {
        if ( !Kit::Text::StringTo::signedInt( exitCode, tokens.getParameter( 1 ) ) )
        {
            outtext.format( "Invalid exit code: '%s'\n", tokens.getParameter( 1 ) );
            context.writeFrame( outtext );
            return Result_T::CMD_ERR_BAD_SYNTAX;
        }
    }

    Kit::System::Shutdown::failure( exitCode );
    return Result_T::CMD_SUCCESS;
}

}  // end namespace
}
}
//------------------------------------------------------------------------------