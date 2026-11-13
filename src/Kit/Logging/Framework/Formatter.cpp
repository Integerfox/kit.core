/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Formatter.h"
#include "Kit/Logging/Framework/IPackage.h"
#include "Kit/Time/BootTime.h"
#include "Kit/Text/Format.h"
#include <inttypes.h>


//------------------------------------------------------------------------------
namespace Kit {
namespace Logging {
namespace Framework {

//////////////////////////////////////////////////////////////////////////////
void Formatter::appendFormattedTimestamp( uint64_t            timestamp,
                                          Kit::Text::IString& dstStringBuf,
                                          TimestampFormat_T   timestampFormat ) noexcept
{
    // Append the boot-counter portion of the entry's timestamp
    KitTimeBootCount_T bootCounter;
    uint64_t           elapsedTimeMs;
    Kit::Time::parseBootTime( timestamp, bootCounter, elapsedTimeMs );
    dstStringBuf.formatAppend( "(%" PRIu64 ":", static_cast<uint64_t>( bootCounter ) );

    // Elapsed time (ms since power-up) OR absolute UTC time (ms since the Epoch)
    if ( timestampFormat == TimestampFormat_T::eELAPSED )
    {
        Kit::Text::Format::timestamp( dstStringBuf, elapsedTimeMs, true, true, true );
    }
    else
    {
        Kit::Text::Format::utcTimestamp( dstStringBuf, elapsedTimeMs, true, true );
    }
    dstStringBuf += ") ";
}

bool Formatter::toString( IApplication&                               application,
                          const Kit::Logging::Framework::EntryData_T& srcEntryToFormat,
                          Kit::Text::IString&                         dstStringBuf,
                          uint64_t                                    persistentStorageId,
                          TimestampFormat_T                           timestampFormat ) noexcept
{
    dstStringBuf.clear();
    if ( persistentStorageId != UINT64_MAX )
    {
        // Format the Persistent Storage ID
        dstStringBuf.formatAppend( "[%" PRIX64 "] ", persistentStorageId );
    }

    // Append the boot-counter portion of the entry's timestamp
    appendFormattedTimestamp( srcEntryToFormat.m_timestamp, dstStringBuf, timestampFormat );

    // Append the identifiers and the info text to the log entry
    const char* appStateText       = application.applicationStateToString( srcEntryToFormat.m_applicationState );
    const char* classificationText = application.classificationIdToString( srcEntryToFormat.m_classificationId );
    const char* packageText        = IPackage::NULL_PKG_ID_TEXT;
    const char* subSystemText      = IPackage::NULL_SUBSYS_ID_TEXT;
    const char* messageIdText      = IPackage::NULL_MSG_ID_TEXT;
    IPackage*   pkgPtr             = application.getPackage( srcEntryToFormat.m_packageId );
    if ( pkgPtr != nullptr )
    {
        packageText   = pkgPtr->packageIdString();
        subSystemText = nullptr;
        messageIdText = nullptr;
        if ( pkgPtr->subSystemAndMessageIdsToString( srcEntryToFormat.m_subSystemId,
                                                     subSystemText,
                                                     srcEntryToFormat.m_messageId,
                                                     messageIdText ) == false )
        {
            if ( subSystemText == nullptr )
            {
                subSystemText = IPackage::NULL_SUBSYS_ID_TEXT;
            }
            if ( messageIdText == nullptr )
            {
                messageIdText = IPackage::NULL_MSG_ID_TEXT;
            }
        }
    }

    // The '%.*s' precision enforces the Application state's text budget, i.e. a non-conforming
    // IApplication cannot push the formatted entry past OPTION_KIT_LOGGING_FORMATTER_MAX_TEXT_LEN
    dstStringBuf.formatAppend( "%.*s%s-%s-%s-%s: %s",
                               static_cast<int>( OPTION_KIT_LOGGING_FORMATTER_APPLICATION_STATE_TEXT_LEN ),
                               appStateText,
                               classificationText,
                               packageText,
                               subSystemText,
                               messageIdText,
                               srcEntryToFormat.m_infoText );

    // All done!
    return !dstStringBuf.truncated();
}

}  // end namespace
}
}
//------------------------------------------------------------------------------