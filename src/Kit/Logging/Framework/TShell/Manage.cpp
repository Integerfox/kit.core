/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Manage.h"
#include "Kit/TShell/ICommand.h"
#include "Kit/Text/StringTo.h"
#include "Kit/Text/Tokenizer/TextBlock.h"
#include "Kit/Logging/Framework/Log.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Logging {
namespace Framework {
namespace TShell {

KIT_SYSTEM_PRINTF_CHECKER( 5, 6 )
static inline Kit::Logging::Framework::LogResult_T logf_( uint8_t     classId,
                                                          uint8_t     pkgId,
                                                          uint8_t     subId,
                                                          uint8_t     msgId,
                                                          const char* msgTextFormat,
                                                          ... ) noexcept
{
    va_list ap;
    va_start( ap, msgTextFormat );
    auto result = Kit::Logging::Framework::vlogf( classId, pkgId, subId, msgId, msgTextFormat, ap );
    va_end( ap );
    return result;
}


///////////////////////////
Kit::TShell::Result_T Manage::execute( Kit::TShell::IContext& context, char* cmdString ) noexcept
{
    Kit::Text::IString&             outtext = context.getOutputBuffer();
    Kit::Text::Tokenizer::TextBlock tokens( cmdString );


    // CLEAR
    if ( tokens.numParameters() == 2 && strcmp( tokens.getParameter( 1 ), "CLEAR" ) == 0 )
    {
        // Attempt to update the Model Point
        if ( !m_logReset.logicalReset() )
        {
            outtext.format( "Failed to reset log" );
            context.writeFrame( outtext );
            return Kit::TShell::Result_T::CMD_ERR_CMD_FAILED;
        }
        bool io = context.writeFrame( "Log cleared" );
        return io ? Kit::TShell::Result_T::CMD_SUCCESS : Kit::TShell::Result_T::CMD_ERR_IO;
    }

    // CREATE
    else if ( tokens.numParameters() == 7 && tokens.getParameter( 1 )[0] == 'c' )
    {
        // Parse the IDs
        uint8_t classId = 0;
        uint8_t pkgId   = 0;
        uint8_t subId   = 0;
        uint8_t msgId   = 0;
        if ( !Kit::Text::StringTo::unsignedInt( classId, tokens.getParameter( 2 ) ) ||
             !Kit::Text::StringTo::unsignedInt( pkgId, tokens.getParameter( 3 ) ) ||
             !Kit::Text::StringTo::unsignedInt( subId, tokens.getParameter( 4 ) ) ||
             !Kit::Text::StringTo::unsignedInt( msgId, tokens.getParameter( 5 ) ) ||
            m_appLogInfo.getPackage( pkgId ) == nullptr )
        {
            outtext.format( "Invalid argument(s): '%s' '%s' '%s' '%s'",
                            tokens.getParameter( 2 ),
                            tokens.getParameter( 3 ),
                            tokens.getParameter( 4 ),
                            tokens.getParameter( 5 ));
            context.writeFrame( outtext );
            return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
        }

        // Strip the quotes from the text argument
        const char* textArg = tokens.getParameter( 6 );

        // Create (well attempt to create) the log entry
        auto result = logf_( classId, pkgId, subId, msgId, "%s", textArg );
        outtext.format( "Log Record: %s",
                        result == Kit::Logging::Framework::LogResult_T::ADDED
                            ? "added"
                        : result == Kit::Logging::Framework::LogResult_T::FILTERED
                            ? "filtered"
                            : "queFull");
        bool io = context.writeFrame( outtext );
        return io ? Kit::TShell::Result_T::CMD_SUCCESS : Kit::TShell::Result_T::CMD_ERR_IO;
    }

    // Invalid command syntax
    return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
}

}  // end namespace
}
}
}
//------------------------------------------------------------------------------