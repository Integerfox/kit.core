/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/Shutdown.h"
#include "kit_config.h"
#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"
#include "Kit/System/Thread.h"
#include "Kit/System/FatalError.h"
#include "Kit/Text/FString.h"
#include "Kit/Io/IInputOutput.h"
#include <string.h>


#define SECT_ "_0test"

#ifndef TEXT_TX_RINGBUFFER_SIZE
#define TEXT_TX_RINGBUFFER_SIZE 7
#endif

#ifndef TEXT_RX_RINGBUFFER_SIZE
#define TEXT_RX_RINGBUFFER_SIZE 11
#endif

#define MAX_MESSAGE 128

#define MSG1        "[Bob's your uncle, well at least he is someones uncle]"
#define MSG2        "[3.14159]"
#define MSG3        "[And the traditional... hello world!]"


extern void loopback_test( Kit::Io::IInputOutput& fd );


////////////////////////////////////////////////////////////////////////////////
namespace {


class Tx : public Kit::System::IRunnable
{
public:
    Kit::Io::IOutput& m_fd;
    const char*       m_msg1;
    const char*       m_msg2;
    const char*       m_msg3;
    size_t            m_loopCount;

public:
    ///
    Tx( Kit::Io::IOutput& fd, const char* msg1, const char* msg2, const char* msg3 )
        : m_fd( fd ), m_msg1( msg1 ), m_msg2( msg2 ), m_msg3( msg3 ), m_loopCount( 0 )
    {
    }

public:
    ///
    void entry() noexcept override
    {
        for ( ;; )
        {
            Bsp_toggle_debug1();

            Bsp_turn_on_debug1();
            Kit::System::Thread::wait();
            Bsp_turn_off_debug1();
            m_fd.write( m_msg1 );
            
            Bsp_turn_on_debug1();
            Kit::System::Thread::wait();
            Bsp_turn_off_debug1();
            m_fd.write( m_msg2 );
            
            Bsp_turn_on_debug1();
            Kit::System::Thread::wait();
            Bsp_turn_off_debug1();
            m_fd.write( m_msg3 );
            
            m_loopCount++;
        }
    }
};

class Rx : public Kit::System::IRunnable
{
public:
    Kit::Io::IInput&                m_fd;
    Tx&                             m_tx;
    Kit::System::Thread&            m_txThread;
    Kit::Text::FString<MAX_MESSAGE> m_inMsg;
    char                            m_temp[MAX_MESSAGE];
    size_t                          m_loopCount;
public:
    Rx( Kit::Io::IInput& fd, Kit::System::Thread& txThread, Tx& tx )
        : m_fd( fd ), m_tx( tx ), m_txThread( txThread ), m_loopCount( 0 )
    {
    }

public:
    void entry() noexcept override
    {
        // Throw any trash bytes on startup
        while ( m_fd.available() )
        {
            m_fd.read( m_inMsg );
        }

        for ( ;; )
        {
            if ( !rx_message( m_tx.m_msg1 ) )
            {
                Kit::System::FatalError::logf( Kit::System::Shutdown::eFAILURE, "Failed 'msg1' loopback" );
            }
            if ( !rx_message( m_tx.m_msg2 ) )
            {
                Kit::System::FatalError::logf( Kit::System::Shutdown::eFAILURE, "Failed 'msg2' loopback" );
            }
            if ( !rx_message( m_tx.m_msg3 ) )
            {
                Kit::System::FatalError::logf( Kit::System::Shutdown::eFAILURE, "Failed 'msg3' loopback" );
            }

            m_loopCount++;
        }
    }

    bool rx_message( const char* msg )
    {
        m_txThread.signal();

        int len = strlen( msg );
        m_inMsg.clear();
        while ( len )
        {
            char c;
            m_fd.read( c );
            m_inMsg += c;
            len--;
        }

        return m_inMsg == msg;
    }
};


class Led : public Kit::System::IRunnable
{
public:
    ///
    int m_onTime;
    ///
    int m_offTime;

public:
    ///
    Led( int ontime, int offtime )
        : m_onTime( ontime ), m_offTime( offtime )
    {
    }

    ///
    void entry() noexcept override
    {
        for ( ;; )
        {
            Bsp_toggle_debug2();
            Kit::System::sleep( m_onTime );
            Bsp_toggle_debug2();
            Kit::System::sleep( m_offTime );
        }
    }
};


};  // end namespace


////////////////////////////////////////////////////////////////////////////////


void loopback_test( Kit::Io::IInputOutput& fd )
{
    // Create some threads....
    Tx*                  transmitterPtr = new ( std::nothrow ) Tx( fd, MSG1, MSG2, MSG3 );
    Kit::System::Thread* ptr            = Kit::System::Thread::create( *transmitterPtr, "TX" );

    Rx* receiverPtr = new ( std::nothrow ) Rx( fd, *ptr, *transmitterPtr );
    Kit::System::Thread::create( *receiverPtr, "RX" );

    Led* blinkPtr = new ( std::nothrow ) Led( 250, 750 );
    Kit::System::Thread::create( *blinkPtr, "LEDs" );

    // Start the scheduler
    Kit::System::enableScheduling();
}
