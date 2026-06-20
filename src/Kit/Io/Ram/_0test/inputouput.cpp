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
#include "Kit/_support/testing/helpers.h"
#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"
#include "Kit/System/IRunnable.h"
#include "Kit/System/Thread.h"
#include "Kit/Text/FString.h"
#include "Kit/Io/Ram/InputOutputAllocate.h"

#include <string.h>
#include <inttypes.h>


#define SECT_ "_0test"

///
using namespace Kit::Io::Ram;
using namespace Kit::Type;


////////////////////////////////////////////////////////////////////////////////
/// Use anonymous namespace to make my class local-to-the-file in scope
namespace {

///
class Writer : public Kit::System::IRunnable
{
public:
    bool                   m_success;
    unsigned               m_writeOperations;
    const char*            m_stringToWrite;
    bool                   m_run;
    bool*                  m_doneFlagPtr;
    Kit::Io::IInputOutput& m_stream;

public:
    ///
    Writer( Kit::Io::IInputOutput& fd, const char* writeString )
        : m_writeOperations( 0 )
        , m_stringToWrite( writeString )
        , m_run( true )
        , m_doneFlagPtr( 0 )
        , m_stream( fd ) {}


public:
    ///
    void startWriting( bool& doneFlag )
    {
        doneFlag      = false;
        m_doneFlagPtr = &doneFlag;
        m_parentThreadPtr_->signal();
    }
    ///
    void terminate()
    {
        m_run = false;
        m_parentThreadPtr_->signal();
    }

public:
    ///
    void entry() noexcept override
    {
        m_success = true;
        while ( m_run && m_success )
        {
            Kit::System::Thread::wait();
            if ( m_run && m_success )  // check again
            {
                m_writeOperations++;
                m_success = m_stream.write( m_stringToWrite );
                if ( m_doneFlagPtr )
                {
                    *m_doneFlagPtr = true;
                }
            }
        }
    }
};

///
class Reader : public Kit::System::IRunnable
{
public:
    bool                   m_success;
    unsigned               m_readOperations;
    char*                  m_dstBuffer;
    SSize_T                m_bytesToRead;
    SSize_T                m_bytesRead;
    bool                   m_run;
    bool*                  m_doneFlagPtr;
    Kit::Io::IInputOutput& m_stream;

public:
    ///
    Reader( Kit::Io::IInputOutput& fd, char* dst, SSize_T numBytesToRead )
        : m_readOperations( 0 )
        , m_dstBuffer( dst )
        , m_bytesToRead( numBytesToRead )
        , m_run( true )
        , m_doneFlagPtr( 0 )
        , m_stream( fd ) {}


public:
    ///
    void startReading( bool& doneFlag )
    {
        doneFlag      = false;
        m_doneFlagPtr = &doneFlag;
        m_parentThreadPtr_->signal();
    }
    ///
    void terminate()
    {
        m_run = false;
        m_parentThreadPtr_->signal();
    }
public:
    ///
    void entry() noexcept override
    {
        m_success = true;
        while ( m_run && m_success )
        {
            Kit::System::Thread::wait();
            if ( m_run && m_success )  // check again
            {
                m_bytesRead            = 0;
                SSize_T bytesRemaining = m_bytesToRead;
                SSize_T bytesRead;
                char    buf[100] = {
                    0,
                };
                memset( m_dstBuffer, 0, m_bytesToRead + 1 );
                m_readOperations++;

                while ( bytesRemaining )
                {
                    if ( !m_stream.read( buf, bytesRemaining, bytesRead ) )
                    {
                        m_success = false;
                        break;
                    }
                    strncat( m_dstBuffer, buf, bytesRead );
                    m_bytesRead    += bytesRead;
                    bytesRemaining -= bytesRead;
                }

                if ( m_doneFlagPtr )
                {
                    *m_doneFlagPtr = true;
                }
            }
        }
    }
};

}  // end anonymous namespace


