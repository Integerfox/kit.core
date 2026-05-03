#ifndef KIT_PERSISTENCE_RECORD_INDEXED_ENTRYRECORD_H
#define KIT_PERSISTENCE_RECORD_INDEXED_ENTRYRECORD_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Persistence/Record/Indexed/IEntry.h"
#include "Kit/Persistence/Record/Indexed/IHead.h"
#include "Kit/Persistence/Record/IMedia.h"
#include "Kit/Persistence/Record/IChunk.h"

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Indexed {

/** This concrete class implements the IRecord interface that manages a collection
    of Entries that are timestamp and oldest entries are silently lost when the
    IMedia storage is 'full'

    This class is responsible for managing the 'head pointer' for the logical
    RingBuffer that is used to manage multiple entries.
*/
class EntryRecord : public IEntry 
{
public:
    /// Constructor.
    EntryRecord( IChunk& entryChunkHandler,
                 Size_T  singleEntrySizeInBytes,
                 IMedia& entryMedia,
                 IHead&  headRecord ) noexcept
        : m_chunk( entryChunkHandler )
        , m_headRecord( headRecord )
        , m_entryMedia( entryMedia )
        , m_entryPayloadHandlerPtr( nullptr )
        , m_entrySize( singleEntrySizeInBytes + getEntryMetadataSize() + entryChunkHandler.getMetadataLength() )
        , m_maxEntries( entryMedia.getMaxSize() / ( m_entrySize ) )
        , m_maxOffset( ( m_maxEntries - 1 ) * m_entrySize )
        , m_started( false )
    {
    }

public:
    /// See Kit::Persistence::Record::IRecord
    bool start( Kit::EventQueue::IQueue& myEventQueue ) noexcept override;

    /// See Kit::Persistence::Record::IRecord
    void stop() noexcept override;

    /// See Kit::Persistence::Record::IRecord
    Size_T getSize() const noexcept override;

public:
    /// See Kit::Persistence::Record::IPayload
    Size_T copyTo( void* dst, Size_T maxDstLen ) noexcept override;

    /// See Kit::Persistence::Record::IPayload
    bool copyFrom( const void* src, Size_T srcLen ) noexcept override;

    /// See Kit::Persistence::Record::IPayload
    Size_T getMaxPayloadSize() const noexcept override;

public:
    /// See Kit::Persistence::Record::Indexed::IEntry
    bool getLatest( IPayload& dst, Marker_T& entryMarker ) noexcept override;

    /// See Kit::Persistence::Record::Indexed::IEntry
    bool getNext( uint64_t       newerThanTimestamp,
                  const Marker_T beginHereMarker,
                  IPayload&      dst,
                  Marker_T&      entryMarker ) noexcept override;

    /// See Kit::Persistence::Record::Indexed::IEntry
    bool getPrevious( uint64_t       olderThanTimestamp,
                      const Marker_T beginHereMarker,
                      IPayload&      dst,
                      Marker_T&      entryMarker ) noexcept override;

    /// See Kit::Persistence::Record::Indexed::IEntry
    bool getByEntryIndex( size_t    entryIndex,
                          IPayload& dst,
                          Marker_T& entryMarker ) noexcept override;

                          /// See Kit::Persistence::Record::Indexed::IEntry
    size_t getMaxIndex() const noexcept override;

    /// See Kit::Persistence::Record::Indexed::IEntry
    bool addEntry( const IPayload& src ) noexcept override;

    /// See Kit::Persistence::Record::Indexed::IEntry
    void resetHead() noexcept override;

    /// See Kit::Persistence::Record::Indexed::IEntry
    bool eraseAllEntries() noexcept override;

public:
    /// See Kit::Persistence::Record::IDataRecord
    bool writeToMedia( Size_T index = 0 ) noexcept override;

    /// See Kit::Persistence::Record::IDataRecord
    bool readFromMedia( Size_T index = 0 ) noexcept override;

    /// See Kit::Persistence::Record::IDataRecord
    bool processNoValidData() noexcept override;

protected:
    /// Hook for a derived classes to add processing when m_latestTimestamp is updated.
    virtual void hookOnLatestTimestampUpdated( uint64_t newTimestampValue ) noexcept { /* Default does nothing */ }

    /// Helper method: Verifies the 'correctness' of the index/head pointer and 'fixes' the head pointer if it is 'bad'
    void verifyIndex() noexcept;

    /// Helper method: scans all of the 'flash' and returns the newest value found.  If no valid entries found - everything is set to zero
    void scanAllEntries();

    /// Helper method: 'increments' the offset by the size of entry (and handles the 'roll-over' case)
    size_t incrementOffset( Size_T offsetToIncrement ) const noexcept;

    /// Helper method: 'decrements' the offset by the size of entry (and handles the 'roll-over' case)
    size_t decrementOffset( Size_T offsetToDecrement ) const noexcept;

    /// Helper method: get an entry by its offset
    bool getByOffset( Size_T            offset,
                      IPayload&         dst,
                      IEntry::Marker_T& entryMarker ) noexcept;

    /// Helper method - returns number of bytes of meta data for each entry
    inline Size_T getEntryMetadataSize() const noexcept
    {
        return sizeof( m_latestTimestamp );
    }

protected:
    /// Reference to the chunk handler for this record
    Kit::Persistence::Record::IChunk& m_chunk;

    /// Timestamp value of the latest record
    uint64_t m_latestTimestamp;

    /// Timestamp value to store/push when reading/writing an entry to persistent storage
    uint64_t m_entryTimestamp;

    /// Head record
    IHead& m_headRecord;

    /// IMedia for the entries
    IMedia& m_entryMedia;

    /// Application specific Payload handler for an entry
    IPayload* m_entryPayloadHandlerPtr;

    /// Size, in bytes, of individual Entry W/the EntryRecord's meta data
    Size_T m_entrySize;

    /// Maximum number of entry that can be stored in the allocated space
    Size_T m_maxEntries;

    /// The maximum offset value that can be used to store entry without exceeded the allocate entry space
    Size_T m_maxOffset;

    /// Offset of the latest record
    Size_T m_latestOffset;

    /// Track the started state
    bool m_started;
};

}  // end namespaces
}
}
}
#endif  // end header latch
