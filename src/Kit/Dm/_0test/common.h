#ifndef common_h_
#define common_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Dm/IObserver.h"
#include "Kit/EventQueue/IQueue.h"
#include "Kit/Itc/IOpenRequest.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/EventQueue/Server.h"
#include "Kit/Dm/ObserverCallback.h"
#include "Kit/Dm/Mp/Uint32.h"
#include "Kit/Itc/OpenCloseSync.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Api.h"
#include "Kit/System/Timer.h"

#define SECT_ "_0test"


///
using namespace Kit::Dm;


/////////////////////////////////////////////////////////////////
class Viewer : public Kit::Itc::OpenCloseSync
{
public:
    ///
    volatile bool m_opened;
    ///
    Kit::System::Thread& m_masterThread;
    ///
    Kit::Itc::IOpenRequest::OpenMsg* m_pendingOpenMsgPtr;
    ///
    Mp::Uint32& m_mp1;
    ///
    uint32_t m_mpNotificationCount1;
    ///
    uint32_t m_mpEndValue;
    ///
    ObserverCallback<Mp::Uint32> m_observerMp1;
    ///
    uint16_t m_lastSeqNumber;
    ///
    uint32_t m_lastValue;
    ///
    bool m_lastValid;
    ///
    bool m_done;

    /// Constructor
    Viewer( Kit::EventQueue::IQueue& myMbox,
            Kit::System::Thread&     masterThread,
            Mp::Uint32&              mp1,
            uint32_t                 mpEndValue1 )
        : Kit::Itc::OpenCloseSync( myMbox )
        , m_opened( false )
        , m_masterThread( masterThread )
        , m_pendingOpenMsgPtr( 0 )
        , m_mp1( mp1 )
        , m_mpNotificationCount1( 0 )
        , m_mpEndValue( mpEndValue1 )
        , m_lastSeqNumber( IModelPoint::SEQUENCE_NUMBER_UNKNOWN )
        , m_lastValue( 0 )
        , m_lastValid( false )
        , m_done( false )
    {
        m_observerMp1.setEventQueue( myMbox );
        m_observerMp1.setCallback<Viewer, &Viewer::mp1_changed>( this );
        KIT_SYSTEM_TRACE_MSG( SECT_, "VIEWER(%p). mp1=%s, endVal=%" PRIu32, this, mp1.getName(), mpEndValue1 );
    }

public:
    ///
    void request( Kit::Itc::IOpenRequest::OpenMsg& msg ) noexcept override
    {
        if ( m_opened )
        {
            FAIL( "OPENING Viewer more than ONCE" );
        }

        m_pendingOpenMsgPtr    = &msg;
        m_mpNotificationCount1 = 0;
        KIT_SYSTEM_TRACE_MSG( SECT_, "SUBSCRIBING (%p) for Change notification. current value =%" PRIu32, this, m_mpNotificationCount1 );

        // Subscribe to my model point
        m_mp1.attach( m_observerMp1 );

        // Note: The open message will be returned once all the model point receives its initial callback
    }

    ///
    void request( Kit::Itc::ICloseRequest::CloseMsg& msg ) noexcept override
    {
        if ( !m_opened )
        {
            FAIL( "CLOSING Viewer more than ONCE" );
        }

        KIT_SYSTEM_TRACE_MSG( SECT_, "VIEWER(%p): Closing... ", this );

        // Un-subscribe to my model point
        m_mp1.detach( m_observerMp1 );
        m_opened = false;
        msg.returnToSender();
    }


public:
    void mp1_changed( Mp::Uint32& modelPointThatChanged, IObserver& clientObserver ) noexcept
    {
        KIT_SYSTEM_TRACE_ALLOCATE( uint32_t, prevValue, m_lastValue );
        KIT_SYSTEM_TRACE_ALLOCATE( int8_t, prevState, m_lastValid );
        uint16_t prevSeqNum = m_lastSeqNumber;

        m_mpNotificationCount1++;
        uint16_t seqNum;
        m_lastValid     = modelPointThatChanged.readAndSync( m_lastValue, seqNum, clientObserver );
        m_lastSeqNumber = m_observerMp1.getSequenceNumber_();
        REQUIRE( m_lastSeqNumber == seqNum );


        if ( m_pendingOpenMsgPtr != 0 && m_mpNotificationCount1 == 1 )
        {
            m_pendingOpenMsgPtr->returnToSender();
            m_opened               = true;
            m_pendingOpenMsgPtr    = 0;
            m_mpNotificationCount1 = 0;
        }

        if ( m_lastValid && m_lastValue >= m_mpEndValue )
        {
            if ( m_done )
            {
                KIT_SYSTEM_TRACE_MSG( SECT_, "Viewer::mp1_changed(%p): Received Change notification after signaling the master thread, may or may not be an error. Prev: value=%" PRIu32 ", state=%d, seqNum=%" PRIu16 ".  Rcvd: value=%" PRIu32 ", state=%d, seqNum=%" PRIu16 ".  read_seq_num=%" PRIu16 ", notifyCount=%" PRIu32, this, prevValue, prevState, prevSeqNum, m_lastValue, m_lastValid, m_lastSeqNumber, m_lastSeqNumber, m_mpNotificationCount1 );
            }
            else
            {
                KIT_SYSTEM_TRACE_MSG( SECT_, "Viewer::mp1_changed(%p, %s): Signaling master thread", this, m_mp1.getName() );
                m_mp1.detach( m_observerMp1 );
                m_masterThread.signal();
                m_done = true;
            }
        }

        REQUIRE( prevSeqNum != m_lastSeqNumber );
    }
};