////////////////////////////////////////////////////////////////////////////////
#define MAX_ELEMENTS 16

#define TEST_STRING1 "hello world"
#define TEST_STRING2 "I am doing fine"
#define TEST_STRING3 "That's great"

TEST_CASE( "inputoutput" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    Kit::Text::FString<MAX_ELEMENTS>  tmpString;
    InputOutputAllocate<MAX_ELEMENTS> uut;

    SECTION( "single threaded" )
    {
        bool result = uut.available();
        REQUIRE( result == false );
        result = uut.isEos();
        REQUIRE( result == false );

        result = uut.write( TEST_STRING1 );
        REQUIRE( result == true );
        uut.flush(); // Flush should do nothing
        result = uut.available();
        REQUIRE( result == true );
        result = uut.read( tmpString );
        REQUIRE( result == true );

        // Zero byte read/write should succeed but do nothing
        SSize_T bytesResult = -1;
        result = uut.write( (void*)TEST_STRING1, 0, bytesResult );
        REQUIRE( bytesResult == 0 );
        REQUIRE( result == true );
        bytesResult = -1;
        result = uut.read( (void*)TEST_STRING1, 0, bytesResult );
        REQUIRE( bytesResult == 0 );
        REQUIRE( result == true );

        uut.close();
        result = uut.available();
        REQUIRE( result == false );
        result = uut.read( tmpString );
        REQUIRE( result == false );
        result = uut.write( TEST_STRING1 );
        REQUIRE( result == false );
        result = uut.isEos();
        REQUIRE( result == true );
    }

    SECTION( "single writer, single reader" )
    {
        Writer writer( uut, TEST_STRING2 );
        char   reader1Buf[100];
        Reader reader1( uut, reader1Buf, strlen( TEST_STRING2 ) );

        Kit::System::Thread* w1 = Kit::System::Thread::create( writer, "Writer" );
        Kit::System::Thread* r1 = Kit::System::Thread::create( reader1, "Reader1" );
        REQUIRE( waitThreadActive( w1 ) == true );
        REQUIRE( waitThreadActive( r1 ) == true );

        // Start my readers
        bool reader1Done;
        reader1.startReading( reader1Done );
        Kit::System::sleep( 200 );  // Wait to ensure all reader loops have started
        REQUIRE( reader1Done == false );

        // Start my write
        bool writerDone;
        writer.startWriting( writerDone );
        Kit::System::sleep( 500 );  // Wait to ensure write is done
        REQUIRE( writerDone == true );
        REQUIRE( writer.m_success == true );
        REQUIRE( writer.m_writeOperations == 1 );

        // Did I read the data?
        REQUIRE( reader1.m_bytesRead == strlen( TEST_STRING2 ) );
        REQUIRE( reader1.m_readOperations == 1 );

        // Shutdown the threads
        writer.terminate();
        reader1.terminate();
        uut.close();  // This will unblock all clients

        // Allow time for threads to stop
        REQUIRE( waitThreadInactive( w1 ) == true );
        REQUIRE( waitThreadInactive( r1 ) == true );

        Kit::System::Thread::destroy( *w1 );
        Kit::System::Thread::destroy( *r1 );
    }

    SECTION( "blocked reader" )
    {
        char                 reader1Buf[100];
        Reader               reader1( uut, reader1Buf, strlen( TEST_STRING2 ) );
        Kit::System::Thread* r1 = Kit::System::Thread::create( reader1, "Reader1" );
        REQUIRE( waitThreadActive( r1 ) == true );

        Writer               writer( uut, TEST_STRING2 );
        Kit::System::Thread* w1 = Kit::System::Thread::create( writer, "Writer" );
        REQUIRE( waitThreadActive( w1 ) == true );

        // Start my reader (which will block waiting for data)
        bool reader1Done;
        reader1.startReading( reader1Done );
        Kit::System::sleep( 200 );  // Wait to ensure all reader loops have started
        REQUIRE( reader1Done == false );

        // Start my write
        bool writerDone;
        writer.startWriting( writerDone );
        Kit::System::sleep( 500 );  // Wait to ensure write is done
        REQUIRE( writerDone == true );
        REQUIRE( writer.m_success == true );
        REQUIRE( writer.m_writeOperations == 1 );
        REQUIRE( reader1.m_bytesRead == strlen( TEST_STRING2 ) );
        REQUIRE( reader1.m_readOperations == 1 );
        REQUIRE( reader1Done == true );

        // Start my reader AGAIN (which will block waiting for data)
        reader1.startReading( reader1Done );
        Kit::System::sleep( 200 );  // Wait to ensure all reader loops have started
        REQUIRE( reader1Done == false );

        // Start my write AGAIN
        writer.startWriting( writerDone );
        Kit::System::sleep( 500 );  // Wait to ensure write is done
        REQUIRE( writerDone == true );
        REQUIRE( writer.m_success == true );
        REQUIRE( writer.m_writeOperations == 2 );
        REQUIRE( reader1.m_bytesRead == strlen( TEST_STRING2 ) );
        REQUIRE( reader1.m_readOperations == 2 );
        REQUIRE( reader1Done == true );

        // Start my reader AGAIN (which will block waiting for data)
        reader1.startReading( reader1Done );
        Kit::System::sleep( 200 );  // Wait to ensure all reader loops have started
        REQUIRE( reader1Done == false );
        uut.close();  // This will unblock all clients

        // Allow time for threads to stop
        writer.terminate();
        reader1.terminate();
        REQUIRE( waitThreadInactive( w1 ) == true );
        REQUIRE( waitThreadInactive( r1 ) == true );

        Kit::System::Thread::destroy( *w1 );
        Kit::System::Thread::destroy( *r1 );
    }

    SECTION( "blocked writer" )
    {
        char                 reader1Buf[100];
        Reader               reader1( uut, reader1Buf, strlen( TEST_STRING2 ) );
        Kit::System::Thread* r1 = Kit::System::Thread::create( reader1, "Reader1" );
        REQUIRE( waitThreadActive( r1 ) == true );

        Writer               writer( uut, TEST_STRING2 );
        Kit::System::Thread* w1 = Kit::System::Thread::create( writer, "Writer" );
        REQUIRE( waitThreadActive( w1 ) == true );

        // Start my write
        bool writerDone;
        writer.startWriting( writerDone );
        Kit::System::sleep( 200 );  // Wait to ensure write is done
        REQUIRE( writerDone == true );
        REQUIRE( writer.m_success == true );
        REQUIRE( writer.m_writeOperations == 1 );

        // Trigger the write again -->should block this time since the RingBuffer is only big enough for one write
        writer.startWriting( writerDone );
        Kit::System::sleep( 200 );  // Wait to ensure write is blocked
        REQUIRE( writerDone == false );

        // Trigger the read to unblock the writer
        bool reader1Done;
        reader1.startReading( reader1Done );
        Kit::System::sleep( 500 );  // Wait to ensure read is done
        REQUIRE( reader1Done == true );
        REQUIRE( writerDone == true );
        REQUIRE( writer.m_success == true );
        REQUIRE( writer.m_writeOperations == 2 );
        REQUIRE( reader1.m_bytesRead == strlen( TEST_STRING2 ) );
        REQUIRE( reader1.m_readOperations == 1 );

        // Trigger the write again -->should block again
        writer.startWriting( writerDone );
        Kit::System::sleep( 200 );  // Wait to ensure write is blocked
        REQUIRE( writerDone == false );
        uut.close();  // This will unblock all clients

        // Allow time for threads to stop. 
        writer.terminate();
        reader1.terminate();
        REQUIRE( waitThreadInactive( w1 ) == true );
        REQUIRE( waitThreadInactive( r1 ) == true );

        Kit::System::Thread::destroy( *w1 );
        Kit::System::Thread::destroy( *r1 );
    }

    uut.close();
    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
