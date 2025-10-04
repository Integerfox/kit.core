/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/Api.h"
#include "Kit/System/Assert.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Trace.h"
#include "Kit/Text/FString.h"
#include "Kit/Io/Socket/InputOutput.h"
#include "Kit/Io/Socket/ListenerRunnable.h"
#include "Kit/EventQueue/Server.h"

///
using namespace Kit::Io::Socket;

#define SECT_ "_0test"


///////////////////
namespace {


class EchoServer : public IListenerClient
{
public:
    InputOutput m_stream;
    const char* m_exitString;
    size_t      m_byteCount;
public:
    ///
    EchoServer( const char* exitString )
        : m_exitString( exitString )
        , m_byteCount( 0 )
    {
    }

public:
    bool newConnection( KitIoSocketHandle_T& newFd, const char* rawConnectionInfo ) noexcept override
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Incoming connection from: %s (fd=%p)", rawConnectionInfo, (void*)( (size_t)newFd ) );
        m_stream.activate( newFd );
        m_byteCount = 0;

        for ( ;; )
        {
            Kit::Text::FString<256> buffer;
            if ( !m_stream.read( buffer ) )
            {
                KIT_SYSTEM_TRACE_MSG( SECT_, "READER: Read failed" );
                m_stream.close();
                return false;
            }
            m_byteCount += buffer.length();
            KIT_SYSTEM_TRACE_MSG( SECT_, "READER: input (%d : %lu) [%s]", buffer.length(), m_byteCount, buffer.getString() );

            if ( !m_stream.write( buffer ) )
            {
                KIT_SYSTEM_TRACE_MSG( SECT_, "READER: Write failed" );
                m_stream.close();
                return false;
            }
            m_stream.flush();

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


///////////////////
void echoServer( int portToListenOn, const char* exitString ) noexcept
{
    ListenerRunnable listener;
    EchoServer       uut( exitString );

    Kit::System::Thread* t1 = Kit::System::Thread::create( listener, "Listener" );
    KIT_SYSTEM_ASSERT( t1 != nullptr );
    Kit::System::sleep( 250 );  // Pause to all the thread to start

    // Start listener
    KIT_SYSTEM_TRACE_MSG( SECT_, "Listening on port %d 2min....", portToListenOn );
    listener.startListening( uut, portToListenOn );

    // Don't let main exit
    for( ;; )
    {
        Kit::System::sleep( 100000 );
    }
}
