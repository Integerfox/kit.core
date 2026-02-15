#ifndef KIT_DM_MODELPOINTBASE_H_
#define KIT_DM_MODELPOINTBASE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Dm/IModelPoint.h"
#include "Kit/Dm/IModelDatabase.h"
#include "Kit/Dm/IObserver.h"
#include "Kit/Container/DList.h"
#include <stdint.h>
#include <stdlib.h>

///
namespace Kit {
///
namespace Dm {


/** This concrete class provide common infrastructure for a Model Point.
 */
class ModelPointBase : public Kit::Dm::IModelPoint
{
protected:
    /// Constructor
    ModelPointBase( IModelDatabase& myModelBase,
                       const char*     symbolicName,
                       void*           myDataPtr,
                       size_t          dataSizeInBytes,
                       bool            isValid = false );

public:
    /// See Kit::Dm::IModelPoint
    const char* getName() const noexcept override;

    /// See Kit::Dm::IModelPoint.
    size_t getSize() const noexcept override;

    /// See Kit::Dm::IModelPoint
    uint16_t getSequenceNumber() const noexcept override;

    /// See Kit::Dm::IModelPoint
    uint16_t touch() noexcept override;

    /// See Kit::Dm::IModelPoint
    uint16_t setInvalid( bool forceChgNotification = false, LockRequest_T lockRequest = eNO_REQUEST ) noexcept override;

    /// See Kit::Dm::IModelPoint
    bool isNotValid( uint16_t* seqNumPtr = 0 ) const noexcept override;

    /// See Kit::Dm::IModelPoint
    bool isLocked() const noexcept override;

    /// See Kit::Dm::IModelPoint
    uint16_t setLockState( LockRequest_T lockRequest ) noexcept override;

    /// See Kit::Dm::IModelPoint
    size_t getExternalSize( bool includeLockedState = false ) const noexcept override;

    /// See Kit::Dm::IModelPoint.  Note: The implementation does NOT account for Endianess, i.e. assumes the 'platform' is the same for export/import
    size_t exportData( void* dstDataStream, size_t maxDstLength, uint16_t* retSequenceNumber = 0, bool includeLockedState = false ) const noexcept override;

    /// See Kit::Dm::IModelPoint.  Note: The implementation does NOT account for Endianess, i.e. assumes the 'platform' is the same for export/import
    size_t importData( const void* srcDataStream, size_t srcLength, uint16_t* retSequenceNumber = 0, bool includeLockedState = false ) noexcept override;

    /// See Kit::Dm::IModelPoint
    void genericAttach( IObserver& observer, uint16_t initialSeqNumber = SEQUENCE_NUMBER_UNKNOWN ) noexcept override;

    /// See Kit::Dm::IModelPoint
    void genericDetach( IObserver& observer ) noexcept override;

    /// See Kit::Dm::IModelPoint
    bool toJSON( char* dst, size_t dstSize, bool& truncated, bool verbose = true, bool pretty = false ) noexcept override;

protected:
    /** This method is used to read the MP contents and synchronize
        the observer with the current MP contents.  This method should ONLY be
        used in the notification callback method and the 'observerToSync'
        argument MUST be the argument provided by the callback method

        Note: The observer will be subscribed for change notifications after
              this call.
     */
    inline bool readAndSync( void* dstData, size_t dstSize, uint16_t& seqNum, IObserver& observerToSync )
    {
        bool result = readData( dstData, dstSize, &seqNum );
        attachObserver( observerToSync, seqNum );
        return result;
    }

public:
    /** This method is used to test the validate state of the MP and synchronize
        the observer with the current MP contents.  This method should ONLY be
        used in the notification callback method and the 'observerToSync'
        argument MUST be the argument provided by the callback method

        Note: The observer will be subscribed for change notifications after
              this call
     */
    inline bool isNotValidAndSync( IObserver& observerToSync )
    {
        uint16_t seqNum;
        bool     result = isNotValid( &seqNum );
        attachObserver( observerToSync, seqNum );
        return result;
    }


protected:
    /// See Kit::Dm::IModelPoint
    bool readData( void*     dstData,
                   size_t    dstSize,
                   uint16_t* seqNumPtr = 0 ) const noexcept override;

