/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Viewer.h"
#include "Kit/Logging/Framework/Formatter.h"
#include "Kit/Text/Tokenizer/TextBlock.h"
#include "Kit/Text/StringTo.h"
#include "Kit/Logging/Framework/IApplication.h"
#include "Kit/Logging/Pkg/ClassificationId.h"
#include "Kit/Logging/Pkg/Package.h"
#include <inttypes.h>

using namespace Kit::Logging::Framework;
using namespace Kit::Persistence::Record::Journal;
//------------------------------------------------------------------------------
namespace Kit {
namespace Logging {
namespace Framework {
namespace TShell {

namespace {

class KitOnlyApp : public Kit::Logging::Framework::IApplication
{
public:
    bool isClassificationIdValid( uint8_t classificationId ) noexcept override
    {
        return Kit::Logging::Pkg::ClassificationId::_from_integral_nothrow( classificationId );
    }

    const char* classificationIdToString( uint8_t classificationId ) noexcept override
    {
        return Kit::Type::betterEnumToString<Kit::Logging::Pkg::ClassificationId, uint8_t>( classificationId,
                                                                                            NULL_CLASSIFICATION_ID_TEXT );
    }

    IPackage* getPackage( uint8_t packageId ) noexcept override
    {
        if ( packageId == m_kitPackage.PACKAGE_ID )
        {
            return &m_kitPackage;
        }
        return nullptr;
    }

private:
    Kit::Logging::Pkg::Package m_kitPackage;
};


static bool parseUnsigned_( const char* src, size_t& dst ) noexcept
{
    unsigned value = 0;
    if ( !Kit::Text::StringTo::unsignedInt( value, src ) )
    {
        return false;
    }

    dst = value;
    return true;
}

}  // end anonymous namespace


///////////////////////////
Kit::TShell::Result_T Viewer::execute( Kit::TShell::IContext& context, char* cmdString ) noexcept
{
    Kit::Text::Tokenizer::TextBlock tokens( cmdString );
    Kit::Text::IString&             outtext = context.getOutputBuffer();
    unsigned                        numArgs = tokens.numParameters();

    // Default: Display ONLY the latest record
    size_t startNth = 0;
    size_t maxCount = 1;

    // Parse: Display all/N Entries
    if ( numArgs == 2 )
    {
        // ALL
        const char* arg1 = tokens.getParameter( 1 );
        if ( arg1[0] == '*' )
        {
            maxCount = m_logReader.maxIndex() + 1;
        }
        else
        {
            // N entries
            if ( !parseUnsigned_( arg1, maxCount ) || maxCount == 0 )
            {
                outtext.format( "Invalid argument: '%s'", arg1 );
                context.writeFrame( outtext );
                return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
            }
        }
    }

    // Parse: Range of Entries
    else if ( numArgs == 3 )
    {
        const char* arg1 = tokens.getParameter( 1 );
        const char* arg2 = tokens.getParameter( 2 );

        if ( !parseUnsigned_( arg1, startNth ) )
        {
            outtext.format( "Invalid argument: '%s'", arg1 );
            context.writeFrame( outtext );
            return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
        }

        if ( arg2[0] == '*' )
        {
            maxCount = m_logReader.maxIndex() + 1;
        }
        else
        {
            if ( !parseUnsigned_( arg2, maxCount ) || maxCount == 0 )
            {
                outtext.format( "Invalid argument: '%s'", arg2 );
                context.writeFrame( outtext );
                return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
            }
        }
    }

    // Bad syntax: Too many arguments
    else if ( numArgs > 3 )
    {
        return Kit::TShell::Result_T::CMD_ERR_BAD_SYNTAX;
    }

    // Get the latest entry
    EntryData_T      logEntry;
    IEntry::Marker_T marker;
    if ( !m_logReader.retrieveLatest( logEntry, marker ) )
    {
        context.writeFrame( "No log entries found" );
        return Kit::TShell::Result_T::CMD_ERR_CMD_FAILED;
    }

    // Skip over entries to display WHEN not starting with the latest
    for ( size_t i = 0; i < startNth; ++i )
    {
        Kit::Persistence::Record::Journal::IEntry::Marker_T previousMarker;
        if ( !m_logReader.retrievePrevious( marker.timestamp, marker, logEntry, previousMarker ) )
        {
            return Kit::TShell::Result_T::CMD_SUCCESS;
        }
        marker = previousMarker;
    }

    // Display the requested number of entries
    bool io = true;
    size_t entriesDisplayed = 0;
    for ( size_t i = 0; i < maxCount && io; ++i )
    {
        // Display the found log entry
        if ( !Formatter::toString( m_appLogInfo, logEntry, outtext, marker.timestamp ) )
        {
            outtext.format( "Failed to format log entry (%" PRIu64 ").", marker.timestamp );
            context.writeFrame( outtext );
            return Kit::TShell::Result_T::CMD_ERR_CMD_FAILED;
        }
        io &= context.writeFrame( outtext );
        ++entriesDisplayed;

        // Get the next older entry
        Kit::Persistence::Record::Journal::IEntry::Marker_T previousMarker;
        if ( !m_logReader.retrievePrevious( marker.timestamp, marker, logEntry, previousMarker ) )
        {
            break;
        }
        marker = previousMarker;
    }

    // Summary of how many entries were displayed
    outtext.format( "log entries found = %zu.", entriesDisplayed );
    io &= context.writeFrame( outtext );
    return io ? Kit::TShell::Result_T::CMD_SUCCESS : Kit::TShell::Result_T::CMD_ERR_IO;
}

}  // end namespace
}
}
}
//------------------------------------------------------------------------------