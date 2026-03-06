/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "ChangeHandler.h"
#include "IObserver.h"
#include "IModelPoint.h"
#include "Kit/System/Assert.h"
#include "Kit/System/GlobalLock.h"


///
//------------------------------------------------------------------------------
namespace Kit {
namespace Dm {

///////////////////
void ChangeHandler::processChangeNotifications() noexcept
{
    // Get the next pending change notification
    Kit::System::GlobalLock::begin();
    IObserver* observerPtr = m_pendingMpNotifications.get();
    Kit::System::GlobalLock::end();

    // Execute - at MOST one - the change notification callback
    if ( observerPtr )
    {
        // Get the model point that changed
        IModelPoint* mpPtr = observerPtr->getModelPoint_();
        KIT_SYSTEM_ASSERT( mpPtr != nullptr );   // NOTE: getModelPoint_() is guaranteed to return a valid pointer, but just in case...
        IModelPoint& modelPoint = *mpPtr;

        // Update the subscriber's state
        modelPoint.processSubscriptionEvent_( *observerPtr, IModelPoint::eNOTIFYING );

        // Execute the callback
        observerPtr->genericModelPointChanged_( modelPoint, *observerPtr );

        // Update the subscriber's state
        modelPoint.processSubscriptionEvent_( *observerPtr, IModelPoint::eNOTIFY_COMPLETE );
    }
}

void ChangeHandler::addPendingChangingNotification( IObserver& observer, Kit::System::ISignable& myEventLoop ) noexcept
{
    // Add the notification to my list and send myself an Event to wake up the mailbox
    Kit::System::GlobalLock::begin();
    m_pendingMpNotifications.put( observer );
    Kit::System::GlobalLock::end();
    myEventLoop.signal();
}   

void ChangeHandler::removePendingChangingNotification( IObserver& observer ) noexcept
{
    // Remove the subscriber from the notification
    Kit::System::GlobalLock::begin();
    m_pendingMpNotifications.remove( observer );
    Kit::System::GlobalLock::end();
}

bool ChangeHandler::isPendingChangingNotifications() noexcept
{
    Kit::System::GlobalLock::begin();
    bool pending = m_pendingMpNotifications.first() != nullptr;
    Kit::System::GlobalLock::end();
    return pending;
}

} // end namespace
}
//------------------------------------------------------------------------------