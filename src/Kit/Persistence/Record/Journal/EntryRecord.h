#ifndef KIT_PERSISTENCE_RECORD_JOURNAL_ENTRYRECORD_H
#define KIT_PERSISTENCE_RECORD_JOURNAL_ENTRYRECORD_H
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
#include "Kit/Persistence/Record/Journal/IEntry.h"
#include "Kit/Persistence/Record/Journal/IHead.h"
#include "Kit/Persistence/Record/IMedia.h"
#include "Kit/Persistence/Record/IChunk.h"
#include "Kit/System/Assert.h"

/** The maximum number of consecutive corrupt entries to scan during recovery 
    of the Record.  The larger this number, the more resilient the recovery
    process is to consecutive corrupt entries, but it may increase the recovery
    time - this includes the initial 'virgin' boot of the persistent storage.
 */
#ifndef OPTION_KIT_PERSISTENCE_JOURNAL_ENTRY_RECORD_MAX_CORRUPT_SCAN
#define OPTION_KIT_PERSISTENCE_JOURNAL_ENTRY_RECORD_MAX_CORRUPT_SCAN 8
#endif

/** Number of consecutive corrupt entries skip over when using getNext/getPrevious 
    to traverse the entries before declaring an error.  This provides fault tolerance
    against entries that may have been corrupted when a power-loss occurs while writing
    the entry to persistent storage.
 */
#ifndef OPTION_KIT_PERSISTENCE_JOURNAL_ENTRY_RECORD_MAX_CONSECUTIVE_CORRUPT_SKIP
#define OPTION_KIT_PERSISTENCE_JOURNAL_ENTRY_RECORD_MAX_CONSECUTIVE_CORRUPT_SKIP 3
#endif

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Journal {

/** This concrete class implements the IRecord interface that manages a collection
    of Entries that are timestamp and oldest entries are silently lost when the
    IMedia storage is 'full'

    This class is responsible for managing the 'head pointer' for the logical
    RingBuffer that is used to manage multiple entries.
*/
class EntryRecord : public IEntry
{
public:
    /** Constructor.
        @param entryChunkHandler          Chunk handler used to read/write each entry
        @param singleEntrySizeInBytes     Size of the application payload for one entry, in bytes
        @param entryMedia                 IMedia region that holds the entries
        @param headRecord                 IHead instance that persists the ring-buffer head pointer
        @param maxCorruptScan             The maximum number of consecutive corrupt entries to walk
                                          back over when recovering the head pointer at startup.
        @param maxConsecutiveCorruptSkip  The maximum number of consecutive corrupt entries to skip
                                          over when traversing entries via getNext()/getPrevious()
                                          before declaring a failure.

        NOTE: The Application MUST allocate storage for at least 2 entries 
    */
    EntryRecord( IChunk& entryChunkHandler,
                 Size_T  singleEntrySizeInBytes,
                 IMedia& entryMedia,
                 IHead&  headRecord,
                 Size_T  maxCorruptScan            = OPTION_KIT_PERSISTENCE_JOURNAL_ENTRY_RECORD_MAX_CORRUPT_SCAN,
                 Size_T  maxConsecutiveCorruptSkip = OPTION_KIT_PERSISTENCE_JOURNAL_ENTRY_RECORD_MAX_CONSECUTIVE_CORRUPT_SKIP ) noexcept
        : m_chunk( entryChunkHandler )
        , m_headRecord( headRecord )
        , m_entryMedia( entryMedia )
        , m_entryPayloadHandlerPtr( nullptr )
        , m_entrySize( singleEntrySizeInBytes + getEntryMetadataSize() + entryChunkHandler.getMetadataLength() )
        , m_maxEntries( entryMedia.getMaxSize() / ( m_entrySize ) )
        , m_maxOffset( ( m_maxEntries - 1 ) * m_entrySize )
        , m_maxCorruptScan( maxCorruptScan )
        , m_maxConsecutiveCorruptSkip( maxConsecutiveCorruptSkip )
        , m_started( false )
    {
        KIT_SYSTEM_ASSERT( m_maxEntries > 1 );
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
    /// See Kit::Persistence::Record::Journal::IEntry
    bool getLatest( IPayload& dst, Marker_T& entryMarker ) noexcept override;

    /// See Kit::Persistence::Record::Journal::IEntry
    bool getNext( uint64_t       newerThanTimestamp,
                  const Marker_T beginHereMarker,
                  IPayload&      dst,
                  Marker_T&      entryMarker ) noexcept override;

    /// See Kit::Persistence::Record::Journal::IEntry
    bool getPrevious( uint64_t       olderThanTimestamp,
                      const Marker_T beginHereMarker,
                      IPayload&      dst,
                      Marker_T&      entryMarker ) noexcept override;

    /// See Kit::Persistence::Record::Journal::IEntry
    bool getByEntryIndex( Size_T    entryIndex,
                          IPayload& dst,
                          Marker_T& entryMarker ) noexcept override;

    /// See Kit::Persistence::Record::Journal::IEntry
    Size_T getMaxIndex() const noexcept override;

    /// See Kit::Persistence::Record::Journal::IEntry
    bool addEntry( const IPayload& src ) noexcept override;

    /// See Kit::Persistence::Record::Journal::IEntry
    bool resetHead() noexcept override;

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
    Size_T incrementOffset( Size_T offsetToIncrement ) const noexcept;

    /// Helper method: 'decrements' the offset by the size of entry (and handles the 'roll-over' case)
    Size_T decrementOffset( Size_T offsetToDecrement ) const noexcept;

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

    /// Maximum consecutive corrupt entries to walk back over during startup scan
    Size_T m_maxCorruptScan;

    /// Maximum consecutive corrupt entries to skip during getNext()/getPrevious() traversal
    Size_T m_maxConsecutiveCorruptSkip;

    /// Track the started state
    bool m_started;
};

}  // end namespaces
}
}
}
#endif  // end header latch
