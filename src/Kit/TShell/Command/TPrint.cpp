/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "TPrint.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/Text/Tokenizer/TextBlock.h"
#include "Kit/Text/Format.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace TShell {
namespace Command {


Result_T TPrint::execute( IContext& context, char* cmdString ) noexcept
{
    Kit::Text::Tokenizer::TextBlock tokens( cmdString,
                                            context.getDelimiterChar(),
                                            context.getTerminatorChar(),
                                            context.getQuoteChar(),
                                            context.getEscapeChar() );
    Kit::Text::IString&             outtext = context.getOutputBuffer();

    // Error Checking
    if ( tokens.numParameters() > 2 )
    {
        return +Result_T::CMD_ERR_BAD_SYNTAX;
    }

    // Generate the output string
    Kit::Text::Format::timestamp( outtext, Kit::System::ElapsedTime::milliseconds() );
    if ( tokens.numParameters() == 2 )
    {
        outtext.formatAppend( " %s\n", tokens.getParameter( 1 ) );
    }
 
    // Output the final string
    return writeLastFrame( context, outtext );
}

}  // end namespace
}
}
//------------------------------------------------------------------------------