/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/File/Input.h"
#include "Kit/Io/Socket/IListenerClient.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/api.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Trace.h"
#include "Kit/Text/FString.h"
#include "Kit/Io/Socket/InputOutput.h"
#include "Kit/Io/Socket/ListenerRunnable.h"
#include "Kit/Io/Socket/Connector.h"
#include "Kit/EventQueue/Server.h"

///
using namespace Kit::Io::Socket;

#define SECT_     "_0test"

#define PORT_NUM_ 5002


///////////////////
namespace {


class ListenerClientUut : public IListenerClient
{
public:
    InputOutput m_stream;
    const char* m_exitString;
public:
    ///
    ListenerClientUut( const char* exitString )
        : m_exitString( exitString )
    {
    }

public:
    bool newConnection( KitIoSocketHandle_T newFd, const char* rawConnectionInfo ) noexcept override
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Incoming connection from: %s (fd=%p)", rawConnectionInfo, (void*)( (size_t)newFd ) );
        m_stream.activate( newFd );

        for ( ;; )
        {
            Kit::Text::FString<256> buffer;
            if ( !m_stream.read( buffer ) )
            {
                KIT_SYSTEM_TRACE_MSG( SECT_, "READER: Read failed" );
                m_stream.close();
                return false;
            }
            KIT_SYSTEM_TRACE_MSG( SECT_, "READER: input [%s]", buffer.getString() );

            if ( !m_stream.write( buffer ) )
            {
                KIT_SYSTEM_TRACE_MSG( SECT_, "READER: Write failed" );
                m_stream.close();
                return false;
            }
            m_stream.flush();
            KIT_SYSTEM_TRACE_MSG( SECT_, "READER: echoed [%s]", buffer.getString() );

            if ( buffer == m_exitString )
            {
                KIT_SYSTEM_TRACE_MSG( SECT_, "READER: Exit string received.  Closing connection." );
                m_stream.close();
                return false;
            }
        }
    }
};

};  // end anonymous namespace


#define TEXT1 "Hello World"
#define TEXT2 "hello world"
#define TEXT3 "Really Long string.................................................................................................................okay maybe not so long"


///////////////////
TEST_CASE( "loopback-inthread-client" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();


    Reader reader1;
    Reader reader2;

    Kit::Itc::MailboxServer testApplication;
    ListenerClientUut       myClient( testApplication, reader1, reader2 );

    Kit::System::Thread* t1 = Kit::System::Thread::create( *listenerPtr_, "Listener" );
    Kit::System::Thread* t2 = Kit::System::Thread::create( reader1, "Reader1" );
    Kit::System::Thread* t3 = Kit::System::Thread::create( reader2, "Reader2" );
    Kit::System::Thread* t4 = Kit::System::Thread::create( testApplication, "TestApp" );
    Kit::System::Api::sleep( 250 );  // Wait to ensure all threads have started

    // Start listener
    listenerPtr_->startListening( myClient, PORT_NUM_ );
    KIT_SYSTEM_TRACE_MSG( SECT_, ( "Listening on port %d 2min....", PORT_NUM_ ) );
    Kit::System::Api::sleep( 50 );

    // Connect
    Kit::Io::Descriptor clientFd;
    REQUIRE( connectorPtr_->establish( "localhost", PORT_NUM_ + 1, clientFd ) != Connector::eSUCCESS );
    REQUIRE( connectorPtr_->establish( "localhost", PORT_NUM_, clientFd ) == Connector::eSUCCESS );
    InputOutput         clientStream( clientFd );
    Kit::Io::Descriptor client2Fd;
    REQUIRE( connectorPtr_->establish( "localhost", PORT_NUM_, client2Fd ) == Connector::eSUCCESS );
    InputOutput client2Stream( client2Fd );

    // Loop back
    Kit::Text::FString<512> buffer1;
    REQUIRE( clientStream.write( TEXT1 ) );
    minWaitOnStreamData( clientStream, 10, 1000 );
    REQUIRE( clientStream.read( buffer1 ) );
    REQUIRE( buffer1 == TEXT1 );

    Kit::Text::FString<512> buffer2;
    REQUIRE( client2Stream.write( TEXT2 ) );
    minWaitOnStreamData( client2Stream, 10, 1000 );
    REQUIRE( client2Stream.read( buffer2 ) );
    REQUIRE( buffer2 == TEXT2 );

    REQUIRE( clientStream.write( TEXT3 ) );
    minWaitOnStreamData( clientStream, 10, 1000 );
    REQUIRE( clientStream.read( buffer1 ) );
    REQUIRE( buffer1 == TEXT3 );

    REQUIRE( client2Stream.write( TEXT3 ) );
    minWaitOnStreamData( client2Stream, 10, 1000 );
    REQUIRE( client2Stream.read( buffer2 ) );
    REQUIRE( buffer2 == TEXT3 );

    REQUIRE( clientStream.write( TEXT2 ) );
    minWaitOnStreamData( clientStream, 10, 1000 );
    REQUIRE( clientStream.read( buffer1 ) );
    REQUIRE( buffer1 == TEXT2 );

    REQUIRE( client2Stream.write( TEXT1 ) );
    minWaitOnStreamData( client2Stream, 10, 1000 );
    REQUIRE( client2Stream.read( buffer2 ) );
    REQUIRE( buffer2 == TEXT1 );

    clientStream.close();

    Kit::Io::Descriptor client3Fd;
    REQUIRE( connectorPtr_->establish( "localhost", PORT_NUM_, client3Fd ) == Connector::eSUCCESS );
    InputOutput client3Stream( client3Fd );

    REQUIRE( client3Stream.write( TEXT3 ) );
    minWaitOnStreamData( client3Stream, 10, 1000 );
    REQUIRE( client3Stream.read( buffer1 ) );
    REQUIRE( buffer1 == TEXT3 );

    REQUIRE( client2Stream.write( TEXT3 ) );
    REQUIRE( minWaitOnStreamData( client2Stream ) );
    REQUIRE( client2Stream.read( buffer2 ) );
    REQUIRE( buffer2 == TEXT3 );

    client2Stream.close();
    client3Stream.close();


    // Clean-up
    KIT_SYSTEM_TRACE_MSG( SECT_, ( "Test done.  Cleaning up..." ) );
    listenerPtr_->terminate();
    reader1.terminate();
    reader2.terminate();
    testApplication.pleaseStop();

    // Allow time for threads to stop
    Kit::System::Api::sleep( 250 );
    REQUIRE( t1->isRunning() == false );
    REQUIRE( t2->isRunning() == false );
    REQUIRE( t3->isRunning() == false );
    REQUIRE( t4->isRunning() == false );

    Kit::System::Thread::destroy( *t1 );
    Kit::System::Thread::destroy( *t2 );
    Kit::System::Thread::destroy( *t3 );
    Kit::System::Thread::destroy( *t4 );

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