/////////////////////////////////////////////////////////////////
class Writer : public Kit::Itc::OpenCloseSync
{
public:
    ///
    volatile bool m_opened;
    ///
    Kit::System::Thread& m_masterThread;
    ///
    Mp::Uint32& m_mp1;
    ///
    uint32_t m_intervalMsec;
    ///
    uint32_t m_writeCount;
    ///
    uint32_t m_currentValue;
    ///
    uint32_t m_endValue;
    ///
    uint32_t m_stepSize;
    ///
    Kit::System::TimerComposer<Writer> m_timer;

    /// Constructor
    Writer( Kit::EventQueue::IQueue& myMbox,
            Kit::System::Thread&     masterThread,
            Mp::Uint32&              mp1,
            uint32_t                 intervalMsec,
            uint32_t                 startValue,
            uint32_t                 endValue,
            uint32_t                 stepSize )
        : Kit::Itc::OpenCloseSync( myMbox )
        , m_opened( false )
        , m_masterThread( masterThread )
        , m_mp1( mp1 )
        , m_intervalMsec( intervalMsec )
        , m_writeCount( 0 )
        , m_currentValue( startValue )
        , m_endValue( endValue )
        , m_stepSize( stepSize )
        , m_timer( *this, &Writer::timerExpired, myMbox )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "WRITER(%p). mp1=%s, endVal=%" PRIu32 ", interval=%" PRIu32, this, mp1.getName(), endValue, intervalMsec );
    }

public:
    ///
    void request( Kit::Itc::IOpenRequest::OpenMsg& msg ) noexcept override
    {
        if ( m_opened )
        {
            FAIL( "OPENING Writer more than ONCE" );
        }

        KIT_SYSTEM_TRACE_MSG( SECT_, "WRITER(%p): Starting interval timer (%" PRIu32 ")", this, m_intervalMsec );
        m_opened = true;
        m_timer.start( m_intervalMsec );
        msg.returnToSender();
    }

    ///
    void request( Kit::Itc::ICloseRequest::CloseMsg& msg ) noexcept override
    {
        if ( !m_opened )
        {
            FAIL( "CLOSING Writer more than ONCE" );
        }

        KIT_SYSTEM_TRACE_MSG( SECT_, "WRITER(%p): Closing... ", this );

        // Stop my writ timer
        m_opened = false;
        m_timer.stop();
        msg.returnToSender();
    }


public:
    ///
    void timerExpired( void )
    {
        if ( m_opened )
        {
            m_writeCount++;
            m_mp1.write( m_currentValue );
            Kit::System::sleep( 40 );  // Add a delay because Catch2 is not thread safe

            if ( m_currentValue >= m_endValue )
            {
                KIT_SYSTEM_TRACE_MSG( SECT_, "Writer::timerExpired(%p): Signaling master thread", this );
                m_masterThread.signal();
            }
            else
            {
                m_currentValue += m_stepSize;
                m_timer.start( m_intervalMsec );
            }
        }
        else
        {
            FAIL( "WRITER: SHOULD NOT HAPPEN - The interval timer expired before the Writer was opened" );
        }
    }
};


/////////////////////////////////////////////////////////////////
class GenericViewer : public Kit::Itc::OpenCloseSync
{
public:
    ///
    volatile bool m_opened;
    ///
    Kit::System::Thread& m_masterThread;
    ///
    Kit::Itc::IOpenRequest::OpenMsg* m_pendingOpenMsgPtr;
    ///
    IModelPoint& m_mp1;
    ///
    IModelPoint& m_mp2;
    ///
    IModelPoint& m_mp3;
    ///
    unsigned m_maxNotifyCounts;
    ///
    uint32_t m_mpNotificationCount;
    ///
    ObserverCallback<IModelPoint> m_observerMp1;
    ///
    ObserverCallback<IModelPoint> m_observerMp2;
    ///
    ObserverCallback<IModelPoint> m_observerMp3;
    ///
    uint16_t m_lastSeqNumber;
    ///
    bool m_lastValid;
    ///
    bool m_done;

