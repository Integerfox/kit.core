/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "ModelPointBase.h"
#include "Kit/System/Mutex.h"
#include "Kit/EventQueue/IChangeNotification.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Assert.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Dm {

/// IObserver States
enum State_T
{
    eSTATE_UNSUBSCRIBED = 0,       // Not subscribed to any model point
    eSTATE_IDLE,                   // Subscribed and waiting for a change
    eSTATE_NOTIFY_PENDING,         // Subscribed and waiting for next change notification dispatch cycle
    eSTATE_NOTIFY_NOTIFYING,       // The Client change notification callback is being executed
    eSTATE_NOTIFY_PENDING_DETACH,  // The subscription was requested to be cancelled during the change notification callback
};


////////////////////////
ModelPointBase::ModelPointBase( IModelDatabase& myModelBase,
                                const char*     symbolicName,
                                void*           myDataPtr,
                                size_t          dataSizeInBytes,
                                bool            isValid )
    : m_name( symbolicName )
    , m_modelDatabase( myModelBase )
    , m_dataPtr( myDataPtr )
    , m_dataSize( dataSizeInBytes )
    , m_seqNum( IModelPoint::SEQUENCE_NUMBER_UNKNOWN + 1 )
    , m_locked( false )
    , m_valid( isValid )
{
    // Automagically add myself to the Model Database
    m_modelDatabase.insert_( *this );

    // Make sure that I process the 'transition' to the invalid state
    if ( !m_valid )
    {
        hookSetInvalid();
    }
}

/////////////////
const char* ModelPointBase::getName() const noexcept
{
    return m_name;
}

size_t ModelPointBase::getSize() const noexcept
{
    return m_dataSize;
}

uint16_t ModelPointBase::getSequenceNumber() const noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_modelDatabase.getMutex_() );
    return m_seqNum;
}

bool ModelPointBase::isNotValid( uint16_t* seqNumPtr ) const noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_modelDatabase.getMutex_() );
    //
    bool result = m_valid;
    if ( seqNumPtr )
    {
        *seqNumPtr = m_seqNum;
    }
    return !result;
}

uint16_t ModelPointBase::setInvalid( bool forceChgNotification, LockRequest_T lockRequest ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_modelDatabase.getMutex_() );
    if ( testAndUpdateLock( lockRequest ) )
    {
        if ( m_valid || forceChgNotification )
        {
            m_valid = false;
            hookSetInvalid();
            processChangeNotifications();
        }
    }

    return m_seqNum;
}

void ModelPointBase::hookSetInvalid() noexcept
{
    // Set the data to a known state so that transition from the invalid to the
    // valid state when using read-modify-write operation is consistent in its behavior
    memset( m_dataPtr, 0, m_dataSize );
}

bool ModelPointBase::readData( void* dstData, size_t dstSize, uint16_t* seqNumPtr ) const noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_modelDatabase.getMutex_() );
    //
    bool valid = m_valid;
    if ( dstData && valid )
    {
        copyDataTo_( dstData, dstSize );
    }
    if ( seqNumPtr )
    {
        *seqNumPtr = m_seqNum;
    }

    return valid;
}

uint16_t ModelPointBase::writeData( const void* srcData, size_t srcSize, bool forceChgNotification, LockRequest_T lockRequest ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_modelDatabase.getMutex_() );
    if ( srcData && testAndUpdateLock( lockRequest ) )
    {
        if ( !m_valid || forceChgNotification || isDataEqual_( srcData ) == false )
        {
            copyDataFrom_( srcData, srcSize );
            processDataUpdated();
        }
    }
    return m_seqNum;
}

uint16_t ModelPointBase::copyDataAndStateFrom( const ModelPointBase& src, LockRequest_T lockRequest ) noexcept
{
    // Handle the src.invalid case
    if ( src.isNotValid() )
    {
        return setInvalid();
    }

    Kit::System::Mutex::ScopeLock criticalSection( m_modelDatabase.getMutex_() );
    return writeData( src.m_dataPtr, src.m_dataSize, false, lockRequest );
}

uint16_t ModelPointBase::touch() noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_modelDatabase.getMutex_() );
    processChangeNotifications();
    return m_seqNum;
}

