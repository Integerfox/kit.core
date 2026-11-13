/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/Dm/Mp/_0test/common.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Dm/Mp/Uint32.h"
#include "Kit/Logging/Framework/JournalLogSink.h"
#include "Kit/Container/RingBufferMPAllocate.h"
#include "Kit/Time/BootTime.h"
#include "Kit/_support/testing/ManualEventQueue.h"

using namespace Kit::Logging::Framework;
using namespace Kit::Persistence::Record::Journal;
using namespace Kit::Persistence;

#define MAX_ENTRIES         10

#define FILTERED_ELAPSED_MS 999

extern uint16_t g_bootCount;  // Boot counter returned by the test's getBootTime() (see boottime.cpp)

namespace {  // Anonymous namespace for test support code

/// Fake Journal record that captures the entries written to it
class FakeEntryRecord : public IEntry
{
public:
    EntryData_T m_entries[MAX_ENTRIES];
    unsigned    m_numAdded;

public:
    FakeEntryRecord()
        : m_numAdded( 0 )
    {
    }

public:
    bool addEntry( const IPayload& src ) noexcept override
    {
        if ( m_numAdded < MAX_ENTRIES )
        {
            m_entries[m_numAdded] = static_cast<const EntryData_T&>( src );
            m_numAdded            = m_numAdded + 1;
        }
        return true;
    }

    bool   start( Kit::EventQueue::IQueue& myEventQueue ) noexcept override { return true; }
    void   stop() noexcept override {}
    Size_T getSize() const noexcept override { return EntryData_T::entryLen; }
    bool   writeToMedia( Size_T index ) noexcept override { return true; }
    bool   readFromMedia( Size_T index ) noexcept override { return true; }
    bool   processNoValidData() noexcept override { return true; }
    Size_T copyTo( void* dst, Size_T maxDstLen ) noexcept override { return KIT_PERSISTENCE_SIZE_MAX; }
    bool   copyFrom( const void* src, Size_T srcLen ) noexcept override { return false; }
    Size_T getMaxPayloadSize() const noexcept override { return EntryData_T::entryLen; }
    bool   getLatest( IPayload& dst, Marker_T& entryMarker ) noexcept override { return false; }
    bool   getNext( uint64_t newerThanTimestamp, const Marker_T beginHereMarker, IPayload& dst, Marker_T& entryMarker ) noexcept override { return false; }
    bool   getPrevious( uint64_t olderThanTimestamp, const Marker_T beginHereMarker, IPayload& dst, Marker_T& entryMarker ) noexcept override { return false; }
    bool   getByEntryIndex( Size_T entryIndex, IPayload& dst, Marker_T& entryMarker ) noexcept override { return false; }
    Size_T getMaxIndex() const noexcept override { return MAX_ENTRIES; }
    bool   resetHead() noexcept override { return true; }
};

/// Counts the remove-entry calls, and filters entries
class MyUtt : public JournalLogSink
{
public:
    unsigned m_numRemoveCalls;

public:
    MyUtt( Kit::EventQueue::IQueue& myEventQueue, IEntry& entryRecord, Kit::Container::RingBufferMP<EntryData_T>& logFifo, const std::atomic<bool>* pauseUpdates )
        : JournalLogSink( myEventQueue, entryRecord, logFifo, pauseUpdates )
        , m_numRemoveCalls( 0 )
    {
    }

protected:
    /// Filters out entries with an elapsed time of FILTERED_ELAPSED_MS
    bool hookAddingEntry( EntryData_T& entry ) noexcept override
    {
        uint16_t bootCounter = 0;
        uint64_t elapsedMs   = 0;
        Kit::Time::parseBootTime( entry.m_timestamp, bootCounter, elapsedMs );
        return JournalLogSink::hookAddingEntry( entry ) && elapsedMs != FILTERED_ELAPSED_MS;
    }

protected:
    bool hookRemovingEntry( EntryData_T& entry ) noexcept override
    {
        m_numRemoveCalls = m_numRemoveCalls + 1;
        return JournalLogSink::hookRemovingEntry( entry );
    }
};

}  // end anonymous namespace
////////////////////////////////////////////////////////////////////////////////

static Kit::Dm::ModelDatabase modelDb_( "ignoreThisParameter_usedToInvokeTheStaticConstructor" );
static Kit::Dm::Mp::Uint32    mp_journalFifoCount_( modelDb_, "journalLogFIFO" );
static Kit::Dm::Mp::Uint32    mp_journalFifoCount2_( modelDb_, "journalLogFIFO2" );

static void addEntry( Kit::Container::RingBufferMP<EntryData_T>& logFifo, uint16_t bootCounter, uint64_t elapsedMs )
{
    EntryData_T entry;
    entry.m_timestamp = Kit::Time::constructBootTime( bootCounter, elapsedMs );
    REQUIRE( logFifo.add( entry ) );
}

static uint16_t bootCounterOf( const EntryData_T& entry )
{
    uint16_t bootCounter = 0;
    uint64_t elapsedMs   = 0;
    Kit::Time::parseBootTime( entry.m_timestamp, bootCounter, elapsedMs );
    return bootCounter;
}

