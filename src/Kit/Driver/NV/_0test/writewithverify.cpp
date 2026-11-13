/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Driver/NV/WriteWithVerify.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "Kit/System/Trace.h"
#include "catch2/catch_test_macros.hpp"
#include <string.h>

#define SECT_ "_0test"

namespace {
class MockNvDriver : public Kit::Driver::NV::IApi
{
public:
    void reset()
    {
        m_writeResult  = true;
        m_readResult   = true;
        m_corruptWrite = false;
        m_readCount    = 0;
        m_writeCount   = 0;
        memset( m_mockMemory, 0xAA, m_numPages * m_pageSize );
    }

public:
    MockNvDriver( size_t numPages, size_t pageSize ) noexcept
        : m_numPages( numPages )
        , m_pageSize( pageSize )
    {
        m_mockMemory = new uint8_t[m_numPages * m_pageSize];
        reset();
    }

    ~MockNvDriver() noexcept
    {
        delete[] m_mockMemory;
    }

    bool start( void* = nullptr ) noexcept override
    {
        return true;
    }

    void stop() noexcept override
    {
    }

    bool write( size_t offset, const void* data, size_t size ) noexcept override
    {
        m_writeCount++;
        if ( !m_corruptWrite )
        {
            memcpy( m_mockMemory + offset, data, size );
        }
        else
        {
            memcpy( m_mockMemory + offset, data, size - 1 );
        }
        return m_writeResult;
    }

    bool read( size_t offset, void* data, size_t sizeDstData, size_t size ) noexcept override
    {
        m_readCount++;
        if ( sizeDstData < size )
        {
            return false;
        }
        memcpy( data, m_mockMemory + offset, size );
        return m_readResult;
    }

    size_t getNumPages() const noexcept override
    {
        return m_numPages;
    }

    size_t getPageSize() const noexcept override
    {
        return m_pageSize;
    }

    size_t getTotalSize() const noexcept override
    {
        return m_numPages * m_pageSize;
    }

public:
    size_t   m_numPages;
    size_t   m_pageSize;
    uint8_t* m_mockMemory;
    bool     m_writeResult;
    bool     m_readResult;
    bool     m_corruptWrite;
    unsigned m_writeCount;
    unsigned m_readCount;
};

/// Records the pre/post write hook calls
class HookedWriteWithVerify : public Kit::Driver::NV::WriteWithVerify
{
public:
    HookedWriteWithVerify( Kit::Driver::NV::IApi& nvDriver ) noexcept
        : Kit::Driver::NV::WriteWithVerify( nvDriver )
        , m_preWriteResult( true )
        , m_postWriteResult( true )
        , m_preWriteCount( 0 )
        , m_postWriteCount( 0 )
        , m_lastWriteResult( false )
    {
    }

    bool preWrite( size_t, const void*, size_t ) noexcept override
    {
        m_preWriteCount++;
        return m_preWriteResult;
    }

    bool postWrite( bool writeResult ) noexcept override
    {
        m_postWriteCount++;
        m_lastWriteResult = writeResult;
        return m_postWriteResult;
    }

    bool     m_preWriteResult;
    bool     m_postWriteResult;
    unsigned m_preWriteCount;
    unsigned m_postWriteCount;
    bool     m_lastWriteResult;
};

}  // end anonymous namespace

////////////////////////////////////////////////////////////////////////////////
#define NUM_PAGES  128
#define PAGE_SIZE  3
#define TOTAL_SIZE ( NUM_PAGES * PAGE_SIZE )
#define DATA_LEN   8

