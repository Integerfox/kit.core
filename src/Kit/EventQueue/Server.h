#ifndef KIT_EVENT_QUEUE_SERVER_h_
#define KIT_EVENT_QUEUE_SERVER_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/EventLoop.h"
#include "Kit/System/IWatchedEventLoop.h"
#include "Kit/Itc/Mailbox.h"


///
#ifndef USE_KIT_EVENT_QUEUE_ITC_MSG_ONLY
#include "Kit/Dm/ChangeHandler.h"
#endif

///
namespace Kit {
///
namespace EventQueue {

#ifndef USE_KIT_EVENT_QUEUE_ITC_MSG_ONLY

/** This concrete class extends the Kit::System::EventLoop interface to include
    processing events from Event Queue.  Depending on the compile time configuration,
    the Event Queue adds events such as:
       1. ITC Messages
       2. Data Model change notifications
 */
class Server : public Kit::Itc::Mailbox,
               public Kit::System::EventLoop
{
public:
    /** Constructor.  See the Kit::System::EventLoop constructor for description
        of the method's arguments.
     */
    Server( uint32_t                                        timeOutPeriodInMsec = OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
            Kit::Container::SList<Kit::System::IEventFlag>* eventFlagsList      = nullptr,
            Kit::System::IWatchedEventLoop*                 watchdog            = nullptr ) noexcept;


    /// See Kit::EventQueue::IChangeNotification
    void addPendingChangingNotification( Kit::Dm::IObserver& observer ) noexcept override
    {
        m_changeHandler.addPendingChangingNotification( observer, *this );
    }

    /// See Kit::EventQueue::IChangeNotification
    void removePendingChangingNotification( Kit::Dm::IObserver& observer ) noexcept override
    {
        m_changeHandler.removePendingChangingNotification( observer );
    }

public:
    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    void pleaseStop() noexcept override                                 { EventLoop::pleaseStop(); }
    
    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    int  signal() noexcept override                                     { return EventLoop::signal(); }
    
    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    int  su_signal() noexcept override                                  { return EventLoop::su_signal(); }
    
    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    void signalMultipleEvents( uint32_t events ) noexcept override      { EventLoop::signalMultipleEvents( events ); }
    
    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    void signalEvent( uint8_t eventNumber ) noexcept override           { EventLoop::signalEvent( eventNumber ); }
    
    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    void su_signalMultipleEvents( uint32_t events ) noexcept override   { EventLoop::su_signalMultipleEvents( events ); }
    
    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    void su_signalEvent( uint8_t eventNumber ) noexcept override        { EventLoop::su_signalEvent( eventNumber ); }
    
    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    void attach( Kit::System::ICounter& c ) noexcept override           { TimerManager::attach( c ); }
    
    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    bool detach( Kit::System::ICounter& c ) noexcept override           { return TimerManager::detach( c ); }
    
    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    uint32_t msecToCounts( uint32_t ms ) const noexcept override        { return TimerManager::msecToCounts( ms ); }

protected:
    /// See Kit::System::IRunnable
    void entry() noexcept override;

protected:
    /// Helper method that waits for events
    virtual bool serverWaitAndProcessEvents() noexcept
    {
        return waitAndProcessEvents( isPendingMessage() || m_changeHandler.isPendingPendingChangingNotifications() );
    }

    /// Helper method that processes events
    virtual void serverProcessEvents() noexcept
    {
        m_changeHandler.processChangeNotifications();
        processMessages();
    }

protected:
    /// Change Handler for processing Data Model change notifications
    Kit::Dm::ChangeHandler m_changeHandler;
};

#else
/** This concrete class extends the Kit::System::EventLoop interface to include
    processing events from Event Queue.  Depending on the compile time configuration,
    the Event Queue adds events such as:
       1. ITC Messages
 */
class Server : public Kit::Itc::Mailbox,
               public Kit::System::EventLoop
{
public:
    /** Constructor.  See the Kit::System::EventLoop constructor for description
        of the method's arguments.
     */
    Server( uint32_t                                        timeOutPeriodInMsec = OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
            Kit::Container::SList<Kit::System::IEventFlag>* eventFlagsList      = nullptr,
            Kit::System::IWatchedEventLoop*                 watchdog            = nullptr ) noexcept;

public:
    // Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    void pleaseStop() noexcept override                                 { EventLoop::pleaseStop(); }
    int  signal() noexcept override                                     { return EventLoop::signal(); }
    int  su_signal() noexcept override                                  { return EventLoop::su_signal(); }
    void signalMultipleEvents( uint32_t events ) noexcept override      { EventLoop::signalMultipleEvents( events ); }
    void signalEvent( uint8_t eventNumber ) noexcept override           { EventLoop::signalEvent( eventNumber ); }
    void su_signalMultipleEvents( uint32_t events ) noexcept override   { EventLoop::su_signalMultipleEvents( events ); }
    void su_signalEvent( uint8_t eventNumber ) noexcept override        { EventLoop::su_signalEvent( eventNumber ); }
    void attach( Kit::System::ICounter& c ) noexcept override           { TimerManager::attach( c ); }
    bool detach( Kit::System::ICounter& c ) noexcept override           { return TimerManager::detach( c ); }
    uint32_t msecToCounts( uint32_t ms ) const noexcept override        { return TimerManager::msecToCounts( ms ); }

protected:
    /// See Kit::System::IRunnable
    void entry() noexcept override;

protected:
    /// Helper method that waits for events
    virtual bool serverWaitAndProcessEvents() noexcept
    {
        return waitAndProcessEvents( isPendingMessage() );
    }

    /// Helper method that processes events
    virtual void serverProcessEvents() noexcept
    {
        processMessages();
    }
};

#endif  // end !USE_KIT_EVENT_QUEUE_ITC_MSG_ONLY

}  // end namespaces
}
#endif  // end header latch