void ModelPointBase::copyDataTo_( void* dstData, size_t dstSize ) const noexcept
{
    KIT_SYSTEM_ASSERT( dstSize <= m_dataSize );
    memcpy( dstData, m_dataPtr, dstSize );
}

void ModelPointBase::copyDataFrom_( const void* srcData, size_t srcSize ) noexcept
{
    KIT_SYSTEM_ASSERT( srcSize <= m_dataSize );
    memcpy( m_dataPtr, srcData, m_dataSize );
}

bool ModelPointBase::isDataEqual_( const void* otherData ) const noexcept
{
    return memcmp( m_dataPtr, otherData, m_dataSize ) == 0;
}

void* ModelPointBase::getImportExportDataPointer_() noexcept
{
    return m_dataPtr;
}

size_t ModelPointBase::getInternalDataSize_() const noexcept
{
    return getSize();
}

/////////////////
bool ModelPointBase::toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose, bool pretty ) noexcept
{
    bool result = true;

    // Get a snapshot of the my data and state
    Kit::System::Mutex::ScopeLock criticalSection( m_modelDatabase.getMutex_() );

    // Start the conversion
    JsonDocument& doc = beginJSON( m_valid, m_locked, m_seqNum, verbose );

    // Construct the 'val' key/value pair (as a simple numeric)
    if ( m_valid )
    {
        result = setJSONVal( doc );
    }

    // End the conversion
    endJSON( dst, dstSize, truncated, verbose, pretty );
    return result;
}

/////////////////
uint16_t ModelPointBase::setLockState( LockRequest_T lockRequest ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_modelDatabase.getMutex_() );
    if ( lockRequest == eLOCK )
    {
        m_locked = true;
    }
    else if ( lockRequest == eUNLOCK )
    {
        m_locked = false;
    }
    return m_seqNum;
}

bool ModelPointBase::isLocked() const noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_modelDatabase.getMutex_() );
    return m_locked;
}


/////////////////
size_t ModelPointBase::exportData( void* dstDataStream, size_t maxDstLength, uint16_t* retSeqNum, bool includeLockedState ) const noexcept
{
    size_t result = 0;
    if ( dstDataStream )
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_modelDatabase.getMutex_() );

        // Do nothing if there is not enough space left in the destination stream
        if ( maxDstLength >= getExternalSize() )
        {
            // Export metadata (if there is any)
            size_t bytesAdded = 0;
            if ( exportMetadata_( dstDataStream, bytesAdded ) )
            {
                // Export Data
                uint8_t* dstPtr   = static_cast<uint8_t*>( dstDataStream ) + bytesAdded;
                size_t   dataSize = getSize();
                memcpy( dstPtr, const_cast<ModelPointBase*>( this )->getImportExportDataPointer_(), dataSize );

                // Export Valid State
                memcpy( dstPtr + dataSize, &m_valid, sizeof( m_valid ) );

                // Export Locked state
                if ( includeLockedState )
                {
                    memcpy( dstPtr + dataSize + sizeof( m_valid ), &m_locked, sizeof( m_locked ) );
                }

                // Return the Sequence number when requested
                if ( retSeqNum )
                {
                    *retSeqNum = m_seqNum;
                }

                result = getExternalSize( includeLockedState );
            }
        }
    }
    return result;
}

size_t ModelPointBase::importData( const void* srcDataStream, size_t srcLength, uint16_t* retSeqNum, bool includeLockedState ) noexcept
{
    size_t result = 0;
    if ( srcDataStream )
    {
        Kit::System::Mutex::ScopeLock criticalSection( m_modelDatabase.getMutex_() );

        // Fail the import when there is not enough data left in the input stream
        if ( getExternalSize() <= srcLength )
        {
            // Consume incoming metadata (if there is any)
            size_t bytesConsumed = 0;
            if ( importMetadata_( srcDataStream, bytesConsumed ) )
            {
                // Import Data
                const uint8_t* srcPtr   = static_cast<const uint8_t*>( srcDataStream ) + bytesConsumed;
                size_t         dataSize = getSize();
                memcpy( getImportExportDataPointer_(), srcPtr, dataSize );

                // Import Valid State
                memcpy( &m_valid, srcPtr + dataSize, sizeof( m_valid ) );

                // Import Locked state
                if ( includeLockedState )
                {
                    memcpy( &m_locked, srcPtr + dataSize + sizeof( m_valid ), sizeof( m_locked ) );
                }

                // Generate change notifications and return the Sequence number when requested
                processChangeNotifications();
                if ( retSeqNum )
                {
                    *retSeqNum = m_seqNum;
                }

                result = getExternalSize( includeLockedState );
            }
        }
    }
    return result;
}

