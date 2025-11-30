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
#include "Kit/Time/gmtime.h"
#include <inttypes.h>


//------------------------------------------------------------------------------
namespace Kit {
namespace Logging {
namespace Framework {

//////////////////////////////////////////////////////////////////////////////
void Formatter::appendFormattedTimestamp( uint64_t            timestamp,
                                          Kit::Text::IString& dstStringBuf ) noexcept
{
    // Append the boot-counter portion of the entry's timestamp
    uint16_t bootCounter;
    uint64_t elapsedTimeMs;
    Kit::Time::parseBootTime( timestamp, bootCounter, elapsedTimeMs );
    dstStringBuf.formatAppend( "(%u:", bootCounter );

    // Convert elapsed time (ms since epoch) to absolute UTC time format
    time_t     seconds      = elapsedTimeMs / 1000;
    uint16_t   milliseconds = elapsedTimeMs % 1000;
    struct tm  utcTime;
    struct tm* utcTimePtr = Kit::Time::gmtimeMT( &seconds, &utcTime );
    if ( utcTimePtr != nullptr )
    {
        dstStringBuf.formatAppend( "%04d-%02d-%02d %02d:%02d:%02d.%03u) ",
                                   utcTimePtr->tm_year + 1900,
                                   utcTimePtr->tm_mon + 1,
                                   utcTimePtr->tm_mday,
                                   utcTimePtr->tm_hour,
                                   utcTimePtr->tm_min,
                                   utcTimePtr->tm_sec,
                                   milliseconds );
    }
}

bool Formatter::toString( IApplication&                               application,
                          const Kit::Logging::Framework::EntryData_T& srcEntryToFormat,
                          Kit::Text::IString&                         dstStringBuf,
                          uint64_t                                    persistentStorageId ) noexcept
{
    dstStringBuf.clear();
    if ( persistentStorageId != UINT64_MAX )
    {
        // Format the Persistent Storage ID
        dstStringBuf.formatAppend( "[%" PRIu64 "] ", persistentStorageId );
    }

    // Append the boot-counter portion of the entry's timestamp
    appendFormattedTimestamp( srcEntryToFormat.m_timestamp, dstStringBuf );

    // Append the identifiers and the info text to the log entry
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
    dstStringBuf.formatAppend( "%s-%s-%s-%s: %s",
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