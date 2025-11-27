#ifndef KIT_LOGGING_FRAMEWORK_LOGSOURCE_H_
#define KIT_LOGGING_FRAMEWORK_LOGSOURCE_H_
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
#include "Kit/Logging/Framework/ILog.h"
#include "Kit/Logging/Framework/Formatter.h"
#include "Kit/System/Mutex.h"
#include "Kit/Text/FString.h"

// #include "Kit/Container/RingBufferMP.h"
#include "Kit/Logging/Framework/EntryData.h"
#include <stdint.h>

/** The minimum amount of log FIFO queue space that must be available before
    adding new log entries again resumes after an overflow condition
 */
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_MIN_QUEUE_SPACE
#define OPTION_KIT_LOGGING_FRAMEWORK_MIN_QUEUE_SPACE 4
#endif


///
namespace Kit {
///
namespace Logging {
///
namespace Framework {

/** This class is responsible 'generating' log entries, i.e. provides the
    inteface for clients of the Logging framework to generate log entries.  The
    incoming log entries are:
    - Echoed to the TRACE engine
    - Filtered by Classification and Package
    - Log entries that 'pass' the filters and placed into the Log Entry FIFO

    The inteface IS thread safe. Client's can call the logf() from any thread.
    However, because log entries are echoed to the TRACE engine - the calling
    thread MUST be a KIT thread (see Kit::System::Trace interface for details
    on this restriction).
    */
class LogSource : public ILog
{
public:
    /// Constructor
    LogSource( IApplication& appInstance,
               /* Kit::Container::RingBufferMP<EntryData_T>& logEntryFIFO, */
               uint8_t     classificationIdForQueueOverflow,
               uint8_t     packageIdForQueueOverflow,
               uint8_t     subSystemIdForQueueOverflow,
               uint8_t     messageIdForQueueOverflow ) noexcept;


public:
    /// See Kit::Logging::Framework::ILog
    void vlogf( uint8_t     classificationId,
                uint8_t     packageId,
                uint8_t     subSystemId,
                uint8_t     messageId,
                const char* formatInfoText,
                va_list     ap ) noexcept override;

public:
    /// See Kit::Logging::Framework::ILog
    KitLoggingClassificationMask_T enableClassification( KitLoggingClassificationMask_T classificationMask ) noexcept override;

    /// See Kit::Logging::Framework::ILog
    KitLoggingClassificationMask_T disableClassification( KitLoggingClassificationMask_T classificationMask ) noexcept override;

    /// See Kit::Logging::Framework::ILog
    void setClassificationMask( KitLoggingClassificationMask_T newMask ) noexcept override;

    /// See Kit::Logging::Framework::ILog
    KitLoggingClassificationMask_T getClassificationEnabledMask() noexcept override;

public:
    /// See Kit::Logging::Framework::ILog
    KitLoggingPackageMask_T enablePackage( KitLoggingPackageMask_T packageMask ) noexcept override;

    /// See Kit::Logging::Framework::ILog
    KitLoggingPackageMask_T disablePackage( KitLoggingPackageMask_T packageMask ) noexcept override;

    /// See Kit::Logging::Framework::ILog
    void setPackageMask( KitLoggingPackageMask_T newMask ) noexcept override;

    /// See Kit::Logging::Framework::ILog
    KitLoggingPackageMask_T getPackageEnabledMask() noexcept override;

protected:
    /// Helper method to determine if the log queue is full
    bool isQueFull() noexcept;

    /// Helper method to create and add the 'overflow' log entry
    void createAndAddOverflowEntry() noexcept;

protected:
    /// Log entry FIFO
    // Kit::Container::RingBufferMP<EntryData_T>& m_logFifo;

    /// Provides thread safety/critical section
    Kit::System::Mutex m_lock;

    /// My application (i.e. used to resolve ID text strings)
    IApplication& m_app;

    /// Current Classification filter mask
    KitLoggingClassificationMask_T m_classificationFilterMask;

    /// Current Package filter mask
    KitLoggingPackageMask_T m_packageFilterMask;

    /// Number of log entries dropped due to FIFO full condition
    uint16_t m_overflowCount;

    /// Overflow ID for classification
    uint8_t m_classificationIdForQueueOverflow;

    /// Overflow ID for package
    uint8_t m_packageIdForQueueOverflow;

    /// Overflow ID for subsystem
    uint8_t m_subSystemIdForQueueOverflow;

    /// Overflow ID for message
    uint8_t m_messageIdForQueueOverflow;
    /// Work buffer to convert log entries to text
    Kit::Text::FString<OPTION_KIT_LOGGING_FORMATTER_MAX_TEXT_LEN + 1> m_workBuffer;

    /// Trace the log entry FIFO full state
    bool m_queueFull;
};


}  // end namespaces
}
}
#endif  // end header latch