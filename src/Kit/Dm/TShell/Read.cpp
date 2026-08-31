/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Read.h"
#include "Kit/Dm/IModelPoint.h"
#include "Kit/Text/Tokenizer/TextBlock.h"
#include <string.h>

//------------------------------------------------------------------------------
namespace Kit {
namespace Dm {
namespace TShell {


///////////////////////////
Kit::TShell::Result_T Read::execute( Kit::TShell::IContext& context, char* cmdString ) noexcept
{
    Kit::Text::Tokenizer::TextBlock tokens( cmdString );
    Kit::Text::IString&             outtext  = context.getOutputBuffer();
    unsigned                        numParms = tokens.numParameters();

    // LIST
    if ( numParms == 1 || numParms == 3 )
    {
        // Is there filter?
        const char* filter = numParms < 3 ? nullptr : tokens.getParameter( 2 );

        // Walk the Model database
        Kit::Dm::IModelPoint* point = m_database.getFirstByName();
        while ( point )
        {
            if ( filter == nullptr || strstr( point->getName(), filter ) != 0 )
            {
                if ( !context.writeFrame( point->getName() ) )
                {
                    return Kit::TShell::Result_T::CMD_ERR_IO;
                }
            }
            point = m_database.getNextByName( *point );
        }

        // If I get here -->the command succeeded
        return Kit::TShell::Result_T::CMD_SUCCESS;
    }

    // READ Sub-command
    else if ( numParms == 2 )
    {
        // Look-up the model point
        Kit::Dm::IModelPoint* point = m_database.lookupModelPoint( tokens.getParameter( 1 ) );
        if ( point == nullptr )
        {
            outtext.format( "Model point not found: '%s'", tokens.getParameter( 1 ) );
            context.writeFrame( outtext );
            return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
        }

        // Generate the JSON object/string for the Model point
        bool  truncated;
        int   outlen;
        char* outptr = outtext.getBuffer( outlen );
        if ( point->toJSON( outptr, outlen, truncated, true, true ) == false || truncated )
        {
            outtext.format( "Failed to generate JSON for model point: %s", point->getName() );
            context.writeFrame( outtext );
            return Kit::TShell::Result_T::CMD_ERR_CMD_FAILED;
        }
        {
            outtext.format( "Failed to generate JSON for model point: %s", point->getName() );
            context.writeFrame( outtext );
            return Kit::TShell::Result_T::CMD_ERR_CMD_FAILED;
        }

        // Output the JSON object: NOTE: The JSON object contains newlines -->use 'special' write frame method
        return context.writeFrameWithSpecialChars( outtext ) ? Kit::TShell::Result_T::CMD_SUCCESS : Kit::TShell::Result_T::CMD_ERR_IO;
    }

    // If I get here the command syntax/arguments are bad
    return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
}

}  // end namespace
}
}
//------------------------------------------------------------------------------