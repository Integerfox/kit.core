/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "catch2/catch_test_macros.hpp"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "Kit/System/Trace.h"
#include "Kit/Persistence/Record/Media/NVAdapter.h"
#include "Kit/EventQueue/Server.h"
#include <string.h>

#define SECT_ "_0test"

///
using namespace Kit::Persistence::Record::Media;
using namespace Kit::Persistence::Record;
using namespace Kit::Persistence;


////////////////////////////////////////////////////////////////////////////////
namespace {

// Mock NV driver - backs reads/writes with an in-memory buffer
class MockNVApi : public Kit::Driver::NV::IApi
{
public:
    bool     startResult = true;
    bool     writeResult = true;
    bool     readResult  = true;
    unsigned startCount  = 0;
    unsigned stopCount   = 0;
    size_t   lastOffset  = 0;
    size_t   lastLen     = 0;
    uint8_t  storage[128] = { 0 };

public:
    /// See Kit::Driver::IStart
    bool start( void* startArgs = nullptr ) noexcept override
    {
        startCount++;
        return startResult;
    }

    /// See Kit::Driver::IStop
    void stop() noexcept override
    {
        stopCount++;
    }

    /// See Kit::Driver::NV::IApi
    bool write( size_t dstOffset, const void* srcData, size_t numBytesToWrite ) noexcept override
    {
        lastOffset = dstOffset;
        lastLen    = numBytesToWrite;
        if ( !writeResult )
        {
            return false;
        }
        memcpy( storage + dstOffset, srcData, numBytesToWrite );
        return true;
    }

    /// See Kit::Driver::NV::IApi
    bool read( size_t srcOffset, void* dstData, size_t sizeDstData, size_t numBytesToRead ) noexcept override
    {
        lastOffset = srcOffset;
        lastLen    = numBytesToRead;
        if ( !readResult || sizeDstData < numBytesToRead )
        {
            return false;
        }
        memcpy( dstData, storage + srcOffset, numBytesToRead );
        return true;
    }

    /// See Kit::Driver::NV::IApi
    size_t getNumPages() const noexcept override { return 1; }

    /// See Kit::Driver::NV::IApi
    size_t getPageSize() const noexcept override { return sizeof( storage ); }

    /// See Kit::Driver::NV::IApi
    size_t getTotalSize() const noexcept override { return sizeof( storage ); }
};

};  // end anonymous namespace


////////////////////////////////////////////////////////////////////////////////
#define ALLOCATED_LEN 32
#define STARTING_OFFSET 8

TEST_CASE( "NVAdapter" )
{
    KIT_SYSTEM_TRACE_SCOPE( SECT_, "NVAdapter test" );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::EventQueue::Server mockEventQueue;
    MockNVApi                driver;
    NVAdapter                uut( driver, STARTING_OFFSET, ALLOCATED_LEN );
    char                     buffer[ALLOCATED_LEN];

    SECTION( "start/stop delegate to the low level driver" )
    {
        REQUIRE( uut.start( mockEventQueue ) == true );
        REQUIRE( driver.startCount == 1 );

        driver.startResult = false;
        REQUIRE( uut.start( mockEventQueue ) == false );

        uut.stop();
        REQUIRE( driver.stopCount == 1 );
    }

    SECTION( "getMaxSize() returns the allocated length" )
    {
        REQUIRE( uut.getMaxSize() == ALLOCATED_LEN );
    }

    SECTION( "write/read happy path - starting offset is applied to the driver calls" )
    {
        REQUIRE( uut.write( 0, "Hello World", 11 ) == true );
        REQUIRE( driver.lastOffset == STARTING_OFFSET );
        REQUIRE( driver.lastLen == 11 );

        REQUIRE( uut.read( 0, buffer, 11 ) == 11 );
        REQUIRE( driver.lastOffset == STARTING_OFFSET );
        REQUIRE( strncmp( buffer, "Hello World", 11 ) == 0 );

        REQUIRE( uut.write( 6, "Kit!!", 5 ) == true );
        REQUIRE( driver.lastOffset == STARTING_OFFSET + 6 );
        REQUIRE( uut.read( 0, buffer, 11 ) == 11 );
        REQUIRE( strncmp( buffer, "Hello Kit!!", 11 ) == 0 );
    }

    SECTION( "write/read fail when the low level driver fails" )
    {
        driver.writeResult = false;
        REQUIRE( uut.write( 0, "Hello", 5 ) == false );

        driver.readResult = false;
        REQUIRE( uut.read( 0, buffer, 5 ) == KIT_PERSISTENCE_SIZE_MAX );
    }

    SECTION( "write/read fail (without invoking the driver) when the request exceeds the allocated length" )
    {
        REQUIRE( uut.write( ALLOCATED_LEN - 4, "TooLong", 7 ) == false );
        REQUIRE( uut.read( ALLOCATED_LEN - 4, buffer, 7 ) == KIT_PERSISTENCE_SIZE_MAX );

        REQUIRE( uut.write( ALLOCATED_LEN + 1, "x", 1 ) == false );
        REQUIRE( uut.read( ALLOCATED_LEN + 1, buffer, 1 ) == KIT_PERSISTENCE_SIZE_MAX );
    }

    SECTION( "write/read reject requests that would overflow Size_T when computing offset+length" )
    {
        Size_T hugeOffset = KIT_PERSISTENCE_SIZE_MAX - 5;
        REQUIRE( uut.write( hugeOffset, "1234567890", 10 ) == false );
        REQUIRE( uut.read( hugeOffset, buffer, 10 ) == KIT_PERSISTENCE_SIZE_MAX );
    }

    uut.stop();
    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
