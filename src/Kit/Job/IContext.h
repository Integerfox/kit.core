#ifndef KIT_JOB_ICONTEXT_H_
#define KIT_JOB_ICONTEXT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/EventQueue/IQueue.h"
#include "Kit/Text/IString.h"

///
namespace Kit {
///
namespace Job {


/** This Package scoped abstract class defines a "Context" for a IJob. The
    Context provide common infrastructure, information, buffers, etc. that
    facilitates interaction between a IManager and individual IJob instances.
    The application SHOULD NEVER directly access this interface.

    This interface is NOT thread safe and assumes it executes in the same thread
    as the client IJob instance and its containing IManager.
*/
class IContext
{
    public:
    /** This method returns a reference to the IQueue associated with the thread
        that the IJob instance executes in.
     */
    virtual Kit::EventQueue::IQueue& getEventQueue() noexcept = 0;

public:
    /** This method returns a 'working' buffer for use by a command.  The buffer
        is only 'valid' for a single event loop processing, i.e. while the
        IJob instance is executing (e.g. during the SW Timer expired() callback).
        In between events - the buffer will be overwritten.
    */
    virtual Kit::Text::IString& getWorkBuffer0() noexcept = 0;

    /** Same as getWorkBuffer0(), except a second buffer instance.
        NOTE: The buffer is the same size as getWorkBuffer0().
    */
    virtual Kit::Text::IString& getWorkBuffer1() noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IContext() = default;
};


}  // end namespaces
}
#endif  // end header latch
