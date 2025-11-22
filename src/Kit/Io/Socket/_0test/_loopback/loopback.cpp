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
#include "Kit/EventQueue/IQueue.h"
#include "Kit/Io/Socket/ListenerRunnable.h"
#include "Kit/Io/Types.h"
#include "Kit/System/Api.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Trace.h"
#include "Kit/Text/FString.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/Io/Socket/ListenerClientSync.h"
#include "Kit/Io/Socket/InputOutput.h"
#include "Kit/Io/Socket/Connector.h"


///
using namespace Kit::Io::Socket;


#define SECT_     "_0test"

#define PORT_NUM_ 5002


///////////////////
namespace {

/// Note: The interaction between a Reader and the LoopBackClient is not truly thread-safe - but it is good enough for the unittest.
class Reader : public Kit::System::IRunnable
{
public:
    ///
    bool m_run;
    ///
    bool* m_doneFlagPtr;
    ///
    InputOutput m_stream;

public:
    ///
    Reader()
        : m_run( true )
        , m_doneFlagPtr( nullptr ) {}


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
        m_stream.close();
        if ( m_parentThreadPtr_ != nullptr )
        {
            m_parentThreadPtr_->signal();
        }
    }

public:
    ///
    void entry() noexcept override
    {
        while ( m_run )
        {
            Kit::System::Thread::wait();

            for ( ;; )
            {
                Kit::Text::FString<256> buffer;
                if ( !m_stream.read( buffer ) )
                {
                    KIT_SYSTEM_TRACE_MSG( SECT_, "READER: Read failed" );
                    break;
                }
                KIT_SYSTEM_TRACE_MSG( SECT_, "READER: input [%s]", buffer.getString() );

                if ( !m_stream.write( buffer ) )
                {
                    KIT_SYSTEM_TRACE_MSG( SECT_, "READER: Write failed" );
                    break;
                }
                m_stream.flush();
                KIT_SYSTEM_TRACE_MSG( SECT_, "READER: echoed [%s]", buffer.getString() );
            }

            KIT_SYSTEM_TRACE_MSG( SECT_, "READER: Exited Loopback loop -->wait for next 'startReader' (m_run=%d)", m_run );
            m_stream.close();
            if ( m_doneFlagPtr )
            {
                *m_doneFlagPtr = true;
            }
        }
    }
};

class LoopBackClient : public ListenerClientSync
{
public:
    ///
    bool m_loop1Free;
    ///
    Reader& m_reader1;
    ///
    bool m_loop2Free;
    ///
    Reader& m_reader2;


public:
    ///
    LoopBackClient( Kit::EventQueue::IQueue& myEventQueue, Reader& reader1, Reader& reader2 )
        : ListenerClientSync( myEventQueue )
        , m_loop1Free( true )
        , m_reader1( reader1 )
        , m_loop2Free( true )
        , m_reader2( reader2 )
    {
    }

public:
    /// Request: NewConnection
    void request( NewConnectionMsg& msg )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Incoming connection from: %s (fd=%p)", msg.getPayload().m_rawConnectionInfo, (void*)( (size_t)msg.getPayload().m_acceptedFd ) );
        if ( m_loop1Free )
        {
            msg.getPayload().m_accepted = true;
            m_reader1.m_stream.activate( msg.getPayload().m_acceptedFd );
            m_reader1.startReading( m_loop1Free );
        }
        else if ( m_loop2Free )
        {
            msg.getPayload().m_accepted = true;
            m_reader2.m_stream.activate( msg.getPayload().m_acceptedFd );
            m_reader2.startReading( m_loop2Free );
        }
        else
        {
            // Note: The default for msg payload is 'm_accepted:=false'
            KIT_SYSTEM_TRACE_MSG( SECT_, "Connection rejected" );
        }

        msg.returnToSender();
    }
};

}  // end anonymous namespace


#define TEXT1 "Hello World"
#define TEXT2 "hello world"
#define TEXT3 "Really Long string.................................................................................................................okay maybe not so long"


