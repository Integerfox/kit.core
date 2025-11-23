#ifndef KIT_LOGGING_FORMATTER_H_
#define KIT_LOGGING_FORMATTER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/EntryData_T.h"
#include "Kit/Text/IString.h"
#include <inttypes.h>


/** The size, in bytes, need to 'format' the message text with the 'textified'
      category and message IDs.  The size does NOT include the space reserved
      for the null terminator
   */
#ifndef OPTION_KIT_LOGGING_FORMATTER_MAX_TEXT_LEN
#define OPTION_KIT_LOGGING_FORMATTER_MAX_TEXT_LEN ( OPTION_KIT_LOGGING_MAX_MSG_TEXT_LEN + OPTION_KIT_LOGGING_MAX_LEN_CATEGORY_ID_TEXT + 1 + OPTION_KIT_LOGGING_MAX_LEN_MESSAGE_ID_TEXT + 2 )
#endif

///
namespace Kit {
///
namespace Logging {

/** This class provides a collection of formatting method(s) for log entries
 */
class EntryFormatter
{
public:
    /** Converts a binary EntryData_T struct instance to formatted text. The
        application is responsible for ensuring the 'dstStringBuf' argument
        can contain the maximum formatted size (the default size is
        OPTION_KIT_LOGGING_FORMATTER_MAX_TEXT_LEN). The 'persistentStorageId'
        argument is optional and represents the persistent storage ID for the
        entry being converted.

        The method returns true when successful.  False is returned if the provided
        string buffer was not large enough to hold the formatted entry.

        \code

        The format is:
            [<storageId>] (<bootCnt>:YYYY/MM/DD-HH:MM:SS.mmm) <category>-<domainId>-<sourceId>-<msgId>: <messageText>
        OR
            (<bootCnt>:YYYY/MM/DD-HH:MM:SS.mmm) <category>-<domainId>-<sourceId>-<msgId>: <messageText>
        \endcode
    */
    static bool toString( const Kit::Logging::EntryData_T& srcEntryToFormat,
                          Kit::Text::IString&              dstStringBuf,
                          uint64_t                         persistentStorageId = UINT64_MAX ) noexcept;
};


}  // end namespaces
}
#endif  // end header latch