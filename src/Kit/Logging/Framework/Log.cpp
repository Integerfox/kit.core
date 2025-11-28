/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Log.h"
#include "Formatter.h"
#include "Kit/Text/FString.h"
#include "Kit/System/Mutex.h"
#include "Kit/System/Trace.h"
#include "Kit/Time/BootTime.h"

#define NUM_BITS( type ) ( sizeof( type ) * 8 )

//------------------------------------------------------------------------------
namespace Kit {
namespace Logging {
namespace Framework {

/* NOTE: Several variables are exposed as global variables (in the Kit::Logging::Framework
         namespace. This hack is done explicitly to support unit testing of
         other modules that generate log entries.  In an attempt to limit the
         'damage' of having global variables - these variables are NOT
         documented/exposed in/to Doxygen.
*/

// static Kit::Container::RingBufferMP<EntryData_T>* logFifo_;
static Kit::System::Mutex                                                lock_;
static IApplication*                                                     app_;
static KitLoggingClassificationMask_T                                    classificationFilterMask_;
static KitLoggingPackageMask_T                                           packageFilterMask_;
uint16_t                                                                 g_overflowCount;
static uint8_t                                                           classificationIdForQueueOverflow_;
static uint8_t                                                           packageIdForQueueOverflow_;
static uint8_t                                                           subSystemIdForQueueOverflow_;
static uint8_t                                                           messageIdForQueueOverflow_;
static Kit::Text::FString<OPTION_KIT_LOGGING_FORMATTER_MAX_TEXT_LEN + 1> workBuffer_;
bool                                                                     g_queueFull;
unsigned                                                                 g_vlogfCallCount;  // Only used for unit testing

void initialize( IApplication& appInstance,
                 /* Kit::Container::RingBufferMP<EntryData_T>& logEntryFIFO, */
                 uint8_t classificationIdForQueueOverflow,
                 uint8_t packageIdForQueueOverflow,
                 uint8_t subSystemIdForQueueOverflow,
                 uint8_t messageIdForQueueOverflow ) noexcept
{
    app_ = &appInstance;
    // logFifo_                             = &logEntryFIFO;
    classificationFilterMask_         = 0xFFFFFFFF;
    packageFilterMask_                = 0xFFFFFFFF;
    g_overflowCount                   = 0;
    classificationIdForQueueOverflow_ = classificationIdForQueueOverflow;
    packageIdForQueueOverflow_        = packageIdForQueueOverflow;
    subSystemIdForQueueOverflow_      = subSystemIdForQueueOverflow;
    messageIdForQueueOverflow_        = messageIdForQueueOverflow;
    g_queueFull                       = false;
}

////////////////////////////////////////////////////////////////////////////////
void createAndAddOverflowEntry() noexcept
{
#if 0
    // Generate entry
    EntryData_T logEntry;
    logEntry.classification = overflowClassificationId_;
    logEntry.msgId          = overflowMsgId_;
    logEntry.timestamp      = Kit::Time::getBootTime();

    // Format text
    Kit::Text::FString<OPTION_KIT_LOGGING_FRAMEWORK_MAX_FORMATTED_MSG_TEXT_LEN> stringBuf;
    startText( stringBuf, classificationTextForQueueOverflow_, overflowMsgText_ );
    stringBuf.formatAppend( "overflow count=%d", g_overflowCount );

    // Add to the FIFO and echo to trace
    // logEntryFIFO_->add( logEntry );
    KIT_SYSTEM_TRACE_MSG( classificationTextForQueueOverflow_,  "%s", stringBuf.getString() ) );
#endif
}

bool isQueFull() noexcept
{
    // Return immediately if not in the overflowed state
    if ( !g_queueFull )
    {
        return false;
    }

    // // Has space freed up?
    // unsigned available = logEntryFIFO_->getMaxItems() - logEntryFIFO_->getNumItems();
    // if ( available >= OPTION_KIT_LOGGING_FRAMEWORK_MIN_QUEUE_SPACE )
    // {
    //     createAndAddOverflowEntry();
    //     g_overflowCount = 0;
    //     g_queueFull     = false;
    //     return false;
    // }

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
    LogResult_T                   result = FILTERED;
    Kit::System::Mutex::ScopeLock criticalSection( lock_ );
    g_vlogfCallCount++;

    // Validate classificationId against the number of bits in KitLoggingClassificationMask_T
    if ( classificationId > ( sizeof( KitLoggingClassificationMask_T ) * 8 ) )
    {
        classificationId = 0;
    }

    // Validate packageId against the number of bits in KitLoggingPackageMask_T
    if ( packageId > ( sizeof( KitLoggingPackageMask_T ) * 8 ) )
    {
        packageId = 0;
    }

    // Check if enabled
    if ( ( classificationId & classificationFilterMask_ ) &&
         ( packageId & packageFilterMask_ ) )
    {
        // Generate entry
        EntryData_T logEntry;
        logEntry.m_classificationId = classificationId;
        logEntry.m_packageId        = packageId;
        logEntry.m_subSystemId      = subSystemId;
        logEntry.m_messageId        = messageId;
        logEntry.m_timestamp        = Kit::Time::getBootTime();
        vsnprintf( logEntry.m_infoText, sizeof( logEntry.m_infoText ), formatInfoText, ap );
        logEntry.m_infoText[OPTION_KIT_LOGGING_FRAMEWORK_MAX_MSG_TEXT_LEN] = '\0';  // Ensure the text string is null terminated

        // Manage the queue overflow state
        if ( !isQueFull() )
        {
            // // Space available in the queue -->add the new entry
            // logEntryFIFO_->add( logEntry );
            // if ( logEntryFIFO_->isFull() )
            // {
            //     g_queueFull = true;
            //     result     = QUEUE_FULL;
            // }
            // else
            // {
            result = ADDED;
            // }
        }
        else
        {
            result = QUEUE_FULL;
        }

        // Echo to the Trace engine (always echoed even when not added to the FIFO)
        Formatter::toString( *app_, logEntry, workBuffer_ );
        KIT_SYSTEM_TRACE_RESTRICTED_MSG( app_->classificationIdToString( classificationId ), "%s", workBuffer_.getString() );
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////
KitLoggingClassificationMask_T enableClassification( KitLoggingClassificationMask_T classificationMask ) noexcept
{
    Kit::System::Mutex::ScopeLock  criticalSection( lock_ );
    KitLoggingClassificationMask_T oldMask  = classificationFilterMask_;
    classificationFilterMask_              |= classificationMask;
    return oldMask;
}

KitLoggingClassificationMask_T disableClassification( KitLoggingClassificationMask_T classificationMask ) noexcept
{
    Kit::System::Mutex::ScopeLock  criticalSection( lock_ );
    KitLoggingClassificationMask_T oldMask  = classificationFilterMask_;
    classificationFilterMask_              &= ~classificationMask;
    return oldMask;
}

KitLoggingClassificationMask_T getClassificationEnabledMask() noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( lock_ );
    return classificationFilterMask_;
}

void setClassificationMask( KitLoggingClassificationMask_T newMask ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( lock_ );
    classificationFilterMask_ = newMask;
}

////////////////////////////////////////////////////////////////////////////////
KitLoggingPackageMask_T enablePackage( KitLoggingPackageMask_T packageMask ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( lock_ );
    KitLoggingPackageMask_T       oldMask  = packageFilterMask_;
    packageFilterMask_                    |= packageMask;
    return oldMask;
}

KitLoggingPackageMask_T disablePackage( KitLoggingPackageMask_T packageMask ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( lock_ );
    KitLoggingPackageMask_T       oldMask  = packageFilterMask_;
    packageFilterMask_                    &= ~packageMask;
    return oldMask;
}

KitLoggingPackageMask_T getPackageEnabledMask() noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( lock_ );
    return packageFilterMask_;
}

void setPackageMask( KitLoggingPackageMask_T newMask ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( lock_ );
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