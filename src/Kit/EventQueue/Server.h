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
#include "Kit/Itc/Mailbox.h"


///
namespace Kit {
///
namespace EventQueue {

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
            Kit::Container::SList<Kit::System::IEventFlag>* eventFlagsList      = nullptr ) noexcept;

protected:
    /// See Kit::System::IRunnable
    void entry() noexcept override;
};

}  // end namespaces
}
#endif  // end header latch