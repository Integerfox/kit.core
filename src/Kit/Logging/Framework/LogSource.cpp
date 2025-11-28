/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "LogSource.h"
#include "EntryData.h"
#include "Kit/Logging/Framework/ILog.h"
#include "Kit/System/Mutex.h"
#include "Kit/System/Assert.h"
#include "Kit/System/Trace.h"
#include "Kit/Time/BootTime.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Logging {
namespace Framework {

////////////////////////////////////////////////////////////////////////////////
LogSource::LogSource( IApplication& appInstance,
                      /* Kit::Container::RingBufferMP<EntryData_T>& logEntryFIFO, */
                      uint8_t classificationIdForQueueOverflow,
                      uint8_t packageIdForQueueOverflow,
                      uint8_t subSystemIdForQueueOverflow,
                      uint8_t messageIdForQueueOverflow ) noexcept
    : m_app( appInstance )
    /*, m_logFifo( logEntryFIFO ) */
    , m_classificationFilterMask( 0xFFFFFFFF )
    , m_packageFilterMask( 0xFFFFFFFF )
    , m_overflowCount( 0 )
    , m_classificationIdForQueueOverflow( classificationIdForQueueOverflow )
    , m_packageIdForQueueOverflow( packageIdForQueueOverflow )
    , m_subSystemIdForQueueOverflow( subSystemIdForQueueOverflow )
    , m_messageIdForQueueOverflow( messageIdForQueueOverflow )
    , m_queueFull( false )
{
    KIT_SYSTEM_ASSERT( classificationIdForQueueOverflow > 0 && classificationIdForQueueOverflow <= 32 );
    KIT_SYSTEM_ASSERT( packageIdForQueueOverflow > 0 && packageIdForQueueOverflow <= 32 );

    // m_logFifo.clearTheBuffer();
}


////////////////////////////////////////////////////////////////////////////////

KitLoggingClassificationMask_T LogSource::enableClassification( KitLoggingClassificationMask_T classificationMask ) noexcept
{
    Kit::System::Mutex::ScopeLock  criticalSection( m_lock );
    KitLoggingClassificationMask_T oldMask  = m_classificationFilterMask;
    m_classificationFilterMask             |= classificationMask;
    return oldMask;
}

KitLoggingClassificationMask_T LogSource::disableClassification( KitLoggingClassificationMask_T classificationMask ) noexcept
{
    Kit::System::Mutex::ScopeLock  criticalSection( m_lock );
    KitLoggingClassificationMask_T oldMask  = m_classificationFilterMask;
    m_classificationFilterMask             &= ~classificationMask;
    return oldMask;
}

KitLoggingClassificationMask_T LogSource::getClassificationEnabledMask() noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_lock );
    return m_classificationFilterMask;
}

void LogSource::setClassificationMask( KitLoggingClassificationMask_T newMask ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_lock );
    m_classificationFilterMask = newMask;
}

////////////////////////////////////////////////////////////////////////////////
KitLoggingPackageMask_T LogSource::enablePackage( KitLoggingPackageMask_T packageMask ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_lock );
    KitLoggingPackageMask_T       oldMask  = m_packageFilterMask;
    m_packageFilterMask                   |= packageMask;
    return oldMask;
}
KitLoggingPackageMask_T LogSource::disablePackage( KitLoggingPackageMask_T packageMask ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_lock );
    KitLoggingPackageMask_T       oldMask  = m_packageFilterMask;
    m_packageFilterMask                   &= ~packageMask;
    return oldMask;
}
KitLoggingPackageMask_T LogSource::getPackageEnabledMask() noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_lock );
    return m_packageFilterMask;
}
void LogSource::setPackageMask( KitLoggingPackageMask_T newMask ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_lock );
    m_packageFilterMask = newMask;
}

////////////////////////////////////////////////////////////////////////////////
void LogSource::createAndAddOverflowEntry() noexcept
{
#if 0
    // Generate entry
    EntryData_T logEntry;
    logEntry.classification = overflowClassificationId_;
    logEntry.msgId          = overflowMsgId_;
    logEntry.timestamp      = Kit::Time::getBootTime();

    // Format text
    Kit::Text::FString<OPTION_KIT_LOGGING_FRAMEWORK_MAX_FORMATTED_MSG_TEXT_LEN> stringBuf;
    startText( stringBuf, m_classificationTextForQueueOverflow, overflowMsgText_ );
    stringBuf.formatAppend( "overflow count=%d", m_overflowCount );

    // Add to the FIFO and echo to trace
    // logEntryFIFO_->add( logEntry );
    KIT_SYSTEM_TRACE_MSG( m_classificationTextForQueueOverflow,  "%s", stringBuf.getString() ) );
#endif
}

bool LogSource::isQueFull() noexcept
{
    // Return immediately if not in the overflowed state
    if ( !m_queueFull )
    {
        return false;
    }

    // // Has space freed up?
    // unsigned available = logEntryFIFO_->getMaxItems() - logEntryFIFO_->getNumItems();
    // if ( available >= OPTION_KIT_LOGGING_FRAMEWORK_MIN_QUEUE_SPACE )
    // {
    //     createAndAddOverflowEntry();
    //     m_overflowCount = 0;
    //     m_queueFull     = false;
    //     return false;
    // }

    // No space - count the number of 'dropped' log entries
    m_overflowCount++;
    return true;
}


////////////////////////////////////////////////////////////////////////////////
LogSource::LogResult_T LogSource::vlogf( uint8_t     classificationId,
                                         uint8_t     packageId,
                                         uint8_t     subSystemId,
                                         uint8_t     messageId,
                                         const char* formatInfoText,
                                         va_list     ap ) noexcept
{
    LogResult_T                   result = FILTERED;
    Kit::System::Mutex::ScopeLock criticalSection( m_lock );

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
    if ( ( classificationId & m_classificationFilterMask ) &&
         ( packageId & m_packageFilterMask ) )
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
            //     m_queueFull = true;
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
        Formatter::toString( m_app, logEntry, m_workBuffer );
        KIT_SYSTEM_TRACE_RESTRICTED_MSG( m_app.classificationIdToString( classificationId ), "%s", m_workBuffer.getString() );
    }

    return result;
}

}  // end namespace
}
}
//------------------------------------------------------------------------------