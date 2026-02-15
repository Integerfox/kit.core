#ifndef KIT_DM_OBSERVER_H_
#define KIT_DM_OBSERVER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Dm/IObserver.h"
#include "Kit/Dm/IModelPoint.h"
#include <stdint.h>

// Forward reference to break dependency on the Event Queue namespace
namespace Kit {
namespace EventQueue {
class IChangeNotification;
}
}

///
namespace Kit {
///
namespace Dm {


/** This mostly concrete class defines the Observer interface - for change
    notifications - to a Model Points data/state
 */
class ObserverBase : public IObserver
{
public:
    /// Constructor
    ObserverBase( Kit::EventQueue::IChangeNotification& myEventQueue ) noexcept;

    /// Constructor (should only be used when creating an array of Observers)
    ObserverBase() noexcept;

    /// Sets the Event Loop (must be called before subscribing)
    void setEventQueue( Kit::EventQueue::IChangeNotification& myEventQueue );

public:
    /// See Kit::Dm::IObserver
    Kit::EventQueue::IChangeNotification* getIChangeNotification_() const noexcept override;

    /// See Kit::Dm::IObserver
    void setModelPoint_( IModelPoint* modelPoint ) noexcept override;

    /// See Kit::Dm::IObserver
    IModelPoint* getModelPoint_() noexcept override;

    /// See Kit::Dm::IObserver
    int getState_() const noexcept override;

    /// See Kit::Dm::IObserver
    void setState_( int newState ) noexcept override;

    /// See Kit::Dm::IObserver
    uint16_t getSequenceNumber_() const noexcept override;

    /// See Kit::Dm::IObserver
    void setSequenceNumber_( uint16_t newSeqNumber ) noexcept override;

protected:
    /// Pointer to the Model Point the instance is observing
    IModelPoint* m_point;

    /// Reference to observer's Event Queue server
    Kit::EventQueue::IChangeNotification* m_eventQueueHdl;

    /// Internal state of the observer.  Note: The state is actual managed by the Model Point
    int m_state;

    /// Sequence number of the observer
    uint16_t m_seqNumber;
};

/////////////////////////////////////////////////////////////////////////////
/** This template class defines a type safe Observer

    Template Arguments:
        MP      - The concrete Model Point Type
 */
template <class MP>
class Observer : public ObserverBase
{
public:
    /// Type safe change notification.  See Kit::Dm::IObserver
    virtual void modelPointChanged( MP& modelPointThatChanged, IObserver& clientObserver ) noexcept = 0;

public:
    /// Constructor
    Observer( Kit::EventQueue::IChangeNotification& myEventQueue )
        : ObserverBase( myEventQueue ) {}

    /// Default Constructor
    Observer()
        : ObserverBase() {}

protected:
    /// See Kit::Dm::IObserver
    void genericModelPointChanged_( IModelPoint& modelPointThatChanged, IObserver& clientObserver ) noexcept override { modelPointChanged( *( (MP*)&modelPointThatChanged ), clientObserver ); }
};


};  // end namespaces
};
#endif  // end header latch
