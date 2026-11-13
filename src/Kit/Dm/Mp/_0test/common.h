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
#include "Kit/Dm/ObserverCallback.h"
#include "Kit/Itc/OpenCloseSync.h"
#include "Kit/System/Trace.h"
#include "catch2/catch_test_macros.hpp"
#include <string.h>


/** This class records the change notifications of a single Model Point.  It is
    intended to be run by a ManualEventQueue (see
    Kit/_support/testing/ManualEventQueue.h), i.e. the test thread checks the
    recorded results after ManualEventQueue::runUntilIdle() - which makes
    'nothing happened' checks (e.g. no notification when the same value is
    written) deterministic.

    Every notification is counted, and the MP's valid state and value at the
    time of the notification are captured.  Use ManualEventQueue::requestOpen()
    and requestClose() to attach/detach the observer.
 */
template <class MPTYPE, class ELEMTYPE>
class Viewer : public Kit::Itc::OpenCloseSync
{
public:
    ///
    Kit::Dm::ObserverCallback<MPTYPE> m_observerMp1;
    ///
    MPTYPE& m_mp;
    /// Number of change notifications received
    unsigned m_notificationCount;
    /// Valid state of the MP at the last change notification
    bool m_lastValid;
    /// Value of the MP at the last change notification (only meaningful when m_lastValid is true)
    ELEMTYPE m_lastValue;

    /// Constructor
    Viewer( Kit::EventQueue::IQueue& myMbox, MPTYPE& mpToMonitor )
        : Kit::Itc::OpenCloseSync( myMbox )
        , m_observerMp1( myMbox )
        , m_mp( mpToMonitor )
        , m_notificationCount( 0 )
        , m_lastValid( false )
        , m_lastValue()
    {
        m_observerMp1.template setCallback<Viewer<MPTYPE, ELEMTYPE>, &Viewer<MPTYPE, ELEMTYPE>::mp1_changed>( this );
    }

    /// Destructor.  Ensures the observer is detached from the (typically static) MP
    ~Viewer()
    {
        m_mp.detach( m_observerMp1 );
    }

public:
    ///
    void request( Kit::Itc::IOpenRequest::OpenMsg& msg ) noexcept override
    {
        m_mp.attach( m_observerMp1 );
        msg.getPayload().success = true;
        msg.returnToSender();
    }

    ///
    void request( Kit::Itc::ICloseRequest::CloseMsg& msg ) noexcept override
    {
        m_mp.detach( m_observerMp1 );
        msg.getPayload().success = true;
        msg.returnToSender();
    }

public:
    ///
    void mp1_changed( MPTYPE& modelPointThatChanged, Kit::Dm::IObserver& clientObserver ) noexcept
    {
        m_notificationCount++;
        m_lastValid = modelPointThatChanged.readAndSync( m_lastValue, clientObserver );
    }
};

/// Same as Viewer, but for Model Points that contain an array of N elements
template <class MPTYPE, class ELEMTYPE, int N>
class ViewerArray : public Kit::Itc::OpenCloseSync
{
public:
    ///
    Kit::Dm::ObserverCallback<MPTYPE> m_observerMp1;
    ///
    MPTYPE& m_mp;
    /// Number of change notifications received
    unsigned m_notificationCount;
    /// Valid state of the MP at the last change notification
    bool m_lastValid;
    /// Value of the MP at the last change notification (only meaningful when m_lastValid is true)
    ELEMTYPE m_lastValue[N];

    /// Constructor
    ViewerArray( Kit::EventQueue::IQueue& myMbox, MPTYPE& mpToMonitor )
        : Kit::Itc::OpenCloseSync( myMbox )
        , m_observerMp1( myMbox )
        , m_mp( mpToMonitor )
        , m_notificationCount( 0 )
        , m_lastValid( false )
        , m_lastValue{}
    {
        m_observerMp1.template setCallback<ViewerArray<MPTYPE, ELEMTYPE, N>, &ViewerArray<MPTYPE, ELEMTYPE, N>::mp1_changed>( this );
    }

    /// Destructor.  Ensures the observer is detached from the (typically static) MP
    ~ViewerArray()
    {
        m_mp.detach( m_observerMp1 );
    }

public:
    ///
    void request( Kit::Itc::IOpenRequest::OpenMsg& msg ) noexcept override
    {
        m_mp.attach( m_observerMp1 );
        msg.getPayload().success = true;
        msg.returnToSender();
    }

    ///
    void request( Kit::Itc::ICloseRequest::CloseMsg& msg ) noexcept override
    {
        m_mp.detach( m_observerMp1 );
        msg.getPayload().success = true;
        msg.returnToSender();
    }

public:
    ///
    void mp1_changed( MPTYPE& modelPointThatChanged, Kit::Dm::IObserver& clientObserver ) noexcept
    {
        m_notificationCount++;
        m_lastValid = modelPointThatChanged.readAndSync( m_lastValue, N, clientObserver );
    }

    /// Returns true if the last notified value matches 'expected' (N elements)
    bool lastValueEquals( const ELEMTYPE* expected ) const noexcept
    {
        return memcmp( m_lastValue, expected, sizeof( m_lastValue ) ) == 0;
    }
};

#endif  // end header latch