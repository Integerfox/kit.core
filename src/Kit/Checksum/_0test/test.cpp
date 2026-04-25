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
#include "Kit/Text/FString.h"
#include "Kit/Checksum/Fletcher16.h"
#include "Kit/Checksum/Crc16CcittFast.h"
#include "Kit/Checksum/Crc32EthernetFast.h"
#include "Kit/Checksum/Md5.h"
#include "Kit/Checksum/Utils.h"
#include "Kit/Type/endian.h"

///
using namespace Kit::Checksum;


////////////////////////////////////////////////////////////////////////////////

#define SECT_           "_0test"

#define DATALEN_        9
#define MAX_CRCLEN_     4

#define CRC_CCITT_xFFFF 0x29B1     // CRC value for the text string "12346789"   (see http://www.zorc.breitbandkatze.de/crc.html)
#define CRC_ETHERNET    0x376E6E7  // CRC value for the text string "12346789"   (see http://www.zorc.breitbandkatze.de/crc.html, Final XOR val=0)
#define FLETCHER16      0x1e03     // Fletcher checksum for the text "123456789"

// clang-format off
uint8_t dataBuffer_[DATALEN_ + MAX_CRCLEN_] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', 0, };
static Kit::Text::FString<128> outbuf_;
// clang-format on

/////////////////////////////
static void printBuf( void* buf, int len, const char* msg )
{
    binToAsciiHex( buf, len, outbuf_ );

    KIT_SYSTEM_TRACE_MSG( SECT_, "%s. Buffer (len=%d): %s", msg, len, outbuf_() );
}


bool testEdc( const char* testName, IEdc& cksum, uint8_t* inBuffer, int inLen, void* resultBuffer, unsigned bufferSize )
{
    // Test error cases
    cksum.reset();
    cksum.accumulate( inBuffer, inLen );
    REQUIRE( cksum.finalize( nullptr, inLen ) == false ); // Should be false since no buffer provided
    REQUIRE( cksum.finalize( resultBuffer, 0 ) == false ); // Should be false since no buffer provided
    REQUIRE( cksum.finalize( resultBuffer, bufferSize ) );

    KIT_SYSTEM_TRACE_MSG( SECT_, "Test for: %s", testName );
    printBuf( inBuffer, inLen, "Input buffer" );
    cksum.reset();
    cksum.accumulate( nullptr, inLen ); // Should be nop
    cksum.accumulate( inBuffer, 0 ); // Should be nop
    cksum.accumulate( inBuffer, inLen );
    bool isFinalized = cksum.finalize( resultBuffer, bufferSize );
    printBuf( resultBuffer, cksum.getEdcSize(), "EDC" );

    KIT_SYSTEM_TRACE_MSG( SECT_, "EDC Finalized=%s.", isFinalized ? "true" : "false" );

    // Validate a 'inbound' CRC'd buffer
    cksum.reset();
    cksum.accumulate( inBuffer, inLen );
    cksum.accumulate( resultBuffer, cksum.getEdcSize() );
    return cksum.isOkay();
}


#define MD5_ITEMS_ 7
static const char* const md5_test_data_[MD5_ITEMS_ * 2] = {
    "",
    "D41D8CD98F00B204E9800998ECF8427E",
    "a",
    "0CC175B9C0F1B6A831C399E269772661",
    "abc",
    "900150983CD24FB0D6963F7D28E17F72",
    "message digest",
    "F96B697D7CB7938D525A2F31AAF161D0",
    "abcdefghijklmnopqrstuvwxyz",
    "C3FCD3D76192E4007DFB496CCA67E13B",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
    "D174AB98D277D9F5A5611C2C9F419D9F",
    "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
    "57EDF4A22BE3C955AC49DA2E2107B67A"
};


/////////////////////////////
TEST_CASE( "Checksums", "[chksum]" )
{

    Kit::System::ShutdownUnitTesting::clearAndUseCounter();


    SECTION( "Checksum - Fletcher16" )
    {
        Fletcher16 cksum;
        uint16_t   sum;
        REQUIRE( testEdc( "CheckSum Fletcher16", cksum, dataBuffer_, DATALEN_, &sum, sizeof( sum ) ) );
        REQUIRE( KIT_TYPE_HTOLE16( sum ) == FLETCHER16 );
    }

    SECTION( "Crc16 - CCITT - Fast" )
    {
        Crc16CcittFast cksum;
        uint16_t       crc;
        REQUIRE( testEdc( "CRC CCITT-FAST", cksum, dataBuffer_, DATALEN_, &crc, sizeof( crc ) ) );
        REQUIRE( KIT_TYPE_HTOBE16( crc ) == CRC_CCITT_xFFFF );
    }

    SECTION( "Crc32 - Ethernet - Fast" )
    {
        Crc32EthernetFast cksum;
        uint32_t          crc;
        REQUIRE( testEdc( "CRC Ethernet-FAST", cksum, dataBuffer_, DATALEN_, &crc, sizeof( crc ) ) );
        REQUIRE( KIT_TYPE_HTOBE32( crc ) == CRC_ETHERNET );
    }

    SECTION( "Md5" )
    {
        Md5                    hash;
        Kit::Text::FString<32> resultAsString;
        Kit::Text::FString<32> expectedResult;
        int                    i;
        
        // Error cases
        hash.reset();
        hash.accumulate( md5_test_data_[0], strlen( md5_test_data_[0] ) );
        REQUIRE( hash.finalize( nullptr, sizeof( Md5::Digest_T ) ) == false ); // Should be false since no buffer provided
        REQUIRE( hash.finalize( &resultAsString, 0 ) == false ); // Should be false since no buffer provided

        // Run against test vectors 
        REQUIRE( hash.getDigestSize() == sizeof( Md5::Digest_T ) );
        Md5::Digest_T result;
        for ( i = 0; i < MD5_ITEMS_ * 2; i += 2 )
        {
            hash.reset();
            hash.accumulate( md5_test_data_[i], strlen( md5_test_data_[i] ) );
            bool isFinalized = hash.finalize( &result, sizeof( result ) );
            expectedResult   = md5_test_data_[i + 1];
            binToAsciiHex( result, sizeof( result ), resultAsString );
            KIT_SYSTEM_TRACE_MSG( SECT_,
                                  "IsFinalized=[%s], hash=[%s] =? expected=[%s]",
                                  isFinalized ? "true" : "false",
                                  resultAsString(),
                                  expectedResult() );
            REQUIRE( resultAsString == expectedResult );
        }
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
