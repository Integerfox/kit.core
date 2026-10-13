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
#include "Kit/Io/File/Md5Checksum.h"
#include "Kit/System/Trace.h"
#include "Kit/Text/FString.h"
#include <string.h>


#define SECT_ "_0test"

/** Hack to get around the fact the SCM systems will convert newline
    characters in Text Files based on the target host, i.e. on Windows my
    newline character will be 2 chars, whereas on posix/linux it will be
    a single character -->hence delta in the file length/checksum.
 */
#define WIN32_TESTINPUT_TXT_FILE_LENGTH 106
#define POSIX_TESTINPUT_TXT_FILE_LENGTH 101
#define WIN32_GOLDEN_HASH               "590C7DF15071D2068085E11EDF9CF1C2"
#define POSIX_GOLDEN_HASH               "D195D9DF44D856E64D9EA8C5B62F4953"

static const uint8_t win32Digest_[] = { 0x59, 0x0c, 0x7d, 0xf1, 0x50, 0x71, 0xd2, 0x06, 0x80, 0x85, 0xe1, 0x1e, 0xdf, 0x9c, 0xf1, 0xc2 };
static const uint8_t posixDigest_[] = { 0xd1, 0x95, 0xd9, 0xdf, 0x44, 0xd8, 0x56, 0xe6, 0x4d, 0x9e, 0xa8, 0xc5, 0xb6, 0x2f, 0x49, 0x53 };

///
using namespace Kit::Io::File;

/// Returns true when the digest matches either line-ending variant of the test file
static bool isExpectedDigest( const Kit::Checksum::Md5::Digest_T digest )
{
    return memcmp( posixDigest_, digest, sizeof( Kit::Checksum::Md5::Digest_T ) ) == 0 ||
           memcmp( win32Digest_, digest, sizeof( Kit::Checksum::Md5::Digest_T ) ) == 0;
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "md5checksum" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Kit::Checksum::Md5::Digest_T digest;

    SECTION( "digest, string, and file length" )
    {
        size_t                                                 filelen = 0;
        Kit::Text::FString<Kit::Checksum::Md5::eDIGEST_LEN * 2> dstString;

        REQUIRE( calcMd5Checksum( "testinput.txt", digest, &dstString, &filelen ) );
        KIT_SYSTEM_TRACE_MSG( SECT_, "md5=[%s], len=%lu", dstString.getString(), (unsigned long)filelen );

        REQUIRE( ( dstString == POSIX_GOLDEN_HASH || dstString == WIN32_GOLDEN_HASH ) );
        REQUIRE( isExpectedDigest( digest ) );
        REQUIRE( ( filelen == POSIX_TESTINPUT_TXT_FILE_LENGTH || filelen == WIN32_TESTINPUT_TXT_FILE_LENGTH ) );
    }

    SECTION( "digest only" )
    {
        REQUIRE( calcMd5Checksum( "testinput.txt", digest ) );
        REQUIRE( isExpectedDigest( digest ) );
    }

    SECTION( "digest and file length" )
    {
        size_t filelen = 0;
        REQUIRE( calcMd5Checksum( "testinput.txt", digest, nullptr, &filelen ) );
        REQUIRE( isExpectedDigest( digest ) );
        REQUIRE( ( filelen == POSIX_TESTINPUT_TXT_FILE_LENGTH || filelen == WIN32_TESTINPUT_TXT_FILE_LENGTH ) );
    }

    SECTION( "file does not exist" )
    {
        size_t filelen = 1;
        REQUIRE( calcMd5Checksum( "no-such-file.txt", digest, nullptr, &filelen ) == false );
        REQUIRE( filelen == 1 );
    }

    SECTION( "read error that is NOT end-of-file" )
    {
        // A directory opens successfully on POSIX, but read()'ing it fails with an
        // error instead of EOF. On Win32 the open() itself fails -->either way the
        // checksum calculation must fail.
        size_t filelen = 1;
        REQUIRE( calcMd5Checksum( ".", digest, nullptr, &filelen ) == false );
        REQUIRE( filelen == 1 );
    }

    SECTION( "no digest buffer" )
    {
        size_t                                                 filelen = 1;
        Kit::Text::FString<Kit::Checksum::Md5::eDIGEST_LEN * 2> dstString( "bob" );

        REQUIRE( calcMd5Checksum( "testinput.txt", nullptr, &dstString, &filelen ) == false );
        REQUIRE( dstString == "bob" );
        REQUIRE( filelen == 1 );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
