#ifndef KIT_LOGGING_FRAMEWORK_FORMATTER_H_
#define KIT_LOGGING_FRAMEWORK_FORMATTER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "Kit/Logging/Framework/EntryData.h"
#include "Kit/Logging/Framework/IApplication.h"
#include "Kit/Text/IString.h"


/** The size, in bytes, needed to format the persistent storage ID.  The size
    does not include the space reserved for the null terminator
    */
#ifndef OPTION_KIT_LOGGING_FORMATTER_PERSISTENT_STORAGE_ID_TEXT_LEN
#define OPTION_KIT_LOGGING_FORMATTER_PERSISTENT_STORAGE_ID_TEXT_LEN ( 16 + 3 )  // 16 digits + brackets
#endif

/** The size, in bytes, needed to format the timestamp information.  The size
    does not include the space reserved for the null terminator
 */
#ifndef OPTION_KIT_LOGGING_FORMATTER_TIMESTAMP_TEXT_LEN
#define OPTION_KIT_LOGGING_FORMATTER_TIMESTAMP_TEXT_LEN 31
#endif

/** The size, in bytes, needed to render the Application state, e.g. "*".  The
    size does not include the space reserved for the null terminator.  Renderings
    longer than this are truncated by toString().
 */
#ifndef OPTION_KIT_LOGGING_FORMATTER_APPLICATION_STATE_TEXT_LEN
#define OPTION_KIT_LOGGING_FORMATTER_APPLICATION_STATE_TEXT_LEN 1
#endif

/** The size, in bytes, needed to format the non-message portions of a
    'textified' log entry plus the maximum message text.  The size does NOT
    include the space reserved for the null terminator.
 */
#ifndef OPTION_KIT_LOGGING_FORMATTER_MAX_TEXT_LEN
#define OPTION_KIT_LOGGING_FORMATTER_MAX_TEXT_LEN                       \
    ( OPTION_KIT_LOGGING_FORMATTER_PERSISTENT_STORAGE_ID_TEXT_LEN + 1 + \
      OPTION_KIT_LOGGING_FORMATTER_TIMESTAMP_TEXT_LEN + 1 +             \
      OPTION_KIT_LOGGING_FORMATTER_APPLICATION_STATE_TEXT_LEN +         \
      OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_CLASSIFICATION_ID_TEXT + 1 + \
      OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_PACKAGE_ID_TEXT + 1 +        \
      OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_SUBSYSTEM_ID_TEXT + 1 +      \
      OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_MESSAGE_ID_TEXT + 2 +        \
      OPTION_KIT_LOGGING_FRAMEWORK_MAX_MSG_TEXT_LEN )
#endif

/** The enumerator name (of Formatter::TimestampFormat_T) of the default
    timestamp format.  USE_KIT_TIME_BOOTTIME_WITH_ABSOLUTETIME is the single
    source of truth, i.e. this symbol is derived - it is NOT an application
    option.  Use Formatter::defaultTimestampFormat instead of this symbol.
 */
#ifdef USE_KIT_TIME_BOOTTIME_WITH_ABSOLUTETIME
#define KIT_LOGGING_FORMATTER_DEFAULT_TIMESTAMP_FORMAT_ eABSOLUTE
#else
#define KIT_LOGGING_FORMATTER_DEFAULT_TIMESTAMP_FORMAT_ eELAPSED
#endif

///
namespace Kit {
///
namespace Logging {
///
namespace Framework {

/** This class provides a collection of formatting method(s) for log entries
 */
class Formatter
{
public:
    /** Selects how the time portion of an entry's timestamp (see
        Kit::Time::getBootTime()) is rendered.  The timestamp's time portion is
        elapsed time since power-up unless the application has configured boot
        time to use absolute time (i.e. USE_KIT_TIME_BOOTTIME_WITH_ABSOLUTETIME)
     */
    enum class TimestampFormat_T
    {
        eABSOLUTE,  //!< UTC date/time, i.e. the time is milliseconds since the Epoch: YYYY-MM-DD HH:MM:SS.mmm
        eELAPSED    //!< Elapsed time, i.e. the time is milliseconds since power-up: DD HH:MM:SS.mmm
    };

    /** The default timestamp format, i.e. the format that matches the time
        source of Kit::Time::getBootTime().  USE_KIT_TIME_BOOTTIME_WITH_ABSOLUTETIME
        (defined in kit_config.h) is the source of truth: eABSOLUTE when it is
        defined, else eELAPSED.

        NOTE: An application that provides its own getBootTime() (see
              USE_KIT_TIME_CUSTOM_GET_BOOT_TIME) that returns absolute time must
              also define USE_KIT_TIME_BOOTTIME_WITH_ABSOLUTETIME.
     */
    static constexpr TimestampFormat_T defaultTimestampFormat = TimestampFormat_T::KIT_LOGGING_FORMATTER_DEFAULT_TIMESTAMP_FORMAT_;

public:
    /** Converts a binary EntryData_T struct instance to formatted text. The
        application is responsible for ensuring the 'dstStringBuf' argument
        can contain the maximum formatted size (the default size is
        OPTION_KIT_LOGGING_FORMATTER_MAX_TEXT_LEN). The 'persistentStorageId'
        argument is optional and represents the persistent storage ID for the
        entry being converted.  The 'timestampFormat' argument selects how the
        timestamp is rendered; it defaults to defaultTimestampFormat.  The
        format shown below is the eABSOLUTE format; eELAPSED replaces
        YYYY-MM-DD HH:MM:SS.mmm with DD HH:MM:SS.mmm.

        The method returns true when successful.  False is returned if the provided
        string buffer was not large enough to hold the formatted entry.

        \code

        The format is:
            [<storageIdAsciiHex>] (<bootCnt>:YYYY-MM-DD HH:MM:SS.mmm) <appState><classification>-<packageId>-<subSystemId>-<msgId>: <messageText>
        OR
            (<bootCnt>:YYYY-MM-DD HH:MM:SS.mmm) <appState><classification>-<packageId>-<subSystemId>-<msgId>: <messageText>

        Where <appState> is the Application's rendering of the entry's application
        state (see IApplication::applicationStateToString()).  By default this is
        an empty string.  The rendering is truncated to
        OPTION_KIT_LOGGING_FORMATTER_APPLICATION_STATE_TEXT_LEN characters.
        \endcode
    */
    static bool toString( IApplication&                               application,
                          const Kit::Logging::Framework::EntryData_T& srcEntryToFormat,
                          Kit::Text::IString&                         dstStringBuf,
                          uint64_t                                    persistentStorageId = UINT64_MAX,
                          TimestampFormat_T                           timestampFormat     = defaultTimestampFormat ) noexcept;

    /// Helper method: Appends the formatted timestamp to 'dstStringBuf' (uses the same timestamp formatting as toString())
    static void appendFormattedTimestamp( uint64_t            timestamp,
                                          Kit::Text::IString& dstStringBuf,
                                          TimestampFormat_T   timestampFormat = defaultTimestampFormat ) noexcept;
};

}  // end namespaces
}
}
#endif  // end header latch