    /// See Kit::Dm::IModelPoint
    uint16_t writeData( const void*   srcData,
                        size_t        srcSize,
                        bool          forceChgNotification = false,
                        LockRequest_T lockRequest          = eNO_REQUEST ) noexcept override;

    /// Updates the MP with the valid-state/data from 'src'. Note: the src.lock state is NOT copied
    virtual uint16_t copyDataAndStateFrom( const ModelPointBase& src, LockRequest_T lockRequest ) noexcept;

    /// See Kit::Dm::IModelPoint
    void attachObserver( IObserver& observer, uint16_t initialSeqNumber = SEQUENCE_NUMBER_UNKNOWN ) noexcept override;

    /// See Kit::Dm::IModelPoint
    void detachObserver( IObserver& observer ) noexcept override;

    /// See Kit::Dm::IModelPoint
    void copyDataTo_( void* dstData, size_t dstSize ) const noexcept override;

    /// See Kit::Dm::IModelPoint
    void copyDataFrom_( const void* srcData, size_t srcSize ) noexcept override;

    /// See Kit::Dm::IModelPoint. Note: This implementation does NOT work if the any of the data content are floats/double data types
    bool isDataEqual_( const void* otherData ) const noexcept override;

    /// See Kit::Dm::IModelPoint.
    void* getImportExportDataPointer_() noexcept override;

    /// See Kit::Dm::IModelPoint.
    size_t getInternalDataSize_() const noexcept override;

public:
    /// See Kit::Dm::IModelPoint
    void processSubscriptionEvent_( IObserver& observer, Event_T event ) noexcept override;


protected:
    /** Internal helper method that completes the data update process as well
        as ensuring any change notifications get generated.  Note: This method
        ALWAYS sets the MP's state to 'valid'

        This method is NOT thread safe.
     */
    virtual void processDataUpdated() noexcept;

    /** Internal helper method that handles generating change notifications
        when the Model Point's data/state has changed.

        This method is NOT thread safe.
     */
    virtual void processChangeNotifications() noexcept;

    /** Internal helper method that advances/updates the Model Point's
        sequence number.

        This method is NOT thread safe.
     */
    virtual void advanceSequenceNumber() noexcept;

    /** Internal helper method that manages testing and updating the locked
        state.

        Rules:
        1) If 'lockRequest' is eNO_REQUEST, the method only returns true if
           the MP is in the unlocked state
        2) If 'lockRequest' is eLOCK, the method only returns if the MP is in
           the unlocked state.  In addition, when true is returned the MP is
           put into the locked state.
        3) If 'lockRequest' is eUNLOCK, the method always returns true and
           the MP is left in the unlocked state.

        This method is NOT thread safe.
     */
    virtual bool testAndUpdateLock( LockRequest_T lockRequest ) noexcept;


protected:
    /// Helper FSM method
    virtual void transitionToNotifyPending( IObserver& observer ) noexcept;

    /// Helper FSM method
    virtual void transitionToSubscribed( IObserver& observer ) noexcept;

    /** Helper method when converting MP to a JSON string. 
        Note: endJSON() MUST always be called after this method to release the
        global JSON document
     */
    virtual JsonDocument& beginJSON( bool     isValid,
                                     bool     locked,
                                     uint16_t seqnum,
                                     bool     verbose = true ) noexcept;

    /// Helper method when converting MP to a JSON string
    virtual void endJSON( char*  dst,
                          size_t dstSize,
                          bool&  truncated,
                          bool   verbose = true,
                          bool   pretty  = false ) noexcept;

    /** Helper method that a child a class can override to change behavior when
        an MP is set to the invalid state.  The default behavior is to zero out
        the data (i.e. perform a memset(m_dataPtr,0, m_dataSize) call on the data)
     */
    virtual void hookSetInvalid() noexcept;


protected:
    /// List of Active Subscribers
    Kit::Container::DList<IObserver> m_subscribers;

    /// The model point's symbolic name
    const char* m_name;

    /// Reference to the containing Model Base
    IModelDatabase& m_modelDatabase;

    /// Reference to my Data
    void* m_dataPtr;

    /// Size of my data
    size_t m_dataSize;

    /// Sequence number used for tracking changes in the Point data
    uint16_t m_seqNum;

    /// Locked state
    bool m_locked;

    /// valid/invalid state
    bool m_valid;
};

};  // end namespaces
};
#endif  // end header latch
