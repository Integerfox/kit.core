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
#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"
#include "Kit/System/Thread.h"
#include "Kit/Text/FString.h"
#include "Kit/Text/Format.h"
#include "Kit/Io/IInputOutput.h"
#include <cstdint>
#include <string.h>
#include <inttypes.h>

#define SECT_ "_0test"

#ifndef OPTION_TEST_END_OF_FRAME_CHAR
#define OPTION_TEST_END_OF_FRAME_CHAR 0x0D
#endif


extern void   echo_test( Kit::Io::IInputOutput& fd );
extern size_t getErrorCounts( bool clearCounts = false );

#define ECHO_BUFFER_SIZE    ( 1024 * 10 )

#define RX_READ_BUFFER_SIZE 64

////////////////////////////////////////////////////////////////////////////////
namespace {

class Receiver : public Kit::System::IRunnable
{
public:
    ///
    Kit::Io::IInputOutput&  m_fd;
    uint8_t                 m_rxMsg[RX_READ_BUFFER_SIZE];
    Kit::Text::FString<128> m_tmpBuf;
    Kit::Io::ByteCount_T    m_numEchoBytes;
    uint8_t                 m_echoMemory[ECHO_BUFFER_SIZE + 1];

public:
    Receiver( Kit::Io::IInputOutput& fd )
        : m_fd( fd )
    {
    }

public:
    void entry() noexcept override
    {
        // Throw any trash bytes on startup
        while ( m_fd.available() )
        {
            Kit::Io::ByteCount_T bytesRead;
            m_fd.read( m_rxMsg, sizeof( m_rxMsg ), bytesRead );
        }

        Kit::Io::ByteCount_T byteCount = 0;
        uint8_t*             dstPtr    = m_echoMemory;
        m_numEchoBytes                 = 0;

        for ( ;; )
        {

            Kit::Io::ByteCount_T bytesRead;
            if ( m_fd.read( m_rxMsg, sizeof( m_rxMsg ), bytesRead ) )
            {
                Bsp_toggle_debug1();
                byteCount += bytesRead;

                // Copy the data to the buffer while looking for ^Q
                uint8_t* ptr = m_rxMsg;
                while ( bytesRead-- )
                {
                    uint8_t inbyte = *ptr++;
                    *dstPtr++      = inbyte;
                    m_numEchoBytes++;

                    // End-of-Frame is the 'trigger' to output data
                    if ( inbyte == OPTION_TEST_END_OF_FRAME_CHAR )
                    {
                        echoMemory( byteCount );
                        dstPtr         = m_echoMemory;
                        m_numEchoBytes = 0;
                    }
                }
            }
        }
    }

    void echoMemory( Kit::Io::ByteCount_T byteCount )
    {
        Kit::Io::ByteCount_T bytes = m_numEchoBytes;

        uint8_t* srcData = m_echoMemory;
        while ( m_numEchoBytes )
        {
            int outlen = 16;
            if ( m_numEchoBytes < 16 )
            {
                outlen = m_numEchoBytes;
            }

            Kit::Text::Format::viewer( srcData, outlen, m_tmpBuf );
            srcData        += outlen;
            m_numEchoBytes -= outlen;
            m_fd.write( m_tmpBuf );
            m_fd.write( "\n" );
        }

        // NOTE: The GCC ARM compiler (Windows host) - does not support the %z print flag :(
        m_tmpBuf.format( "\nRX: frame=%6" PRIi32 ", total count=%7" PRIi32 " (total errs=%" PRIu32 "):\n",
                         bytes,
                         byteCount,
                         static_cast<uint32_t>( getErrorCounts() ) );
        m_fd.write( m_tmpBuf );
    }
};


};  // end namespace


////////////////////////////////////////////////////////////////////////////////


void echo_test( Kit::Io::IInputOutput& fd )
{
    printf( "\nStarting Echo test...\n\n" );
    Receiver* rxPtr = new ( std::nothrow ) Receiver( fd );
    Kit::System::Thread::create( *rxPtr, "RX" );

    // Start the scheduler
    Kit::System::enableScheduling();
}
