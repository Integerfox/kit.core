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
#include "Kit/Logging/Framework/IPackage.h"
#include "Kit/TShell/ICommand.h"
#include "Kit/Text/StringTo.h"
#include "Kit/Text/Tokenizer/TextBlock.h"
#include "Kit/Logging/Framework/Log.h"
#include "Kit/Logging/Framework/Logger.h"

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

static bool parsePackageId_( uint8_t&               dstPkgId,
                             const char*            srcPkgString,
                             IApplication&          appLogInfo,
                             IPackage*&             dstPkgPtr,
                             Kit::TShell::IContext& context,
                             Kit::Text::IString&    outtext ) noexcept
{
    if ( !Kit::Text::StringTo::unsignedInt( dstPkgId, srcPkgString ) ||
         ( dstPkgPtr = appLogInfo.getPackage( dstPkgId ) ) == nullptr )
    {
        outtext.format( "Invalid Package ID: '%s'", srcPkgString );
        context.writeFrame( outtext );
        return false;
    }
    return true;
}

static bool parseClassificationId_( uint8_t&               dstClassId,
                                    const char*            srcClassString,
                                    IApplication&          appLogInfo,
                                    Kit::TShell::IContext& context,
                                    Kit::Text::IString&    outtext ) noexcept
{
    if ( !Kit::Text::StringTo::unsignedInt( dstClassId, srcClassString ) ||
         !appLogInfo.isClassificationIdValid( dstClassId ) )
    {
        outtext.format( "Invalid Classification ID: '%s'", srcClassString );
        context.writeFrame( outtext );
        return false;
    }
    return true;
}

static bool displayClassificationMask_( Kit::TShell::IContext& context, Kit::Text::IString& outtext )
{
    KitLoggingClassificationMask_T mask = getClassificationEnabledMask();
    outtext.format( "Classification IDs: 0x%08" PRIX32, static_cast<uint32_t>( mask ) );
    return context.writeFrame( outtext );
}

static bool displayPackageMask_( Kit::TShell::IContext& context, Kit::Text::IString& outtext )
{
    KitLoggingPackageMask_T mask = getPackageEnabledMask();
    outtext.format( "Package IDs: 0x%08" PRIX32, static_cast<uint32_t>( mask ) );
    return context.writeFrame( outtext );
}

///////////////////////////
Kit::TShell::Result_T Manage::execute( Kit::TShell::IContext& context, char* cmdString ) noexcept
{
    Kit::Text::IString&             outtext = context.getOutputBuffer();
    Kit::Text::Tokenizer::TextBlock tokens( cmdString );

    // CREATE
    if ( tokens.numParameters() == 7 && tokens.getParameter( 1 )[0] == 'c' )
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
                            tokens.getParameter( 5 ) );
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
                            : "queFull" );
        bool io = context.writeFrame( outtext );
        return io ? Kit::TShell::Result_T::CMD_SUCCESS : Kit::TShell::Result_T::CMD_ERR_IO;
    }

    // CLASS / PKG enable/disable
    if ( tokens.numParameters() >= 4 )
    {
        // CLASS
        if ( tokens.getParameter( 1 )[0] == 'c' )
        {
            // CLASS ENABLE
            if ( tokens.getParameter( 2 )[0] == 'e' )
            {
                // Enable the specified Classification IDs
                KitLoggingClassificationMask_T mask = 0;
                for ( size_t i = 3; i < tokens.numParameters(); ++i )
                {
                    uint8_t classId = 0;
                    if ( parseClassificationId_( classId, tokens.getParameter( i ), m_appLogInfo, context, outtext ) == false )
                    {
                        return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
                    }
                    mask |= classificationIdToMask( classId );
                }
                enableClassification( mask );
                return displayClassificationMask_( context, outtext ) ? Kit::TShell::Result_T::CMD_SUCCESS : Kit::TShell::Result_T::CMD_ERR_IO;
            }

            // CLASS DISABLE
            if ( tokens.getParameter( 2 )[0] == 'd' )
            {
                // Disable the specified Classification IDs
                KitLoggingClassificationMask_T mask = 0;
                for ( size_t i = 3; i < tokens.numParameters(); ++i )
                {
                    uint8_t classId = 0;
                    if ( parseClassificationId_( classId, tokens.getParameter( i ), m_appLogInfo, context, outtext ) == false )
                    {
                        return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
                    }
                    mask |= classificationIdToMask( classId );
                }
                disableClassification( mask );
                return displayClassificationMask_( context, outtext ) ? Kit::TShell::Result_T::CMD_SUCCESS : Kit::TShell::Result_T::CMD_ERR_IO;
            }

            return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
        }

        // PKG
        if ( tokens.getParameter( 1 )[0] == 'p' )
        {
            // PKG ENABLE
            if ( tokens.getParameter( 2 )[0] == 'e' )
            {
                // Enable the specified Package IDs
                KitLoggingPackageMask_T mask = 0;
                for ( size_t i = 3; i < tokens.numParameters(); ++i )
                {
                    uint8_t   pkgId  = 0;
                    IPackage* pkgPtr = nullptr;
                    if ( parsePackageId_( pkgId, tokens.getParameter( i ), m_appLogInfo, pkgPtr, context, outtext ) == false )
                    {
                        return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
                    }
                    mask |= packageIdToMask( pkgId );
                }
                enablePackage( mask );
                return displayPackageMask_( context, outtext ) ? Kit::TShell::Result_T::CMD_SUCCESS : Kit::TShell::Result_T::CMD_ERR_IO;
            }

            // PKG DISABLE
            if ( tokens.getParameter( 2 )[0] == 'd' )
            {
                // Disable the specified Package IDs
                KitLoggingPackageMask_T mask = 0;
                for ( size_t i = 3; i < tokens.numParameters(); ++i )
                {
                    uint8_t   pkgId  = 0;
                    IPackage* pkgPtr = nullptr;
                    if ( parsePackageId_( pkgId, tokens.getParameter( i ), m_appLogInfo, pkgPtr, context, outtext ) == false )
                    {
                        return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
                    }
                    mask |= packageIdToMask( pkgId );
                }
                disablePackage( mask );
                return displayPackageMask_( context, outtext ) ? Kit::TShell::Result_T::CMD_SUCCESS : Kit::TShell::Result_T::CMD_ERR_IO;
            }

            return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
        }
        return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
    }

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

    // Invalid command syntax
    return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
}

}  // end namespace
}
}
}
//------------------------------------------------------------------------------