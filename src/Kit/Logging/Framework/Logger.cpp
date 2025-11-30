/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/Framework/IApplication.h"
#include "Kit/Logging/Framework/IPackage.h"
#include "Kit/Logging/Pkg/Package.h"
#include "Kit/System/Assert.h"
#include "Log.h"
#include "Logger.h"
#include "Formatter.h"
#include "Kit/Logging/Pkg/MsgId.h"
#include "Kit/Logging/Pkg/SubSystemId.h"
#include "Kit/Text/FString.h"
#include "Kit/Text/BString.h"
#include "Kit/System/Mutex.h"
#include "Kit/System/Trace.h"
#include "Kit/Time/BootTime.h"

#define NUM_BITS( type ) ( sizeof( type ) * 8 )

//------------------------------------------------------------------------------
namespace Kit {
namespace Logging {
namespace Framework {

/* NOTE: Several variables are exposed as global variables (in the Kit::Logging::Framework
         namespace). This hack is done explicitly to support unit testing of
         other modules that generate log entries.  In an attempt to limit the
         'damage' of having global variables - these variables are NOT
         documented in Doxygen.
*/

static Kit::Container::RingBuffer<EntryData_T>*                          logFifo_;  // TODO: Needs to be Kit::Container::RingBufferMP
Kit::System::Mutex                                                       g_lock;
static IApplication*                                                     app_;
static KitLoggingClassificationMask_T                                    classificationFilterMask_;
static KitLoggingPackageMask_T                                           packageFilterMask_;
uint16_t                                                                 g_overflowCount;
static uint8_t                                                           classificationLoggingError_;
static Kit::Text::FString<OPTION_KIT_LOGGING_FORMATTER_MAX_TEXT_LEN + 1> workBuffer_;
static uint64_t                                                          overflowedTimestamp_;
bool                                                                     g_queueOverflowed;
unsigned                                                                 g_vlogfCallCount;  // Only used for unit testing


static bool isQueueOverflowed( uint64_t timestamp ) noexcept;
static void createAndAddOverflowEntry() noexcept;

// Note: Need a function that can be called from the initialize() method, i.e. to avoid the use case of initialize() be called BEFORE the mutex object has been created
static void resetWithoutCriticalSection() noexcept
{
    g_overflowCount           = 0;
    g_queueOverflowed         = false;
    classificationFilterMask_ = 0xFFFFFFFF;
    packageFilterMask_        = 0xFFFFFFFF;
    g_vlogfCallCount          = 0;
}

// CAUTION: This method is visible to the application by design. This method is
// for INTERNAL and unit testing purposes ONLY. Application code MUST NOT call
// this method!
void resetLoggerState() noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( g_lock );
    resetWithoutCriticalSection();
}


////////////////////////////////////////////////////////////////////////////////
void initialize( IApplication&                            appInstance,
                 Kit::Container::RingBuffer<EntryData_T>& logFifo,
                 uint8_t                                  classificationLoggingError ) noexcept
{
    app_                        = &appInstance;
    logFifo_                    = &logFifo;
    classificationLoggingError_ = classificationLoggingError;
    resetWithoutCriticalSection();
}

////////////////////////////////////////////////////////////////////////////////
void createAndAddOverflowEntry() noexcept
{
    // Generate entry (zero-initialize to avoid copying uninitialized padding bytes)
    EntryData_T logEntry        = {};
    logEntry.m_timestamp        = Kit::Time::getBootTime();
    logEntry.m_classificationId = classificationLoggingError_;
    logEntry.m_packageId        = Pkg::Package::PACKAGE_ID;
    logEntry.m_subSystemId      = Pkg::SubSystemId::LOGGING;
    logEntry.m_messageId        = Pkg::LoggingMsgId::QUEUE_OVERFLOW;

    // Create the info text
    workBuffer_.format( "QUEUE_OVERFLOW! Num entries lost=%u ", g_overflowCount );
    Formatter::appendFormattedTimestamp( overflowedTimestamp_, workBuffer_ );
    strncpy( logEntry.m_infoText, workBuffer_.getString(), OPTION_KIT_LOGGING_FRAMEWORK_MAX_MSG_TEXT_LEN );
    logEntry.m_infoText[OPTION_KIT_LOGGING_FRAMEWORK_MAX_MSG_TEXT_LEN] = '\0';  // Ensure the text string is null terminated

    // Add to the FIFO and echo to trace
    logFifo_->add( logEntry );
    Formatter::toString( *app_, logEntry, workBuffer_ );
    KIT_SYSTEM_TRACE_RESTRICTED_MSG( app_->classificationIdToString( logEntry.m_classificationId ), "%s", workBuffer_.getString() );
}

bool isQueueOverflowed( uint64_t timestamp ) noexcept
{
    // Return immediately if NOT in the overflowed state AND there is space available in the queue
    if ( !g_queueOverflowed && !logFifo_->isFull() )
    {
        return false;
    }

    // If I get here that means we are attempting to add a log entry while the
    // queue is full OR we are already in the overflow state and are waiting on
    // the queue hysteresis to clear.
    g_queueOverflowed = true;

    // Has space freed up?
    unsigned available = logFifo_->getMaxItems() - logFifo_->getNumItems();
    if ( available >= OPTION_KIT_LOGGING_FRAMEWORK_MIN_QUEUE_SPACE )
    {
        createAndAddOverflowEntry();
        g_overflowCount   = 0;
        g_queueOverflowed = false;
        return false;
    }

    // First time in overflow state - record the timestamp
    if ( g_overflowCount == 0 )
    {
        overflowedTimestamp_ = timestamp;
    }

    // No space - count the number of 'dropped' log entries
    g_overflowCount++;
    return true;
}


////////////////////////////////////////////////////////////////////////////////
LogResult_T vlogf( uint8_t     classificationId,
                   uint8_t     packageId,
                   uint8_t     subSystemId,
                   uint8_t     messageId,
                   const char* formatInfoText,
                   va_list     ap ) noexcept
{
    KIT_SYSTEM_ASSERT( app_ != nullptr );
    KIT_SYSTEM_ASSERT( logFifo_ != nullptr );
    KIT_SYSTEM_ASSERT( formatInfoText != nullptr );

    LogResult_T                   result   = FILTERED;
    uint8_t                       errMsgId = IPackage::NULL_MSG_ID;
    Kit::System::Mutex::ScopeLock criticalSection( g_lock );
    g_vlogfCallCount++;

    // Validate classificationId
    if ( app_->isClassificationIdValid( classificationId ) == false )
    {
        errMsgId = Pkg::LoggingMsgId::UNKNOWN_CLASSIFICATION_ID;
    }

    // Validate packageId
    IPackage* pkgPtr = app_->getPackage( packageId );
    if ( pkgPtr == nullptr )
    {
        errMsgId = Pkg::LoggingMsgId::UNKNOWN_PACKAGE_ID;
    }

    // If the package ID is not valid -->then skip subSystemId/messageId validation (since we cannot look it up)
    else
    {
        // Validate Remaining IDs
        const char* dstSubSystemText = nullptr;
        const char* dstMessageText   = nullptr;
        if ( pkgPtr->subSystemAndMessageIdsToString( subSystemId, dstSubSystemText, messageId, dstMessageText ) == false )
        {
            errMsgId = dstSubSystemText == nullptr ? Pkg::LoggingMsgId::UNKNOWN_SUBSYSTEM_ID : Pkg::LoggingMsgId::UNKNOWN_MESSAGE_ID;
        }
    }

    // Generate entry
    EntryData_T logEntry        = {};
    logEntry.m_classificationId = classificationId;
    logEntry.m_packageId        = packageId;
    logEntry.m_subSystemId      = subSystemId;
    logEntry.m_messageId        = messageId;
    logEntry.m_timestamp        = Kit::Time::getBootTime();
    Kit::Text::BString formattingBuffer( logEntry.m_infoText, sizeof( logEntry.m_infoText ) );
    if ( errMsgId == IPackage::NULL_MSG_ID )
    {
        // No logging error - format the user message
        formattingBuffer.vformat( formatInfoText, ap );
    }
    else
    {
        // Replace the original IDs with the error IDs
        logEntry.m_classificationId = classificationLoggingError_;
        logEntry.m_packageId        = Pkg::Package::PACKAGE_ID;
        logEntry.m_subSystemId      = Pkg::SubSystemId::LOGGING;
        logEntry.m_messageId        = errMsgId;

        // There was an error - format an error message
        formattingBuffer.format( "ClassificationID=%u, PackageID=%u, SubSystemID=%u, MessageID=%u.",
                                 classificationId,
                                 packageId,
                                 subSystemId,
                                 messageId );
    }

    // Must pass the filter checks to be added to the log queue
    auto classMask = classificationIdToMask( logEntry.m_classificationId );
    auto pkgMask   = packageIdToMask( logEntry.m_packageId );
    if ( ( classMask & classificationFilterMask_ ) && ( pkgMask & packageFilterMask_ ) )
    {
        // Manage the queue overflow state
        result = QUEUE_FULL;
        if ( !isQueueOverflowed( logEntry.m_timestamp ) )
        {
            // Space available in the queue -->add the new entry
            logFifo_->add( logEntry );
            result = ADDED;
        }
    }

    // Echo to the Trace engine (always echoed even when filtered)
    Formatter::toString( *app_, logEntry, workBuffer_ );
    KIT_SYSTEM_TRACE_RESTRICTED_MSG( app_->classificationIdToString( logEntry.m_classificationId ), "%s", workBuffer_.getString() );
    return result;
}

////////////////////////////////////////////////////////////////////////////////
KitLoggingClassificationMask_T enableClassification( KitLoggingClassificationMask_T classificationMask ) noexcept
{
    Kit::System::Mutex::ScopeLock  criticalSection( g_lock );
    KitLoggingClassificationMask_T oldMask  = classificationFilterMask_;
    classificationFilterMask_              |= classificationMask;
    return oldMask;
}

KitLoggingClassificationMask_T disableClassification( KitLoggingClassificationMask_T classificationMask ) noexcept
{
    Kit::System::Mutex::ScopeLock  criticalSection( g_lock );
    KitLoggingClassificationMask_T oldMask  = classificationFilterMask_;
    classificationFilterMask_              &= ~classificationMask;
    return oldMask;
}

KitLoggingClassificationMask_T getClassificationEnabledMask() noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( g_lock );
    return classificationFilterMask_;
}

void setClassificationMask( KitLoggingClassificationMask_T newMask ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( g_lock );
    classificationFilterMask_ = newMask;
}

////////////////////////////////////////////////////////////////////////////////
KitLoggingPackageMask_T enablePackage( KitLoggingPackageMask_T packageMask ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( g_lock );
    KitLoggingPackageMask_T       oldMask  = packageFilterMask_;
    packageFilterMask_                    |= packageMask;
    return oldMask;
}

KitLoggingPackageMask_T disablePackage( KitLoggingPackageMask_T packageMask ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( g_lock );
    KitLoggingPackageMask_T       oldMask  = packageFilterMask_;
    packageFilterMask_                    &= ~packageMask;
    return oldMask;
}

KitLoggingPackageMask_T getPackageEnabledMask() noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( g_lock );
    return packageFilterMask_;
}

