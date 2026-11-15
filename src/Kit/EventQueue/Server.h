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

#include "kit_config.h"
#include "Kit/EventQueue/ServerBase.h"


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
    processing events from Event Queue.  The Event Queue's events are:
       1. ITC Messages
       2. Data Model change notifications

    NOTE: When USE_KIT_EVENT_QUEUE_ITC_MSG_ONLY is defined, the Data Model
          change notifications are NOT supported, and Kit::EventQueue::Server
          is the Kit::EventQueue::ServerBase class.
 */
class Server : public ServerBase
{
public:
    /** Constructor.  See the Kit::System::EventLoop constructor for description
        of the method's arguments.
     */
    Server( uint32_t                                        timeOutPeriodInMsec = OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
            Kit::Container::SList<Kit::System::IEventFlag>* eventFlagsList      = nullptr,
            Kit::System::IWatchedEventLoop*                 watchdog            = nullptr ) noexcept
        : ServerBase( timeOutPeriodInMsec, eventFlagsList, watchdog )
    {
    }

public:
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

protected:
    /// See Kit::EventQueue::ServerBase
    bool serverWaitAndProcessEvents() noexcept override
    {
        return waitAndProcessEvents( isPendingMessage() || m_changeHandler.isPendingChangingNotifications() );
    }

    /// See Kit::EventQueue::ServerBase
    void serverProcessEvents() noexcept override
    {
        m_changeHandler.processChangeNotifications();
        processMessages();
    }

protected:
    /// Change Handler for processing Data Model change notifications
    Kit::Dm::ChangeHandler m_changeHandler;
};

#else

/// ITC messages only, i.e. no Data Model change notifications (see Kit::EventQueue::ServerBase)
using Server = ServerBase;

#endif  // end !USE_KIT_EVENT_QUEUE_ITC_MSG_ONLY

}  // end namespaces
}
#endif  // end header latch