size_t ModelPointBase::getExternalSize( bool includeLockedState ) const noexcept
{
    size_t baseSize = getInternalDataSize_() + sizeof( m_valid );
    return includeLockedState ? baseSize + sizeof( m_locked ) : baseSize;
}


/////////////////////////////////////
void ModelPointBase::processDataUpdated() noexcept
{
    // By definition - Point now has valid date
    m_valid = true;
    processChangeNotifications();
}

void ModelPointBase::advanceSequenceNumber() noexcept
{
    // Increment my sequence number (when rolling over -->do not allow the 'unknown' value)
    m_seqNum++;
    if ( m_seqNum == SEQUENCE_NUMBER_UNKNOWN )
    {
        m_seqNum = SEQUENCE_NUMBER_UNKNOWN + 1;
    }
}

void ModelPointBase::processChangeNotifications() noexcept
{
    // Increment the sequence number
    advanceSequenceNumber();

    // Generate change notifications
    IObserver* item = m_subscribers.get();
    while ( item )
    {
        processSubscriptionEvent_( *item, eDATA_CHANGED );
        item = m_subscribers.get();
    }
}

/////////////////
void ModelPointBase::attachObserver( IObserver& observer, uint16_t initialSeqNumber ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_modelDatabase.getMutex_() );
    observer.setSequenceNumber_( initialSeqNumber );
    observer.setModelPoint_( this );
    processSubscriptionEvent_( observer, eATTACH );
}

void ModelPointBase::detachObserver( IObserver& observer ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_modelDatabase.getMutex_() );
    processSubscriptionEvent_( observer, eDETACH );
    observer.setModelPoint_( 0 );
}

void ModelPointBase::genericAttach( IObserver& observer, uint16_t initialSeqNumber ) noexcept
{
    attachObserver( observer, initialSeqNumber );
}

void ModelPointBase::genericDetach( IObserver& observer ) noexcept
{
    detachObserver( observer );
}


/////////////////
void ModelPointBase::processSubscriptionEvent_( IObserver& observer, Event_T event ) noexcept
{
    Kit::System::Mutex::ScopeLock criticalSection( m_modelDatabase.getMutex_() );

    switch ( static_cast<State_T>(observer.getState_()) )
    {
    case eSTATE_UNSUBSCRIBED:
        switch ( event )
        {
        case eATTACH:
            transitionToSubscribed( observer );
            break;

        case eDATA_CHANGED:
            Kit::System::FatalError::logf( Kit::System::Shutdown::eDATA_MODEL,
                                           "ModelPointBase::processSubscriptionEvent_(): Data changed received when in the eSTATE_UNSUBSCRIBED state!" );
            break;

            // Ignore all other events
        default:
            break;
        }
        break;

    case eSTATE_IDLE:
        switch ( event )
        {
        case eATTACH:
            transitionToSubscribed( observer );
            break;

        case eDETACH:
            m_subscribers.remove( observer );
            observer.setState_( eSTATE_UNSUBSCRIBED );
            break;

        case eDATA_CHANGED:
            // NOTE: By definition if the eDATA_CHANGED event was generated - the observer is NOT in the MP's subscribers list
            transitionToNotifyPending( observer );
            break;

            // Ignore all other events
        default:
            break;
        }
        break;

    case eSTATE_NOTIFY_PENDING:
        switch ( event )
        {
        case eATTACH:
            observer.getIChangeNotification_()->removePendingChangingNotification( observer );
            transitionToSubscribed( observer );
            break;

        case eDETACH:
            observer.getIChangeNotification_()->removePendingChangingNotification( observer );
            observer.setState_( eSTATE_UNSUBSCRIBED );
            break;

        case eNOTIFYING:
            observer.setSequenceNumber_( m_seqNum );
            observer.setState_( eSTATE_NOTIFY_NOTIFYING );
            break;

        case eDATA_CHANGED:
            Kit::System::FatalError::logf( Kit::System::Shutdown::eDATA_MODEL,
                                           "ModelPointBase::processSubscriptionEvent_(): Data changed received when in the eSTATE_NOTIFY_PENDING state!" );
            break;

            // Ignore all other events
        default:
            break;
        }
        break;

    case eSTATE_NOTIFY_NOTIFYING:
        switch ( event )
        {
        case eDETACH:
            observer.setState_( eSTATE_NOTIFY_PENDING_DETACH );
            break;

        case eNOTIFY_COMPLETE:
            transitionToSubscribed( observer );
            break;

        case eDATA_CHANGED:
            Kit::System::FatalError::logf( Kit::System::Shutdown::eDATA_MODEL,
                                           "ModelPointBase::processSubscriptionEvent_(): Data changed received when in the eSTATE_NOTIFY_NOTIFYING state!" );
            break;

            // Ignore all other events
        default:
            break;
        }
        break;

    case eSTATE_NOTIFY_PENDING_DETACH:
        switch ( event )
        {
        case eATTACH:
            observer.setState_( eSTATE_NOTIFY_NOTIFYING );
            break;

        case eNOTIFY_COMPLETE:
            observer.setState_( eSTATE_UNSUBSCRIBED );
            break;

        case eDATA_CHANGED:
            Kit::System::FatalError::logf( Kit::System::Shutdown::eDATA_MODEL,
                                           "ModelPointBase::processSubscriptionEvent_(): Data changed received when in the eSTATE_NOTIFY_PENDING_DETACH state!" );
            break;

            // Ignore all other events
        default:
            break;
        }
        break;
    }
}