TEST_CASE( "WriteVerifyRead" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    MockNvDriver                     m_nvDriver( NUM_PAGES, PAGE_SIZE );
    Kit::Driver::NV::WriteWithVerify uut( m_nvDriver );
    m_nvDriver.reset();

    SECTION( "Happy path" )
    {
        REQUIRE( uut.start() );

        // Write data to the mock driver
        uint8_t writeData[256] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xA1, 0xB1, 0xC1, 0x1D };
        REQUIRE( uut.write( 0, writeData, sizeof( writeData ) ) == true );
        REQUIRE( m_nvDriver.m_writeCount == 1 );
        REQUIRE( m_nvDriver.m_readCount == sizeof( writeData ) / OPTION_KIT_DRIVER_NV_WRITE_WITH_VERIFY_TEMP_BUF_SIZE );

        // Read back the data
        uint8_t readData[256] = { 0 };
        REQUIRE( uut.read( 0, readData, sizeof( readData ), sizeof( readData ) ) == true );
        REQUIRE( m_nvDriver.m_writeCount == 1 );
        REQUIRE( m_nvDriver.m_readCount == sizeof( writeData ) / OPTION_KIT_DRIVER_NV_WRITE_WITH_VERIFY_TEMP_BUF_SIZE + 1 );

        // Verify the read data
        REQUIRE( memcmp( writeData, readData, sizeof( writeData ) ) == 0 );

        REQUIRE( uut.getNumPages() == NUM_PAGES );
        REQUIRE( uut.getPageSize() == PAGE_SIZE );
        REQUIRE( uut.getTotalSize() == TOTAL_SIZE );

        uut.stop();
    }

    SECTION( "error-bad-data" )
    {
        REQUIRE( uut.start() );
        m_nvDriver.m_corruptWrite = true;

        uint8_t writeData[256] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xA1, 0xB1, 0xC1, 0x1D };
        REQUIRE( uut.write( 0, writeData, DATA_LEN ) == false );
        REQUIRE( m_nvDriver.m_writeCount == 1 );
        REQUIRE( m_nvDriver.m_readCount == DATA_LEN / OPTION_KIT_DRIVER_NV_WRITE_WITH_VERIFY_TEMP_BUF_SIZE );

        // Read back the data
        uint8_t readData[256] = { 0 };
        REQUIRE( uut.read( 0, readData, sizeof( readData ), DATA_LEN - 1 ) == true );

        // Verify the read data
        REQUIRE( memcmp( writeData, readData, DATA_LEN ) != 0 );
        REQUIRE( memcmp( writeData, readData, DATA_LEN - 1 ) == 0 );

        uut.stop();
    }

    SECTION( "constructor asserts" )
    {
        // The unit-test shutdown handler counts the assert instead of terminating
        Kit::System::ShutdownUnitTesting::clearAndUseCounter();
        Kit::Driver::NV::WriteWithVerify maxAllowed( m_nvDriver, UINT8_MAX - 1 );
        REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );

        Kit::Driver::NV::WriteWithVerify tooMany( m_nvDriver, UINT8_MAX );
        REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 1u );
    }

    SECTION( "max-retries" )
    {
        constexpr uint8_t                maxRetries = 3;
        Kit::Driver::NV::WriteWithVerify uut2( m_nvDriver, maxRetries );

        REQUIRE( uut2.start() );
        m_nvDriver.m_corruptWrite = true;

        uint8_t writeData[256] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xA1, 0xB1, 0xC1, 0x1D };
        REQUIRE( uut2.write( 0, writeData, DATA_LEN ) == false );
        REQUIRE( m_nvDriver.m_writeCount == ( maxRetries + 1 ) );
        REQUIRE( m_nvDriver.m_readCount == ( DATA_LEN / OPTION_KIT_DRIVER_NV_WRITE_WITH_VERIFY_TEMP_BUF_SIZE ) * ( maxRetries + 1 ) );

        // Read back the data
        uint8_t readData[256] = { 0 };
        REQUIRE( uut2.read( 0, readData, sizeof( readData ), DATA_LEN - 1 ) == true );

        // Verify the read data
        REQUIRE( memcmp( writeData, readData, DATA_LEN ) != 0 );
        REQUIRE( memcmp( writeData, readData, DATA_LEN - 1 ) == 0 );

        uut2.stop();
    }

    SECTION( "error-read-back-fail" )
    {
        REQUIRE( uut.start() );
        m_nvDriver.m_readResult = false;

        uint8_t writeData[256] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xA1, 0xB1, 0xC1, 0x1D };
        REQUIRE( uut.write( 0, writeData, DATA_LEN ) == false );

        // Read back the data
        uint8_t readData[256]   = { 0 };
        m_nvDriver.m_readResult = true;
        REQUIRE( uut.read( 0, readData, sizeof( readData ), DATA_LEN ) == true );

        // Verify the read data
        REQUIRE( memcmp( writeData, readData, DATA_LEN ) == 0 );

        uut.stop();
    }

    SECTION( "error-write-failed" )
    {
        REQUIRE( uut.start() );
        m_nvDriver.m_writeResult = false;

        // Write data to the mock driver
        uint8_t writeData[256] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xA1, 0xB1, 0xC1, 0x1D };
        REQUIRE( uut.write( 0, writeData, sizeof( writeData ) ) == false );

        uut.stop();
    }

    SECTION( "error-null-pointers" )
    {
        REQUIRE( uut.start() );

        // Invalid Write|Read data
        REQUIRE( uut.write( 0, nullptr, 2 ) == false );
        REQUIRE( uut.read( 0, nullptr, 2, 2 ) == false );

        uut.stop();
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}

TEST_CASE( "WriteVerifyRead - pre/post write hooks" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    MockNvDriver          nvDriver( NUM_PAGES, PAGE_SIZE );
    HookedWriteWithVerify uut( nvDriver );
    uint8_t               writeData[DATA_LEN] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xA1, 0xB1, 0xC1, 0x1D };
    REQUIRE( uut.start() );

    SECTION( "the hooks bracket a write" )
    {
        REQUIRE( uut.write( 0, writeData, sizeof( writeData ) ) == true );
        REQUIRE( uut.m_preWriteCount == 1 );
        REQUIRE( uut.m_postWriteCount == 1 );
        REQUIRE( uut.m_lastWriteResult == true );
        REQUIRE( nvDriver.m_writeCount == 1 );
    }

    SECTION( "postWrite() determines the write result" )
    {
        uut.m_postWriteResult = false;
        REQUIRE( uut.write( 0, writeData, sizeof( writeData ) ) == false );
        REQUIRE( uut.m_lastWriteResult == true );
    }

    SECTION( "a failed preWrite() is still paired with postWrite() - and the write fails without writing" )
    {
        uut.m_preWriteResult = false;
        REQUIRE( uut.write( 0, writeData, sizeof( writeData ) ) == false );
        REQUIRE( uut.m_preWriteCount == 1 );
        REQUIRE( uut.m_postWriteCount == 1 );
        REQUIRE( uut.m_lastWriteResult == false );
        REQUIRE( nvDriver.m_writeCount == 0 );
    }

    SECTION( "a null source pointer fails before the hooks are called" )
    {
        REQUIRE( uut.write( 0, nullptr, sizeof( writeData ) ) == false );
        REQUIRE( uut.m_preWriteCount == 0 );
        REQUIRE( uut.m_postWriteCount == 0 );
    }

    uut.stop();
    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
