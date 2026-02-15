#ifndef Cpl_Dm_EventLoop_h_
#define Cpl_Dm_EventLoop_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/ISignable.h"
#include "Kit/Container/DList.h"
#include "Kit/Dm/IObserver.h"


///
namespace Kit {
///
namespace Dm {

/** This concrete class provides the implementation for processing change notifications
    as well as managing change notification subscriptions.  It is intended to be
    used by the Event Queue server


 */
class ChangeHandler
{
public:
    /// Constructor
    ChangeHandler() noexcept = default;

public:
    /** This method is used add a new 'change notification' to its list
        of pending change notifications.  Calling this method when the
        observer is already registered for change notification will cause
        a FATAL ERROR.

        This method IS thread safe.

        NOTE: The requirements and/or semantics of Model Point Observers are
              that Subscriptions, Notifications, and Cancel-of-Subscriptions
              all happen in a SINGLE thread and that thread is the Observers'
              thread.
     */
    void addPendingChangingNotification( IObserver&              observer,
                                         Kit::System::ISignable& myEventLoop ) noexcept;

    /** This method is used remove a pending 'change notification' from its list
        of pending change notifications.  It is okay to call this method even if
        the Observer is not current registered for change notifications.

        This method IS thread safe.

        NOTE: The requirements and/or semantics of Model Point Observers are
              that Subscriptions, Notifications, and Cancel-of-Subscriptions
              all happen in a SINGLE thread and that thread is the Observers'
              thread.
     */
    void removePendingChangingNotification( IObserver& observer ) noexcept;


public:
    /** This method returns true if there is at least one pending change
        notification.

        This method IS thread safe.
     */
    bool isPendingPendingChangingNotifications() noexcept;

    /** This method processes pending change notifications

        This method IS thread safe.
     */
    void processChangeNotifications() noexcept;

protected:
    /// List of pending Model Point Change Notifications
    Kit::Container::DList<IObserver> m_pendingMpNotifications;
};

}  // end namespaces
}
#endif  // end header latch