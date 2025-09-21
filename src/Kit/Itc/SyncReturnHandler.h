#ifndef KIT_ITC_SYNC_RETURN_HANDLER_h_
#define KIT_ITC_SYNC_RETURN_HANDLER_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Itc/IReturnHandler.h"
#include "Kit/System/ISignable.h"
#include "Kit/System/Thread.h"

///
namespace Kit {
///
namespace Itc {

/** This class implements a synchronous ReturnHandler. The assumption is that
    the client sends a message to a server mailbox, and then blocks on the
    thread's semaphore. When the server thread invokes the rts() method, the
    thread/semaphore is signaled, and thus the client is notified that it has
    ownership of the message and that the server has completed processing the
    message.

    The expected typical usage is to created a SyncReturnHandler on the stack
    in the client's thread just before the mailbox.postSync() method is called.
 */
class SyncReturnHandler : public IReturnHandler
{
public:
    /** The constructor initializes the handler with a reference to the
        thread semaphore of the current thread.
     */
    SyncReturnHandler( Kit::System::ISignable& clientSemaphore = Kit::System::Thread::getCurrent() ) noexcept
        : m_waiter( clientSemaphore)
    {
    }


public:
    /// See ReturnHandler
    void rts() noexcept override
    {
        m_waiter.signal();
    }

protected:
    /** Reference to the client thread's thread semaphore which is used
        to notify the client that the server has completed processing
        the message.
     */
    Kit::System::ISignable& m_waiter;
};


}  // end namespaces
}
#endif  // end header latch