///////////////////
TEST_CASE( "loopback" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    Reader reader1;
    Reader reader2;

    ListenerRunnable        listener;
    Kit::EventQueue::Server testApplication;
    LoopBackClient          myClient( testApplication, reader1, reader2 );

    Kit::System::Thread* t1 = Kit::System::Thread::create( listener, "Listener" );
    Kit::System::Thread* t2 = Kit::System::Thread::create( reader1, "Reader1" );
    Kit::System::Thread* t3 = Kit::System::Thread::create( reader2, "Reader2" );
    Kit::System::Thread* t4 = Kit::System::Thread::create( testApplication, "TestApp" );
    Kit::System::sleep( 250 );  // Wait to ensure all threads have started

    // Start listener
    listener.startListening( myClient, PORT_NUM_ );
    KIT_SYSTEM_TRACE_MSG( SECT_, "Listening on port %d ...", PORT_NUM_ );
    Kit::System::sleep( 100 );

    // Connect.  
    // NOTE: Depending on the Host machine - Connector::establish() may take several 
    //       seconds as it iterates through all of the network adapters/IPv4/IPv6/etc.
    KitIoSocketHandle_T clientFd;
    REQUIRE( Connector::establish( "localhost", PORT_NUM_ + 1, clientFd ) != Connector::eSUCCESS );
    REQUIRE( Connector::establish( "localhost", PORT_NUM_, clientFd ) == Connector::eSUCCESS );
    InputOutput         clientStream( clientFd );
    KitIoSocketHandle_T client2Fd;
    REQUIRE( Connector::establish( "localhost", PORT_NUM_, client2Fd ) == Connector::eSUCCESS );
    InputOutput client2Stream( client2Fd );

    // Loop back
    Kit::Text::FString<512> buffer1;
    REQUIRE( clientStream.write( TEXT1 ) );
    minWaitOnStreamData( clientStream, 100, 1000 );
    REQUIRE( clientStream.read( buffer1 ) );
    REQUIRE( buffer1 == TEXT1 );

    Kit::Text::FString<512> buffer2;
    REQUIRE( client2Stream.write( TEXT2 ) );
    minWaitOnStreamData( client2Stream, 100, 1000 );
    REQUIRE( client2Stream.read( buffer2 ) );
    REQUIRE( buffer2 == TEXT2 );

    REQUIRE( clientStream.write( TEXT3 ) );
    minWaitOnStreamData( clientStream, 100, 1000 );
    REQUIRE( clientStream.read( buffer1 ) );
    REQUIRE( buffer1 == TEXT3 );

    REQUIRE( client2Stream.write( TEXT3 ) );
    minWaitOnStreamData( client2Stream, 100, 1000 );
    REQUIRE( client2Stream.read( buffer2 ) );
    REQUIRE( buffer2 == TEXT3 );

    REQUIRE( clientStream.write( TEXT2 ) );
    minWaitOnStreamData( clientStream, 100, 1000 );
    REQUIRE( clientStream.read( buffer1 ) );
    REQUIRE( buffer1 == TEXT2 );

    REQUIRE( client2Stream.write( TEXT1 ) );
    minWaitOnStreamData( client2Stream, 100, 1000 );
    REQUIRE( client2Stream.read( buffer2 ) );
    REQUIRE( buffer2 == TEXT1 );

    clientStream.close();

    KitIoSocketHandle_T client3Fd;
    REQUIRE( Connector::establish( "localhost", PORT_NUM_, client3Fd ) == Connector::eSUCCESS );
    InputOutput client3Stream( client3Fd );

    REQUIRE( client3Stream.write( TEXT3 ) );
    minWaitOnStreamData( client3Stream, 100, 1000 );
    REQUIRE( client3Stream.read( buffer1 ) );
    REQUIRE( buffer1 == TEXT3 );

    REQUIRE( client2Stream.write( TEXT3 ) );
    REQUIRE( minWaitOnStreamData( client2Stream, 100, 1000 ) );
    REQUIRE( client2Stream.read( buffer2 ) );
    REQUIRE( buffer2 == TEXT3 );

    client2Stream.close();
    client3Stream.close();


    // Clean-up
    KIT_SYSTEM_TRACE_MSG( SECT_, "Test done.  Cleaning up..." );
    listener.pleaseStop();
    reader1.terminate();
    reader2.terminate();
    testApplication.pleaseStop();

    // Allow time for threads to stop
    Kit::System::sleep( 250 );
    REQUIRE( t1->isActive() == false );
    REQUIRE( t2->isActive() == false );
    REQUIRE( t3->isActive() == false );
    REQUIRE( t4->isActive() == false );

    Kit::System::Thread::destroy( *t1 );
    Kit::System::Thread::destroy( *t2 );
    Kit::System::Thread::destroy( *t3 );
    Kit::System::Thread::destroy( *t4 );

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
