#ifndef Cpl_Dm_SubscriberApi_h_
#define Cpl_Dm_SubscriberApi_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/ListItem.h"
#include <stdint.h>

/// Forward reference to the Event Queue server -->used to avoid circular dependencies
namespace Kit {
namespace EventQueue {
class IChangeNotification;
}
}

///
namespace Kit {
///
namespace Dm {

/// Forward reference to a Model point -->used to avoid circular dependencies
class IModelPoint;

/** This abstract class defines the Observer interface - for change
    notifications - to a Model Points data/state
 */
class IObserver : public Kit::Container::ExtendedListItem
{
public:
    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Kit::Dm namespace.  The Application should
        NEVER call this method.

        This method is the client's callback function for a MP change
        notification.  This method is called in as part of the asynchronous
        notification mechanism of the observer's Event Queue server, i.e. executes
        in the thread associated m_eventQueueHdl
     */
    virtual void genericModelPointChanged_( IModelPoint& modelPointThatChanged,
                                            IObserver&   clientObserver ) noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Kit::Dm namespace.  The Application should
        NEVER call this method.

        This method returns a pointer to the Observer's Event Queue server
      */
    virtual Kit::EventQueue::IChangeNotification* getIChangeNotification_() const noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Kit::Dm namespace.  The Application should
        NEVER call this method.

        This method is use to set the Observer's Model Point reference
      */
    virtual void setModelPoint_( IModelPoint* modelPoint ) noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Kit::Dm namespace.  The Application should
        NEVER call this method.

        This method is use to get the Observer's Model Point reference.

        Note: If this method is called BEFORE the setModelPoint() method is
              called then a Fatal Error will be generated.
      */
    virtual IModelPoint* getModelPoint_() noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Kit::Dm namespace.  The Application should
        NEVER call this method.

        This method is use to get the Observer's internal state
      */
    virtual int getState_() const noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Kit::Dm namespace.  The Application should
        NEVER call this method.

        This method is use to set the Observer's internal state
      */
    virtual void setState_( int newState ) noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Kit::Dm namespace.  The Application should
        NEVER call this method.

        This method is use to get the Observer's sequence number
      */
    virtual uint16_t getSequenceNumber_() const noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Kit::Dm namespace.  The Application should
        NEVER call this method.

        This method is use to set the Observer's sequence number
      */
    virtual void setSequenceNumber_( uint16_t newSeqNumber ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IObserver() {}
};


}       // end namespaces
}
#endif  // end header latch