void ModelPointBase::transitionToSubscribed( IObserver& observer ) noexcept
{
    // Ensure that I am not already in the Model Point's list of subscribers (this can happen if subscribing when I am already subscribed)
    m_subscribers.remove( observer );

    if ( m_seqNum == observer.getSequenceNumber_() )
    {
        observer.setState_( eSTATE_IDLE );
        m_subscribers.put( observer );
    }
    else
    {
        return transitionToNotifyPending( observer );
    }
}

void ModelPointBase::transitionToNotifyPending( IObserver& observer ) noexcept
{
    observer.getIChangeNotification_()->addPendingChangingNotification( observer );
    observer.setState_( eSTATE_NOTIFY_PENDING );
}


/////////////////
bool ModelPointBase::testAndUpdateLock( LockRequest_T lockRequest ) noexcept
{
    bool result = false;
    if ( lockRequest == eUNLOCK )
    {
        m_locked = false;
        result   = true;
    }
    else if ( lockRequest == eLOCK )
    {
        m_locked = true;
        result   = true;
    }
    else
    {
        if ( m_locked == false )
        {
            result = true;
        }
    }

    return result;
}

/////////////////
JsonDocument& ModelPointBase::beginJSON( bool isValid, bool locked, uint16_t seqnum, bool verbose ) noexcept
{
    // Get access to the Global JSON document
    IModelDatabase::getGlobalMutex_().lock();
    IModelDatabase::g_doc_.clear();  // Make sure the JSON document is starting "empty"

    // Construct the JSON
    IModelDatabase::g_doc_["name"]  = getName();
    IModelDatabase::g_doc_["valid"] = isValid;
    if ( verbose )
    {
        IModelDatabase::g_doc_["type"]   = getTypeAsText();
        IModelDatabase::g_doc_["seqnum"] = seqnum;
        IModelDatabase::g_doc_["locked"] = locked;
    }
    return IModelDatabase::g_doc_;
}

void ModelPointBase::endJSON( char* dst, size_t dstSize, bool& truncated, bool verbose, bool pretty ) noexcept
{
    size_t jsonLen;
    size_t outputLen;

    // Generate the actual output string
    if ( !pretty )
    {
        jsonLen   = measureJson( IModelDatabase::g_doc_ );
        outputLen = serializeJson( IModelDatabase::g_doc_, dst, dstSize );
    }
    else
    {
        jsonLen   = measureJsonPretty( IModelDatabase::g_doc_ );
        outputLen = serializeJsonPretty( IModelDatabase::g_doc_, dst, dstSize );
    }
    truncated = outputLen == jsonLen ? false : true;

    // Release the Global JSON document
    IModelDatabase::getGlobalMutex_().unlock();
}

}  // end namespace
}
//------------------------------------------------------------------------------