    /// Constructor
    GenericViewer( Kit::EventQueue::Server& myMbox,
                   Kit::System::Thread&     masterThread,
                   IModelPoint&             mp1,
                   IModelPoint&             mp2,
                   IModelPoint&             mp3,
                   unsigned                 maxNotifyCounts )
        : Kit::Itc::OpenCloseSync( myMbox )
        , m_opened( false )
        , m_masterThread( masterThread )
        , m_pendingOpenMsgPtr( 0 )
        , m_mp1( mp1 )
        , m_mp2( mp2 )
        , m_mp3( mp3 )
        , m_maxNotifyCounts( maxNotifyCounts )
        , m_mpNotificationCount( 0 )
        , m_observerMp1( myMbox )
        , m_observerMp2( myMbox )
        , m_observerMp3( myMbox )
        , m_lastSeqNumber( IModelPoint::SEQUENCE_NUMBER_UNKNOWN )
        , m_lastValid( false )
        , m_done( false )
    {
        m_observerMp1.setCallback<GenericViewer,&GenericViewer::mpChanged>(this);
        m_observerMp2.setCallback<GenericViewer,&GenericViewer::mpChanged>(this);
        m_observerMp3.setCallback<GenericViewer,&GenericViewer::mpChanged>(this);
        KIT_SYSTEM_TRACE_MSG( SECT_, "GENERIC VIEWER(%p). mp1=%s", this, mp1.getName() );
    }

public:
    ///
    void request( Kit::Itc::IOpenRequest::OpenMsg& msg ) noexcept override
    {
        if ( m_opened )
        {
            FAIL( "OPENING Generic Viewer more than ONCE" );
        }

        m_pendingOpenMsgPtr   = &msg;
        m_mpNotificationCount = 0;
        KIT_SYSTEM_TRACE_MSG( SECT_, "GENERIC SUBSCRIBING (%p) for Change notification. current value =%u", this, m_mpNotificationCount );

        // Subscribe to my model point
        m_mp1.genericAttach( m_observerMp1 );
        m_mp2.genericAttach( m_observerMp2 );
        m_mp3.genericAttach( m_observerMp3 );

        // Note: The open message will be returned once all the model point receives its initial callback
    }

    ///
    void request( Kit::Itc::ICloseRequest::CloseMsg& msg ) noexcept override
    {
        if ( !m_opened )
        {
            FAIL( "CLOSING GENERIC Viewer more than ONCE" );
        }

        KIT_SYSTEM_TRACE_MSG( SECT_, "GENERIC VIEWER(%p): Closing... ", this );

        // Un-subscribe to my model point
        m_mp1.genericDetach( m_observerMp1 );
        m_mp2.genericDetach( m_observerMp2 );
        m_mp3.genericDetach( m_observerMp3 );
        m_opened = false;
        msg.returnToSender();
    }


public:
    void mpChanged( IModelPoint& modelPointThatChanged, IObserver& clientObserver ) noexcept
    {
        m_mpNotificationCount++;
        m_lastValid     = !modelPointThatChanged.isNotValid();
        m_lastSeqNumber = m_observerMp1.getSequenceNumber_();

        if ( m_pendingOpenMsgPtr != 0 && m_mpNotificationCount == 3 )
        {
            m_pendingOpenMsgPtr->returnToSender();
            m_opened              = true;
            m_pendingOpenMsgPtr   = 0;
            m_mpNotificationCount = 0;
        }

        if ( m_lastValid && m_mpNotificationCount >= m_maxNotifyCounts / 2 )  // Assume at least half the change notifications
        {
            if ( m_mpNotificationCount > m_maxNotifyCounts )
            {
                KIT_SYSTEM_TRACE_MSG( SECT_, "Generic View: Received TOO many change notification (%d, max=%d)", m_mpNotificationCount, m_maxNotifyCounts );
                FAIL( "Generic View: Received TOO many change notification" );
            }

            if ( m_done )
            {
                KIT_SYSTEM_TRACE_MSG( SECT_, "Generic Viewer::mpChanged(%p): Received Change notification after signaling the master thread, may or may not be an error.", this );
            }
            else
            {
                KIT_SYSTEM_TRACE_MSG( SECT_, "GenericViewer::mp1Changed(%p): Signaling master thread", this );
                m_mp1.genericDetach( m_observerMp1 );
                m_mp2.genericDetach( m_observerMp2 );
                m_mp3.genericDetach( m_observerMp3 );
                m_masterThread.signal();
                m_done = true;
            }
        }
    }
};

#endif  // common_h_