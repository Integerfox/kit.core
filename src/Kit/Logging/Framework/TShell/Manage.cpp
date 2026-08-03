#if 0
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Write.h"
#include "Kit/Dm/IModelPoint.h"
#include "Kit/TShell/ICommand.h"
#include "Kit/Text/Strip.h"
#include "Kit/Text/Tokenizer/TextBlock.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Dm {
namespace TShell {


///////////////////////////
Kit::TShell::Result_T Write::execute( Kit::TShell::IContext& context, char* cmdString ) noexcept
{
    Kit::Text::IString& outtext  = context.getOutputBuffer();
    const char*         argument = Kit::Text::Strip::space( Kit::Text::Strip::notSpace( cmdString ) );

    // WRITE
    if ( *argument == '{' )
    {
        // Attempt to update the Model Point
        if ( !m_database.fromJSON( argument, &outtext ) )
        {
            context.writeFrame( outtext );
            return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
        }
        return Kit::TShell::Result_T::CMD_SUCCESS;
    }

    // TOUCH
    else
    {
        Kit::Text::Tokenizer::TextBlock tokens( cmdString );
        if ( tokens.numParameters() != 2 )
        {
            return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
        }

        // Look-up the model point
        Kit::Dm::IModelPoint* point = m_database.lookupModelPoint( tokens.getParameter( 1 ) );
        if ( point == nullptr )
        {
            outtext.format( "Model point not found: '%s'", tokens.getParameter( 1 ) );
            context.writeFrame( outtext );
            return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
        }

        // Call touch on the MP (to trigger change notification(s))
        point->touch();
        return Kit::TShell::Result_T::CMD_SUCCESS;
    }
}

} // end namespace
}
}
//------------------------------------------------------------------------------
#endif