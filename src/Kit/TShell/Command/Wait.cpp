/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Wait.h"
#include "Kit/Text/Tokenizer/TextBlock.h"
#include "Kit/Text/StringTo.h"
#include "Kit/System/Api.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace TShell {
namespace Command {


Result_T Wait::execute( IContext& context, char* cmdString ) noexcept
{
    Kit::Text::Tokenizer::TextBlock tokens( cmdString );
    Kit::Text::IString&             outtext = context.getOutputBuffer();

    // Error Checking
    if ( tokens.numParameters() != 2 )
    {
        return +Result_T::CMD_ERR_BAD_SYNTAX;
    }

    // Get the wait time
    uint32_t waitTime = 0;
    if ( Kit::Text::StringTo::unsignedInt( waitTime, tokens.getParameter( 1 ) ) )
    {
        outtext.format( "Waiting for %u msec ...", waitTime );
        bool io = context.writeFrame( outtext );
        Kit::System::sleep( waitTime );
        return io ? Result_T::CMD_SUCCESS : Result_T::CMD_ERR_IO;
    }

    // If I get here, the argument was bad
    return Result_T::CMD_ERR_BAD_SYNTAX;
}

}  // end namespace
}
}
//------------------------------------------------------------------------------