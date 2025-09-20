#ifndef Cpl_Itc_MailboxServer_h_
#define Cpl_Itc_MailboxServer_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/System/EventLoop.h"
#include "Cpl/Itc/Mailbox.h"


///
namespace Cpl {
///
namespace Itc {

/** This class collects the common functions of a generic server providing a
    mailbox and Event loop.

    The mailbox server extents the Cpl::System::EventLoop interface so it
    has support for EventFlags and Software Timers.

       1. Event Flags are processed.  Events are processed in LSb order.
       2. The timers and their callbacks (if any timers have expired) are
          processed.
       3. A Single ITC message (if one was received) is processed.

 */
class MailboxServer :
    public Mailbox,
    public Cpl::System::EventLoop
{
public:
    /** Constructor.  The argument 'timingTickInMsec' specifies the timing
        resolution that will be used for Cpl::Timer::Local Timers.
     */
    MailboxServer( unsigned long                       timingTickInMsec = OPTION_CPL_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
                   Cpl::System::SharedEventHandlerApi* eventHandler     = 0 ) noexcept;

protected:
    /// See Cpl::System::Runnable
    void appRun();

};

};      // end namespaces
};
#endif  // end header latch