#ifndef KIT_DM_PERSISTENCE_RECORD_H_
#define KIT_DM_PERSISTENCE_RECORD_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Record/IRecord.h"
#include "Kit/Persistence/Record/IChunk.h"
#include "Kit/Persistence/Record/IPayload.h"
#include "Kit/EventQueue/IQueue.h"
#include "Kit/Dm/IModelPoint.h"
#include "Kit/Dm/ObserverCallback.h"
#include "Kit/Dm/Persistence/IManageRequest.h"
#include "Kit/System/Timer.h"
#include "Kit/System/Assert.h"

///
namespace Kit {
///
namespace Dm {
///
namespace Persistence {

/** This mostly concrete class implements the Kit::Persistence::Record::IRecord
    interface where a Record instance contains the data from N model points.  A
    final child class is needed to provide the specifics of 'resetting' the
    Record's data.

    NOTE: There can be multiple MPs that make up the Record and it is possible
          to get change notification for all MPs, i.e. change notifications
          are not atomic to the SET of MPs that make up Record.  However, the
          application can enable the 'settling time' feature of the Record to
          minimize the number of 'extra' NVRAM updates.
 */
class Record : public Kit::Persistence::Record::IRecord,
               public Kit::Persistence::Record::IPayload,
               public Kit::Dm::Persistence::IManageRequest
{
public:
    /** This data structure associates a Data Model subscriber instance with a
        Model Point instance.
     */
    struct Item_T
    {
        Kit::Dm::IModelPoint*                   mpPtr;        //!< Pointer to an allocated Model Point
        Kit::Dm::ObserverCallback<IModelPoint>* observerPtr;  //!< Pointer to an allocated observer, or nullptr if no runtime monitor of changes to the MP is desired to trigger an Record update
    };

    /// Magic value for no-change notifications for a MP in the Item_T list
    static constexpr Kit::Dm::IObserver* NO_SUBSCRIBER = nullptr;

public:
    /** Constructor.  The 'itemList' is array of model points/observer tuples.
        The application is responsible for ALLOCATING all of the Model Point
        and Observer instances in the itemList.


        If the schema indexes does not match when reading data from persistent
        storage, THEN the `schemaChange()` method is called.  If the `schemaChange()`
        method returns false, then the record is defaulted.

        The 'writeDelayMs' and 'maxWriteDelayMs' arguments are used to impose
        a 'settling time' after an model point is updated before writing the
        change(s) to NVRAM.  This is useful when multiple MP changes at the
        'same' time - so that there is single update to NVRAM instead of
        attempting multiple NVRAM writes for each MP change notification.
     */
    Record( Item_T                            itemList[],
            unsigned                          numItems,
            Kit::Persistence::Record::IChunk& chunkHandler,
            uint8_t                           schemaMajorIndex,
            uint8_t                           schemaMinorIndex,
            uint32_t                          writeDelayMs    = 0,
            uint32_t                          maxWriteDelayMs = 0 ) noexcept
        : m_timer( *this, &Record::timerExpired )
        , m_myEventQueuePtr( nullptr )
        , m_items( itemList )
        , m_chunkHandler( chunkHandler )
        , m_delayMs( writeDelayMs )
        , m_maxDelayMs( maxWriteDelayMs )
        , m_timerMarker( 0 )
        , m_numItems( numItems )
        , m_major( schemaMajorIndex )
        , m_minor( schemaMinorIndex )
        , m_started( false )
    {
        KIT_SYSTEM_ASSERT( itemList );
        KIT_SYSTEM_ASSERT( numItems > 0 );
    }

    /// Destructor
    virtual ~Record()
    {
        stop();
    }

public:
    /// See Kit::Persistence::Record::IRecord
    bool start( Kit::EventQueue::IQueue& myEventQueue ) noexcept override;

    /// See Kit::Persistence::Record::IRecord
    void stop() noexcept override;

    /// See Kit::Persistence::Record::IRecord
    Kit::Persistence::Size_T getSize() const noexcept override;

public:
   /// See Kit::Persistence::Record::IPayload
    Kit::Persistence::Size_T copyTo( void* dst, Kit::Persistence::Size_T maxDstLen ) noexcept override;

    /// See Kit::Persistence::Record::IPayload
    bool copyFrom( const void* src, Kit::Persistence::Size_T srcLen ) noexcept override;

    /// See Kit::Persistence::Record::IPayload
    Kit::Persistence::Size_T getMaxPayloadSize() const noexcept override;


public:
    /** Synchronous Flush/update of the Record to persistent storage.
        Note: Use this method with CAUTION.  The call will BLOCK the calling
        thread for an undetermined amount of time!!
     */
    bool flush() noexcept;

    /** Synchronous Invalidate/logically-erase of the Record in persistent storage.
        Note: Use this method with CAUTION.  The call will BLOCK the calling
        thread for an undetermined amount of time!!
     */
    bool erase() noexcept;

public:
    /// See Kit::Dm::Persistence::IManage
    void request( FlushMsg& msg ) noexcept override;

    /// See Kit::Dm::Persistence::IManage
    void request( EraseMsg& msg ) noexcept override;

protected:
    /** This method is responsible for updating all of the Model Points in
        record to their default values.  This method is called when there is
        NO valid data when reading the record's data from persistence storage.

        When the method returns true, the 'reset data' is written to persistence
        storage; else the persistence storage is NOT updated and NO further updates
        of the persistence storage will be allowed.
     */
    virtual bool resetData() noexcept = 0;

    /** This method is called when the stored record data has different
        schema indexes. It is up to the child record class on what to do when
        the schema is different.  The default behavior is default all of the
        data.

        The 'src' argument contains the record data (as stored in persistent
        storage).  The child class can make use of the data as needed.  Note:
        the data in 'src' is AFTER the schema indexes fields.

        The method should return true if the child class has successfully
        processed in the incoming data.  Returning false will cause the
        record data to be defaulted;
     */
    virtual bool schemaChange( uint8_t     previousSchemaMajorIndex,
                               uint8_t     previousSchemaMinorIndex,
                               const void* src,
                               size_t      srcLen ) noexcept
    {
        return false;
    }

    /** This method is called when the Record has been successfully loaded into
        RAM/Model-Points.  The default action does nothing.
     */
    virtual void hookProcessPostRecordLoaded() noexcept {};

    /// Settling timer expired callback
    virtual void timerExpired( void ) noexcept;


protected:
    /// Callback method for Model Point change notifications
    virtual void dataChanged( Kit::Dm::IModelPoint& point, Kit::Dm::IObserver& observer ) noexcept;

    /// Helper method that is used to initiate the update to the NVRAM
    virtual void updateNVRAM() noexcept;

protected:
    /// Software Timer
    Kit::System::TimerComposer<Record> m_timer;

    /// The Event Queue that I am running in (used for my timer)
    Kit::EventQueue::IQueue* m_myEventQueuePtr;

    /// List of model points
    Item_T* m_items;

    /// Chunk handler for the Record
    Kit::Persistence::Record::IChunk& m_chunkHandler;

    /// Delay time, in milliseconds, when updated NVRAM (i.e. 'settling time' after an MP update before writing NVRAM)
    uint32_t m_delayMs;

    /// Maximum amount of time, in milliseconds, to delay before updating NVRAM
    uint32_t m_maxDelayMs;

    /// Timer marker of the 'first' MP change notification (for a NVRAM update)
    uint32_t m_timerMarker;

    /// Number of items in the Item_T list
    unsigned m_numItems;

    /// Schema Major version
    uint8_t m_major;

    /// Schema Minor version
    uint8_t m_minor;

    /// Remember my started state
    bool m_started;
};


}  // end namespaces
}
}
#endif  // end header latch