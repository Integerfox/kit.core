#ifndef KIT_PERSISTENCE_RECORD_JOURNAL_HEADRECORD_H
#define KIT_PERSISTENCE_RECORD_JOURNAL_HEADRECORD_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/EventQueue/IQueue.h"
#include "Kit/Persistence/Record/Journal/IHead.h"
#include "Kit/Persistence/Record/IChunk.h"

///
namespace Kit {
///
namespace Persistence {
///
namespace Record {
///
namespace Journal {

/** This concrete class is responsible for storing and retrieving the "head pointer"
    for the logical RingBuffer used by the EntryRecord to manage multiple entries.
*/
class HeadRecord : public IHead
{
public:
    /// Constructor.
    HeadRecord( Kit::Persistence::Record::IChunk& chunkHandler ) noexcept
        : m_chunk( chunkHandler )
        , m_latestTimestamp( 0 )
        , m_latestOffset( 0 )
        , m_validLatest( false )
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
    /// See Kit::Persistence::Record::Journal::IHead
    bool getLatestOffset( Size_T& offset, uint64_t& timestamp ) const noexcept override;

    /// See Kit::Persistence::Record::Journal::IHead
    void setLatestOffset( Size_T offset, uint64_t timestamp ) noexcept override;

    /// See Kit::Persistence::Record::Journal::IHead
    IHead::State_T getCurrentState() const noexcept override;

    /// See Kit::Persistence::Record::Journal::IHead
    void restoreState( const IHead::State_T& state ) noexcept override;

public:
    /// See Kit::Persistence::Record::IDataRecord
    bool writeToMedia( Size_T index = 0 ) noexcept override;

    /// See Kit::Persistence::Record::IDataRecord
    bool readFromMedia( Size_T index = 0 ) noexcept override;

    /// See Kit::Persistence::Record::IDataRecord
    bool processNoValidData() noexcept override;

protected:
    /// Reference to the chunk handler for this record
    Kit::Persistence::Record::IChunk& m_chunk;

    /// Timestamp of the latest entry
    uint64_t m_latestTimestamp;

    /// Offset for the latest entry
    Size_T m_latestOffset;

    /// Flag that indicates the state of offsets
    bool m_validLatest;

    /// Started state
    bool m_started;
};

}  // end namespaces
}
}
}
#endif  // end header latch