static uint64_t elapsedOf( const EntryData_T& entry )
{
    uint16_t bootCounter = 0;
    uint64_t elapsedMs   = 0;
    Kit::Time::parseBootTime( entry.m_timestamp, bootCounter, elapsedMs );
    return elapsedMs;
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "JournalLogSink" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    Kit::Container::RingBufferMPAllocate<EntryData_T, MAX_ENTRIES + 1> logFifo( mp_journalFifoCount_, false );  // Must be false: EntryData_T has a vtable pointer
    mp_journalFifoCount_.setInvalid();

    uint16_t          savedBootCount = g_bootCount;
    std::atomic<bool> paused( false );
    FakeEntryRecord   record;
    ManualEventQueue  eventQueue;
    MyUtt             uut( eventQueue, record, logFifo, &paused );

    SECTION( "boot counter loaded: entries are stamped with the current boot counter" )
    {
        g_bootCount = 7;
        REQUIRE( eventQueue.requestOpen( uut ) );
        addEntry( logFifo, 0, 100 );  // Logged before the boot-counter record was loaded
        addEntry( logFifo, 6, 200 );  // Logged while the record was loading (previous boot's counter)
        addEntry( logFifo, 7, 300 );  // Already current
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( record.m_numAdded == 3 );
        REQUIRE( bootCounterOf( record.m_entries[0] ) == 7 );
        REQUIRE( elapsedOf( record.m_entries[0] ) == 100 );
        REQUIRE( bootCounterOf( record.m_entries[1] ) == 7 );
        REQUIRE( elapsedOf( record.m_entries[1] ) == 200 );
        REQUIRE( bootCounterOf( record.m_entries[2] ) == 7 );
        REQUIRE( elapsedOf( record.m_entries[2] ) == 300 );
        REQUIRE( eventQueue.requestClose( uut ) );
    }

    SECTION( "the captured boot counter is per instance" )
    {
        g_bootCount = 7;
        REQUIRE( eventQueue.requestOpen( uut ) );

        // A second sink opened later (with a different boot counter) does not affect the first sink
        Kit::Container::RingBufferMPAllocate<EntryData_T, MAX_ENTRIES + 1> logFifo2( mp_journalFifoCount2_, false );
        mp_journalFifoCount2_.setInvalid();
        FakeEntryRecord record2;
        MyUtt           uut2( eventQueue, record2, logFifo2, nullptr );
        g_bootCount = 3;
        REQUIRE( eventQueue.requestOpen( uut2 ) );

        addEntry( logFifo, 0, 100 );
        addEntry( logFifo2, 0, 200 );
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( record.m_numAdded == 1 );
        REQUIRE( record2.m_numAdded == 1 );
        REQUIRE( bootCounterOf( record.m_entries[0] ) == 7 );
        REQUIRE( elapsedOf( record.m_entries[0] ) == 100 );
        REQUIRE( bootCounterOf( record2.m_entries[0] ) == 3 );
        REQUIRE( elapsedOf( record2.m_entries[0] ) == 200 );
        REQUIRE( eventQueue.requestClose( uut2 ) );
        REQUIRE( eventQueue.requestClose( uut ) );
    }

    SECTION( "filtered entries are removed from the log buffer and discarded" )
    {
        g_bootCount = 4;
        REQUIRE( eventQueue.requestOpen( uut ) );
        addEntry( logFifo, 0, 100 );
        addEntry( logFifo, 0, FILTERED_ELAPSED_MS );
        addEntry( logFifo, 0, 300 );
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( logFifo.isEmpty() );
        REQUIRE( record.m_numAdded == 2 );
        REQUIRE( elapsedOf( record.m_entries[0] ) == 100 );
        REQUIRE( elapsedOf( record.m_entries[1] ) == 300 );
        REQUIRE( eventQueue.requestClose( uut ) );
    }

    SECTION( "paused: entries are held in the log buffer" )
    {
        g_bootCount = 2;
        paused      = true;
        REQUIRE( eventQueue.requestOpen( uut ) );

        // More than one batch (OPTION_KIT_PERSISTENCE_JOURNAL_SERVER_MAX_BATCH_WRITE)
        const unsigned numPaused = OPTION_KIT_PERSISTENCE_JOURNAL_SERVER_MAX_BATCH_WRITE + 2;
        for ( unsigned i = 0; i < numPaused; i++ )
        {
            addEntry( logFifo, 0, i );
        }

        // The queue goes idle (i.e. no busy-looping while paused) with nothing written
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( record.m_numAdded == 0 );
        REQUIRE( logFifo.getNumItems() == numPaused );
        REQUIRE( uut.m_numRemoveCalls <= numPaused );  // At most one remove attempt per change notification

        // Resume: the held entries are written on the next change notification (i.e. the next logged entry)
        paused = false;
        addEntry( logFifo, 0, 500 );
        REQUIRE( eventQueue.runUntilIdle() );
        REQUIRE( record.m_numAdded == numPaused + 1 );
        REQUIRE( logFifo.isEmpty() );
        for ( unsigned i = 0; i < numPaused; i++ )
        {
            REQUIRE( bootCounterOf( record.m_entries[i] ) == 2 );
            REQUIRE( elapsedOf( record.m_entries[i] ) == i );
        }
        REQUIRE( bootCounterOf( record.m_entries[numPaused] ) == 2 );
        REQUIRE( elapsedOf( record.m_entries[numPaused] ) == 500 );
        REQUIRE( eventQueue.requestClose( uut ) );
    }

    g_bootCount = savedBootCount;

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
