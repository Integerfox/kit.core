#ifndef KIT_ITC_SELFPOSTMESSAGE_H_
#define KIT_ITC_SELFPOSTMESSAGE_H_
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
#include "Kit/Itc/IMessage.h"
#include "Kit/EventQueue/IQueue.h"


///
namespace Kit {
///
namespace Itc {

/** This template class implements a message that a component posts to its OWN
    event queue in order to yield execution back to its event loop and then be
    called back as soon as possible - without busy-waiting and without
    recursing on its own call stack.

    THIS IS DELIBERATELY *NOT* THE ITC CLIENT/SERVER REQUEST PATTERN.  There is
    no SAP, no payload, no 'request()' method and no ReturnHandler because
    there is no second thread, nothing to transfer and nothing to return to.
    The sender and the receiver are the same component in the same thread; the
    event queue is being used purely as a deferred-execution queue.  Use
    Kit::Itc::RequestMessage (or the SAP/OpenApi/CloseApi helpers) for genuine
    cross-thread client/server requests.

    The typical use case is a component that performs a long running sequence of
    steps in the thread that owns its event queue.  Instead of running every step
    in a single callback - which starves the thread's other messages, event
    flags and timers - the component performs one step, calls schedule(), and
    returns.

    Mechanics:
        o schedule() posts this message to the component's own event queue,
          i.e. the message is queued at the TAIL of the mailbox FIFO.
        o Kit::Itc::Mailbox::processMessages() dispatches at MOST one message
          per pass of the event loop, so the yield is fair with respect to
          other ITC traffic directed at the thread.
        o Because a message is pending, Kit::System::EventLoop skips its
          semaphore wait, so the callback occurs on the very next pass of the
          event loop instead of waiting for the thread's timing tick.
        o Event flags and software timers are serviced on that same pass, so
          neither is starved by a chain of continuations.

    Compared to Kit::System::Timer::start(0) - which also defers the callback,
    but only re-fires on the next timer tick - this mechanism resumes
    immediately.  Use a timer when the intent is to pace/throttle the work; use
    this class when the intent is purely to yield.

    THREADING INVARIANT: schedule(), cancel() and isPending() MUST be called
    ONLY from the thread that owns 'myQueue'.  Unlike ordinary cross-thread
    ITC use, the component retains ownership of this message while it is queued.
    This is safe ONLY because the sender and receiver are the same thread, so
    the thread itself serializes all access to m_pending.  Calling schedule()
    from any other thread races on m_pending and can double-post the message,
    violating the intrusive-container single-membership rule and causing a
    fatal error.

    LIFETIME: The message memory is owned by the component, and a scheduled
    message is referenced by the mailbox FIFO.  The component must therefore
    outlive any scheduled callback; in particular, it must not be destroyed
    while this message is queued.

    BOUNDED CHAINS: While a message is pending the event loop does not block on
    its semaphore, so a continuation chain spins the thread at 100%.  The chain
    MUST terminate, i.e. the component must eventually stop calling schedule().
    Use a timer for open ended polling.

    Template args:
        CONTEXT   Type of the class that implements the context for the
                  callback.

    Example:
    @code
        #include "Kit/Itc/OpenCloseSync.h"
        #include "Kit/Itc/SelfPostMessage.h"

        class MyComponent : public Kit::Itc::OpenCloseSync
        {
        public:
            MyComponent( Kit::EventQueue::IQueue& myQueue )
                : Kit::Itc::OpenCloseSync( myQueue )
                , m_resume( myQueue, *this, &MyComponent::nextStep )
                , m_step( 0 )
                , m_opened( false )
            {
            }

            void request( OpenMsg& msg ) noexcept override
            {
                if ( !m_opened )
                {
                    m_opened = true;
                    m_step   = 0;
                    m_resume.schedule();  // Will run on the next Event Loop iteration but AFTER the OpenMsg has been returned
                }
                msg.getPayload().success = true;
                msg.returnToSender();
            }

            void request( CloseMsg& msg ) noexcept override
            {
                if ( m_opened )
                {
                    m_opened = false;
                    m_resume.cancel();
                }
                msg.getPayload().success = true;
                msg.returnToSender();
            }

        protected:
            void nextStep() noexcept
            {
                doStep( m_step++ );
                if ( m_step < NUM_STEPS )
                {
                    m_resume.schedule();    // yield, then resume
                }
            }

            Kit::Itc::SelfPostMessage<MyComponent> m_resume;
            unsigned                               m_step;
            bool                                   m_opened;
        };
    @endcode
 */
template <class CONTEXT>
class SelfPostMessage : public IMessage
{
    /// Deleted copy/move constructors and assignment operators
    SelfPostMessage( const SelfPostMessage& ) = delete;
    SelfPostMessage& operator=( const SelfPostMessage& ) = delete;
    SelfPostMessage( SelfPostMessage&& ) = delete;
    SelfPostMessage& operator=( SelfPostMessage&& ) = delete;

public:
    /// Definition of the call-back method that resumes the component after the yield
    typedef void ( CONTEXT::*ResumeFunction_T )();

public:
    /// Constructor.  'myQueue' MUST be the event queue of the owning thread
    SelfPostMessage( Kit::EventQueue::IQueue& myQueue,
                     CONTEXT&                 context,
                     ResumeFunction_T         resumeCallbackFunc ) noexcept
        : m_queue( myQueue )
        , m_context( context )
        , m_callbackFuncPtr( resumeCallbackFunc )
        , m_pending( false )
        , m_cancelled( false )
    {
    }

public:
    /** Yields the processor back to the event loop and guarantees that the
        callback method is invoked on a subsequent pass of that loop.

        Calling schedule() more than once before the callback runs coalesces
        into a SINGLE callback - the message is only posted when it is not
        already in the mailbox.  This makes the call safe to issue
        speculatively, and it prevents a double-post that would violate the
        intrusive-container single-membership rule.

        Calling schedule() after cancel() - but before the cancelled message
        has drained - re-arms the already-queued message.
     */
    void schedule() noexcept
    {
        m_cancelled = false;
        if ( !m_pending )
        {
            m_pending = true;
            m_queue.post( *this );
        }
    }

    /** Cancels a pending callback, i.e. the callback method will NOT be
        called.  It is okay to call cancel() when nothing is scheduled.

        Note: A posted message cannot be pulled back out of the mailbox FIFO,
              so the message is still dispatched - it simply does nothing.
              isPending() therefore stays true until the event loop drains it.
     */
    void cancel() noexcept
    {
        m_cancelled = m_pending;
    }

    /// Returns true when the message is in the mailbox FIFO, i.e. not yet dispatched
    bool isPending() const noexcept
    {
        return m_pending;
    }

public:
    /// See Kit::Itc::IMessage
    void process() noexcept override
    {
        // Clear BEFORE the callback so that the callback can re-schedule me
        m_pending = false;
        if ( m_cancelled )
        {
            m_cancelled = false;
            return;
        }

        ( m_context.*m_callbackFuncPtr )();
    }

protected:
    /// The event queue I post myself to
    Kit::EventQueue::IQueue& m_queue;

    /// The component that owns me
    CONTEXT& m_context;

    /// The method that resumes the component
    ResumeFunction_T m_callbackFuncPtr;

    /// Set while I am in the mailbox FIFO
    bool m_pending;

    /// Set when a pending callback has been cancelled
    bool m_cancelled;
};

}  // end namespaces
}
#endif  // end header latch
