/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Echo.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/Text/Tokenizer/TextBlock.h"
#include "Kit/Text/Format.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace TShell {
namespace Command {


Result_T Echo::execute( IContext& context, char* cmdString ) noexcept
{
    Kit::Text::Tokenizer::TextBlock tokens( cmdString );
    Kit::Text::IString&             outtext = context.getOutputBuffer();

    // Error Checking
    if ( tokens.numParameters() > 3 )
    {
        return +Result_T::CMD_ERR_BAD_SYNTAX;
    }
    else if ( tokens.numParameters() == 3 && tokens.getParameter( 1 )[0] != '-' )
    {
        return +Result_T::CMD_ERR_BAD_SYNTAX;
    }

    // Generate the output string
    if ( tokens.numParameters() == 1 || ( tokens.numParameters() == 2 && tokens.getParameter( 1 )[0] != '-' ) )
    {
        Kit::Text::Format::timestamp( outtext, Kit::System::ElapsedTime::milliseconds() );
    }
    if ( tokens.numParameters() == 2 && tokens.getParameter( 1 )[0] != '-' )
    {
        outtext.formatAppend( " %s", tokens.getParameter( 1 ) );
    }
    else if ( tokens.numParameters() == 3 )
    {
        outtext.formatAppend( "%s", tokens.getParameter( 2 ) );
    }

    // Output the final string
    return writeLastFrame( context, outtext );
}

}  // end namespace
}
}
//------------------------------------------------------------------------------