#ifndef KIT_SYSTEM_TIMER_MANAGER_H_
#define KIT_SYSTEM_TIMER_MANAGER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/ICounter.h"
#include "Kit/Container/DList.h"

///
namespace Kit {
///
namespace System {

/** This concrete class implements manages a list of Software Timers. When an
    individual timer's Count object reaches zero, its callback method is invoked
    and it is remove from the Active List.  The Active List is designed to be
    very efficient with respect to decrement counts when there are many Timers
    in the list, i.e. the amount of time to decrement the individual Timers is
    NOT a function of the number of active Timers.

    The Timer Manager requires that the Timer Manager instances, all Timer
    instances, and the Timer's Context (i.e. the code that executes the
    timer expired callbacks) all execute in the SAME thread.
 */
class TimerManager
{
public:
    /// Constructor
    TimerManager();

public:
    /** This method starts the Timer Manager.  This method should be called
        only once just before the Main/Event loop processing begins.

        This method must be called from the same thread that the Timer Manager
        executes in.
     */
    virtual void startManager( void ) noexcept;

    /** This method processes the current active timer lists.  For each timer
        that has expired, the timer's context callback method is called.
     */
    virtual void processTimers( void ) noexcept;

    /// Returns true if there is at least one active timer
    virtual bool areActiveTimers( void ) const noexcept;


public:
    /// Register for notification
    virtual void attach( ICounter& clientToCallback ) noexcept;

    /** Unregister for notification.  Returns true if the client was currently
        attached/registered; else false is returned.
     */
    virtual bool detach( ICounter& clientToCallback ) noexcept;

    /// This method converts the requested duration in milliseconds to counter ticks.
    virtual uint32_t msecToCounts( uint32_t durationInMsecs ) const noexcept;


protected:
    /// Helper method.
    virtual void addToActiveList( ICounter& clientToCallback ) noexcept;

    /** This method is intended to be call by a the timing source and each
        call to this method represents that one tick has expired, i.e. decrement
        the active Counter objects' by one.
     */
    virtual void tick( uint32_t milliseconds = 1 ) noexcept;

    /** This method is used by the Tick source to information the Timer Manager
        that there are no more ticks for the timing source's current tick cycle
     */
    virtual void tickComplete( void ) noexcept;

protected:
    /// Allocate first list (the list pointers are swapped to avoid moving nodes)
    Kit::Container::DList<ICounter> m_listA;

    /// Allocate second list (the list pointers are swapped to avoid moving nodes)
    Kit::Container::DList<ICounter> m_listB;

    /// List of active counters. Points to a physical list.
    Kit::Container::DList<ICounter>* m_counters;

    /// List of Pending-to-attach counters (this happens when timers attach from the timer-expired-callbacks). Points to a physical list.
    Kit::Container::DList<ICounter>* m_pendingAttach;

    /// Elapsed time of the previous processing cycle
    uint32_t m_timeMark;

    /// Elapsed time of the current processing cycle
    uint32_t m_timeNow;

    /// Flag to tracks when I am actively processing/consuming ticks
    bool m_inTickCall;
};

}  // end namespaces
}
#endif  // end header latch
