#ifndef KIT_EVENT_QUEUE_SERVER_BASE_H_
#define KIT_EVENT_QUEUE_SERVER_BASE_H_
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
#include "Kit/System/EventLoop.h"
#include "Kit/System/IWatchedEventLoop.h"
#include "Kit/Itc/Mailbox.h"

///
namespace Kit {
///
namespace EventQueue {

/** This concrete class extends the Kit::System::EventLoop interface to include
    processing ITC messages from the Event Queue.  It provides the event
    processing that is common to all of the Event Queue's compile time
    configurations.

    The application uses the Kit::EventQueue::Server class - NOT this class.
    When USE_KIT_EVENT_QUEUE_ITC_MSG_ONLY is defined, Kit::EventQueue::Server
    is this class; else Kit::EventQueue::Server is a child class that adds
    processing of Data Model change notifications.

    NOTE: The configuration specific code is in different classes (instead of
          different definitions of the same class) so that every method has
          exactly one source location.  This is required when the code
          coverage metrics from builds with different configurations are
          merged.
 */
class ServerBase : public Kit::Itc::Mailbox,
                   public Kit::System::EventLoop
{
public:
    /** Constructor.  See the Kit::System::EventLoop constructor for description
        of the method's arguments.
     */
    ServerBase( uint32_t                                        timeOutPeriodInMsec = OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
                Kit::Container::SList<Kit::System::IEventFlag>* eventFlagsList      = nullptr,
                Kit::System::IWatchedEventLoop*                 watchdog            = nullptr ) noexcept;

public:
    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    void pleaseStop() noexcept override;

    /// See Kit::EventQueue::IMsgNotification
    void postSync( Kit::Itc::IMessage& msg ) noexcept override;

    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    int signal() noexcept override { return EventLoop::signal(); }

    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    int su_signal() noexcept override { return EventLoop::su_signal(); }

    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    void signalMultipleEvents( uint32_t events ) noexcept override { EventLoop::signalMultipleEvents( events ); }

    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    void signalEvent( uint8_t eventNumber ) noexcept override { EventLoop::signalEvent( eventNumber ); }

    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    void su_signalMultipleEvents( uint32_t events ) noexcept override { EventLoop::su_signalMultipleEvents( events ); }

    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    void su_signalEvent( uint8_t eventNumber ) noexcept override { EventLoop::su_signalEvent( eventNumber ); }

    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    void attach( Kit::System::ICounter& c ) noexcept override { TimerManager::attach( c ); }

    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    bool detach( Kit::System::ICounter& c ) noexcept override { return TimerManager::detach( c ); }

    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    uint32_t msecToCounts( uint32_t ms ) const noexcept override { return TimerManager::msecToCounts( ms ); }

    /// Explicit overrides to resolve MSVC C4250 dominance warnings (diamond inheritance via IQueue/EventLoop)
    bool isRunning( const Kit::System::ICounter& timerToInspect ) const noexcept override { return TimerManager::isRunning( timerToInspect ); }

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


}  // end namespaces
}
#endif  // end header latch
