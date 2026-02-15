#ifndef KIT_EVENT_QUEUE_ICHANGE_NOTIFICATION_H_
#define KIT_EVENT_QUEUE_ICHANGE_NOTIFICATION_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


/// Forward reference to break dependency on the Dm namespace
namespace Kit {
namespace Dm {
class IObserver;
}
}



///
namespace Kit {
///
namespace EventQueue {


/** This abstract class defines the interface to manage Data Model Change
    notification events that are published from Model Points.

    NOTE: The requirements and/or semantics of Model Point Observers are
          that subscriptions, notifications, and cancel-of-subscriptions
          all happen in a SINGLE thread and that thread is the 'Observers'
          thread.

    NOTE: Use of this interface is RESTRICTED to the Kit::Dm and Kit::EventQueue
          namespaces. The Application (or other namespaces) should NEVER use
          this interface
 */
class IChangeNotification
{
public:
    /** This method is used add a new 'change notification' to its list
        of pending change notifications.  Calling this method when the
        observer is already registered for change notification will cause
        a FATAL ERROR.

        This method IS thread safe.
     */
    virtual void addPendingChangingNotification( Kit::Dm::IObserver& observer ) noexcept = 0;

    /** This method is used remove a pending 'change notification' from its list
        of pending change notifications.  It is okay to call this method even if
        the observer is not current registered for change notifications.

        This method IS thread safe.
     */
    virtual void removePendingChangingNotification( Kit::Dm::IObserver& observer ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IChangeNotification() = default;
};


}  // end namespaces
}
#endif  // end header latch