void setPackageMask( KitLoggingPackageMask_T newMask ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( g_lock );
    packageFilterMask_ = newMask;
}

////////////////////////////////////////////////////////////////////////////////
KitLoggingClassificationMask_T classificationIdToMask( uint8_t classificationId ) noexcept
{
    if ( classificationId == 0 || classificationId > NUM_BITS( KitLoggingClassificationMask_T ) )
    {
        return 0;
    }
    return ( (KitLoggingClassificationMask_T)1 ) << ( classificationId - 1 );
}

uint8_t maskToClassificationId( KitLoggingClassificationMask_T classificationMask ) noexcept
{
    for ( uint8_t i = 0; i < NUM_BITS( KitLoggingClassificationMask_T ); ++i )
    {
        if ( classificationMask == ( ( (KitLoggingClassificationMask_T)1 ) << i ) )
        {
            return i + 1;
        }
    }
    return 0;
}

KitLoggingPackageMask_T packageIdToMask( uint8_t packageId ) noexcept
{
    if ( packageId == 0 || packageId > NUM_BITS( KitLoggingPackageMask_T ) )
    {
        return 0;
    }
    return ( (KitLoggingPackageMask_T)1 ) << ( packageId - 1 );
}

uint8_t maskToPackageId( KitLoggingPackageMask_T packageMask ) noexcept
{
    for ( uint8_t i = 0; i < NUM_BITS( KitLoggingPackageMask_T ); ++i )
    {
        if ( packageMask == ( ( (KitLoggingPackageMask_T)1 ) << i ) )
        {
            return i + 1;
        }
    }
    return 0;
}

}  // end namespace
}
}
//------------------------------------------------------------------------------