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
#include "Kit/EventQueue/Server.h"
#include "Kit/Dm/ObserverCallback.h"
#include "Kit/Itc/OpenCloseSync.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"
#include "catch2/catch_test_macros.hpp"

/// Macro to help shutdown the observer thread
#define WAIT_FOR_THREAD_TO_STOP( t )  \
    for ( int i = 0; i < 200; i++ )   \
    {                                 \
        Kit::System::sleep( 50 );     \
        if ( t->isActive() == false ) \
        {                             \
            break;                    \
        }                             \
    }


template <class MPTYPE, class ELEMTYPE>
class Viewer : public Kit::Itc::OpenCloseSync
{
public:
    ///
    Kit::System::Thread& m_masterThread;
    ///
    Kit::Dm::ObserverCallback<MPTYPE> m_observerMp1;
    ///
    MPTYPE& m_mp;
    ///
    ELEMTYPE& m_elemValue;
    ///
    bool m_firstTime;

    /// Constructor
    Viewer( Kit::EventQueue::Server& myMbox, Kit::System::Thread& masterThread, MPTYPE& mpToMonitor, ELEMTYPE& elemValue )
        : Kit::Itc::OpenCloseSync( myMbox )
        , m_masterThread( masterThread )
        , m_observerMp1( myMbox )
        , m_mp( mpToMonitor )
        , m_elemValue( elemValue )
        , m_firstTime( true )
    {
        m_observerMp1.template setCallback<Viewer<MPTYPE, ELEMTYPE>, &Viewer<MPTYPE, ELEMTYPE>::mp1_changed>( this );
    }

public:
    ///
    void request( Kit::Itc::IOpenRequest::OpenMsg& msg ) noexcept override
    {
        m_mp.attach( m_observerMp1 );
        msg.returnToSender();
    }

    ///
    void request( Kit::Itc::ICloseRequest::CloseMsg& msg ) noexcept override
    {
        m_mp.detach( m_observerMp1 );
        msg.returnToSender();
    }


public:
    void mp1_changed( MPTYPE& modelPointThatChanged, Kit::Dm::IObserver& clientObserver ) noexcept
    {
        if ( m_firstTime && modelPointThatChanged.isNotValidAndSync( clientObserver ) == false )
        {
            m_firstTime = false;
            ELEMTYPE elem;
            REQUIRE( modelPointThatChanged.readAndSync( elem, clientObserver ) );
            displayElement( "new", elem );
            displayElement( "exp", m_elemValue );

            m_masterThread.signal();
        }
    }

    /** Note: Template specialization can be used to properly display the MP value
        for the Observer test.  For example - MP Type is BETTER_ENUM
        \code
            template <>
            void Viewer<MyUut, MyEnum>::displayElement( const char* label, MyEnum& elem )
            {
                KIT_SYSTEM_TRACE_MSG( SECT_, "%s:%s", label, elem._to_string() );
            }
        \endcode
    */
    void displayElement( const char* label, ELEMTYPE& elem )
    {
        KIT_SYSTEM_TRACE_MSG( "_0test", "%s:%d", label, elem );
    }
};

#endif  // end header latch