/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Driver/NV/File/Api.h"
#include "Kit/Io/File/Input.h"
#include "Kit/Io/File/InputOutput.h"
#include "Kit/Io/File/System.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include <limits>
#include <stdint.h>
#include <string.h>

#define SECT_         "_0test"

#define FILE_NAME_    "nvfile.bin"
#define FILE_NAME2_   "nvfile2.bin"
#define DIR_NAME_     "nvfile_dir"
#define BAD_PATH_     "nvfile_no_such_dir/nvfile.bin"
#define NUM_PAGES_    3
#define PAGE_SIZE_    15  // Not a multiple of the (16 byte) erase buffer
#define TOTAL_SIZE_   ( NUM_PAGES_ * PAGE_SIZE_ )
#define ERASED_VALUE_ OPTION_KIT_DRIVER_NV_FILE_ERASED_VALUE

using namespace Kit::Driver::NV::File;
using Kit::Io::File::System;

namespace {

void removeTestFiles() noexcept
{
    System::remove( FILE_NAME_ );
    System::remove( FILE_NAME2_ );
    System::remove( DIR_NAME_ );
    System::remove( OPTION_KIT_DRIVER_NV_FILE_NAME );
}

// Reads the raw file content (bypassing the driver)
bool readRawFile( const char* fname, uint8_t* dst, Kit::Type::SSize_T numBytes ) noexcept
{
    Kit::Io::File::Input fd( fname );
    return fd.read( dst, numBytes );
}

bool isErased( const uint8_t* buf, size_t len ) noexcept
{
    for ( size_t i = 0; i < len; i++ )
    {
        if ( buf[i] != ERASED_VALUE_ )
        {
            return false;
        }
    }
    return true;
}

}  // end anonymous namespace

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "File" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    removeTestFiles();

    uint8_t buf[TOTAL_SIZE_ + 1];
    Api     uut( NUM_PAGES_, PAGE_SIZE_, FILE_NAME_ );

    SECTION( "geometry and default arguments" )
    {
        REQUIRE( uut.getNumPages() == NUM_PAGES_ );
        REQUIRE( uut.getPageSize() == PAGE_SIZE_ );
        REQUIRE( uut.getTotalSize() == TOTAL_SIZE_ );
        REQUIRE( strcmp( uut.getFileName(), FILE_NAME_ ) == 0 );

        Api dflt;
        REQUIRE( dflt.getNumPages() == OPTION_KIT_DRIVER_NV_FILE_NUM_PAGES );
        REQUIRE( dflt.getPageSize() == OPTION_KIT_DRIVER_NV_FILE_BYTES_PER_PAGE );
        REQUIRE( dflt.getTotalSize() == OPTION_KIT_DRIVER_NV_FILE_NUM_PAGES * OPTION_KIT_DRIVER_NV_FILE_BYTES_PER_PAGE );
        REQUIRE( strcmp( dflt.getFileName(), OPTION_KIT_DRIVER_NV_FILE_NAME ) == 0 );
        REQUIRE( dflt.start() );
        REQUIRE( System::size( OPTION_KIT_DRIVER_NV_FILE_NAME ) == OPTION_KIT_DRIVER_NV_FILE_NUM_PAGES * OPTION_KIT_DRIVER_NV_FILE_BYTES_PER_PAGE );
        dflt.stop();
    }

    SECTION( "start creates an erased file" )
    {
        REQUIRE( !System::exists( FILE_NAME_ ) );
        REQUIRE( uut.start() );
        REQUIRE( System::isFile( FILE_NAME_ ) );
        REQUIRE( System::size( FILE_NAME_ ) == TOTAL_SIZE_ );

        memset( buf, 0, sizeof( buf ) );
        REQUIRE( readRawFile( FILE_NAME_, buf, TOTAL_SIZE_ ) );
        REQUIRE( isErased( buf, TOTAL_SIZE_ ) );

        memset( buf, 0, sizeof( buf ) );
        REQUIRE( uut.read( 0, buf, sizeof( buf ), TOTAL_SIZE_ ) );
        REQUIRE( isErased( buf, TOTAL_SIZE_ ) );
    }

    SECTION( "start/stop" )
    {
        REQUIRE( uut.start() );
        REQUIRE( uut.start() );  // Already started -->nop
        uut.stop();
        uut.stop();  // Already stopped -->nop
        REQUIRE( !uut.write( 0, "a", 1 ) );
        REQUIRE( !uut.read( 0, buf, sizeof( buf ), 1 ) );
    }

    SECTION( "existing file content is preserved across a restart" )
    {
        REQUIRE( uut.start() );
        REQUIRE( uut.write( 5, "hello", 5 ) );
        uut.stop();

        Api uut2( NUM_PAGES_, PAGE_SIZE_, FILE_NAME_ );
        REQUIRE( uut2.start() );
        memset( buf, 0, sizeof( buf ) );
        REQUIRE( uut2.read( 5, buf, sizeof( buf ), 5 ) );
        REQUIRE( memcmp( buf, "hello", 5 ) == 0 );
        uut2.stop();
    }

    SECTION( "start failures" )
    {
        // File name is a directory
        REQUIRE( System::createDirectory( DIR_NAME_ ) );
        REQUIRE( uut.setFileName( DIR_NAME_ ) );
        REQUIRE( !uut.start() );

        // File cannot be created
        REQUIRE( uut.setFileName( BAD_PATH_ ) );
        REQUIRE( !uut.start() );
        REQUIRE( !uut.write( 0, "a", 1 ) );

        // Storage does not fit in SSize_T
        size_t tooBig = static_cast<size_t>( std::numeric_limits<Kit::Type::SSize_T>::max() ) + 1u;
        Api    big( tooBig, 1, FILE_NAME2_ );
        REQUIRE( !big.start() );
        REQUIRE( !System::exists( FILE_NAME2_ ) );
    }

    SECTION( "setFileName" )
    {
        REQUIRE( !uut.setFileName( nullptr ) );
        REQUIRE( strcmp( uut.getFileName(), FILE_NAME_ ) == 0 );
        REQUIRE( uut.setFileName( FILE_NAME2_ ) );
        REQUIRE( strcmp( uut.getFileName(), FILE_NAME2_ ) == 0 );

        REQUIRE( uut.start() );
        REQUIRE( System::exists( FILE_NAME2_ ) );
        REQUIRE( !System::exists( FILE_NAME_ ) );
        REQUIRE( !uut.setFileName( FILE_NAME_ ) );  // Fails while started
        REQUIRE( strcmp( uut.getFileName(), FILE_NAME2_ ) == 0 );
        uut.stop();
        REQUIRE( uut.setFileName( FILE_NAME_ ) );
    }

    SECTION( "write/read" )
    {
        REQUIRE( uut.start() );

        // Across a page boundary
        REQUIRE( uut.write( PAGE_SIZE_ - 2, "abcd", 4 ) );
        memset( buf, 0, sizeof( buf ) );
        REQUIRE( uut.read( PAGE_SIZE_ - 3, buf, sizeof( buf ), 6 ) );
        REQUIRE( buf[0] == ERASED_VALUE_ );
        REQUIRE( memcmp( buf + 1, "abcd", 4 ) == 0 );
        REQUIRE( buf[5] == ERASED_VALUE_ );

        // Last byte(s) of the storage
        REQUIRE( uut.write( TOTAL_SIZE_ - 3, "xyz", 3 ) );
        memset( buf, 0, sizeof( buf ) );
        REQUIRE( uut.read( TOTAL_SIZE_ - 3, buf, 3, 3 ) );
        REQUIRE( memcmp( buf, "xyz", 3 ) == 0 );

        // Whole storage
        uint8_t pattern[TOTAL_SIZE_];
        for ( size_t i = 0; i < sizeof( pattern ); i++ )
        {
            pattern[i] = static_cast<uint8_t>( i );
        }
        REQUIRE( uut.write( 0, pattern, sizeof( pattern ) ) );
        memset( buf, 0, sizeof( buf ) );
        REQUIRE( uut.read( 0, buf, sizeof( buf ), TOTAL_SIZE_ ) );
        REQUIRE( memcmp( buf, pattern, TOTAL_SIZE_ ) == 0 );
        REQUIRE( System::size( FILE_NAME_ ) == TOTAL_SIZE_ );

        // Zero bytes at the end of the storage
        REQUIRE( uut.write( TOTAL_SIZE_, pattern, 0 ) );
        REQUIRE( uut.read( TOTAL_SIZE_, buf, sizeof( buf ), 0 ) );
    }

    SECTION( "out of range" )
    {
        REQUIRE( uut.start() );
        REQUIRE( !uut.write( TOTAL_SIZE_, "a", 1 ) );
        REQUIRE( !uut.write( TOTAL_SIZE_ + 1, "a", 0 ) );
        REQUIRE( !uut.write( TOTAL_SIZE_ - 1, "ab", 2 ) );
        REQUIRE( !uut.write( 1, buf, SIZE_MAX ) );  // size_t overflow
        REQUIRE( !uut.read( TOTAL_SIZE_, buf, sizeof( buf ), 1 ) );
        REQUIRE( !uut.read( TOTAL_SIZE_ + 1, buf, sizeof( buf ), 0 ) );
        REQUIRE( !uut.read( TOTAL_SIZE_ - 1, buf, sizeof( buf ), 2 ) );
        REQUIRE( !uut.read( SIZE_MAX, buf, sizeof( buf ), 1 ) );
        REQUIRE( System::size( FILE_NAME_ ) == TOTAL_SIZE_ );
    }

    SECTION( "destination buffer too small" )
    {
        REQUIRE( uut.start() );
        REQUIRE( !uut.read( 0, buf, 3, 4 ) );
        REQUIRE( uut.read( 0, buf, 4, 4 ) );
    }

    SECTION( "file removed after start" )
    {
        REQUIRE( uut.start() );
        REQUIRE( System::remove( FILE_NAME_ ) );
        REQUIRE( !uut.write( 0, "a", 1 ) );
        REQUIRE( !System::exists( FILE_NAME_ ) );  // write does NOT re-create the file
        REQUIRE( !uut.read( 0, buf, sizeof( buf ), 1 ) );
    }

    SECTION( "file shorter than the storage size (e.g. the storage grew) is padded" )
    {
        // Existing (smaller) file with known content
        {
            Kit::Io::File::InputOutput fd( FILE_NAME_, true, true );
            REQUIRE( fd.write( "hello", 5 ) );
        }
        REQUIRE( System::size( FILE_NAME_ ) == 5 );

        REQUIRE( uut.start() );
        REQUIRE( System::size( FILE_NAME_ ) == TOTAL_SIZE_ );

        // Existing content is preserved, the remainder is erased
        memset( buf, 0, sizeof( buf ) );
        REQUIRE( uut.read( 0, buf, sizeof( buf ), TOTAL_SIZE_ ) );
        REQUIRE( memcmp( buf, "hello", 5 ) == 0 );
        REQUIRE( isErased( buf + 5, TOTAL_SIZE_ - 5 ) );

        // The last byte is accessible
        REQUIRE( uut.write( TOTAL_SIZE_ - 1, "z", 1 ) );
        REQUIRE( uut.read( TOTAL_SIZE_ - 1, buf, sizeof( buf ), 1 ) );
        REQUIRE( buf[0] == 'z' );
    }

    SECTION( "empty or partially initialized file is completed" )
    {
        REQUIRE( System::createFile( FILE_NAME_ ) );
        REQUIRE( System::size( FILE_NAME_ ) == 0 );
        REQUIRE( uut.start() );
        REQUIRE( System::size( FILE_NAME_ ) == TOTAL_SIZE_ );
        memset( buf, 0, sizeof( buf ) );
        REQUIRE( readRawFile( FILE_NAME_, buf, TOTAL_SIZE_ ) );
        REQUIRE( isErased( buf, TOTAL_SIZE_ ) );
        uut.stop();

        // Simulate an interrupted padding: an erased file that is too short
        REQUIRE( System::remove( FILE_NAME_ ) );
        {
            Kit::Io::File::InputOutput fd( FILE_NAME_, true, true );
            memset( buf, ERASED_VALUE_, sizeof( buf ) );
            REQUIRE( fd.write( buf, TOTAL_SIZE_ / 2 ) );
        }
        REQUIRE( uut.start() );
        REQUIRE( System::size( FILE_NAME_ ) == TOTAL_SIZE_ );
        memset( buf, 0, sizeof( buf ) );
        REQUIRE( readRawFile( FILE_NAME_, buf, TOTAL_SIZE_ ) );
        REQUIRE( isErased( buf, TOTAL_SIZE_ ) );
    }

    SECTION( "file longer than the storage size (e.g. the storage shrank) is accepted" )
    {
        Api bigger( NUM_PAGES_ + 1, PAGE_SIZE_, FILE_NAME_ );
        REQUIRE( bigger.start() );
        REQUIRE( bigger.write( TOTAL_SIZE_, "x", 1 ) );  // Beyond the smaller storage
        REQUIRE( bigger.write( 0, "a", 1 ) );
        bigger.stop();

        REQUIRE( uut.start() );
        REQUIRE( System::size( FILE_NAME_ ) == TOTAL_SIZE_ + PAGE_SIZE_ );  // Not modified
        REQUIRE( uut.read( 0, buf, sizeof( buf ), 1 ) );
        REQUIRE( buf[0] == 'a' );
        REQUIRE( !uut.read( TOTAL_SIZE_, buf, sizeof( buf ), 1 ) );  // Only the first getTotalSize() bytes are used
    }

    uut.stop();
    removeTestFiles();
    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
