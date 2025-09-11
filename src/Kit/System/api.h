#ifndef KIT_SYSTEM_API_H_
#define KIT_SYSTEM_API_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file 

    This file defines methods for initializing the KIT C++ class library
    and other startup/init actions.

    NOTE: The KIT C++ class library MUST BE INITIALIZED before calling
          any other methods/class/functions in the Kit namespace!
*/

#include <stdint.h>

///
namespace Kit {
///
namespace System {


/*----------------------------------------------------------------------------*/
/** This function initializes the KIT C++ class library.  This function should
    be called as soon possible on start-up/boot-up of the application.

    NOTES:

        o This method MUST be called BEFORE any other methods in the
          'Kit' namespace is called! The only exception to this rule
          is for internal KIT Library use only.
        o This method should only be called ONCE!
 */
void initialize() noexcept;


/** This function is used to initiate thread scheduling.  The semantics of
    this call is that thread scheduling is guarantied to occur AFTER this
    call.  However thread scheduling could have already begun/started PRIOR
    to this call. The actual behavior is platform specific.

    NOTE: This function MAY or MAY NOT return.  The specific behavior is
          platform specific.  It is the application's responsible to properly
          handle the function's 'return behavior'
 */
void enableScheduling() noexcept;

/** This function returns true if scheduling has enabled using the
    enableScheduling() functions.  When this function returns true one or
    more threads are actively running, i.e. the application CANNOT assume
    a single threaded model.
 */
bool isSchedulingEnabled() noexcept;


/*----------------------------------------------------------------------------*/
/** This method causes the current thread to be suspended for
    n milliseconds.
 */
 void sleep( uint32_t milliseconds ) noexcept;

/** This method is the same as sleep(), EXCEPT that is guaranteed to
    suspend in 'real time'.  Typically an application NEVER needs
    to call this method.  See the SimTick interface for additional
    details about Simulated Time.
 */
 void sleepInRealTime( uint32_t milliseconds ) noexcept;

/*----------------------------------------------------------------------------*/
/** This method is used to temporarily suspend thread scheduling, i.e. no
    thread context switch will occur until resumeScheduling() is called.
    For every call to suspendScheduling() there must be a matching call
    to resumeScheduling().  Whether or not the calls to resumeScheduling()
    can be nested is platform specific.

    NOTES:
        o The application MUST be very careful when using this method, i.e.
          it should NOT make an Kit::System calls when scheduling has
          been suspended.
        o The time between suspending and resuming scheduling should be kept
          as SMALL AS POSSIBLE.
        o The details of what happens is VERY platform specific, i.e. Windows
          and POSIX do not natively provide this functionality.  Typically,
          this type of semantic only has meaning/is-useful when using an RTOS.
          OR said another - use of the suspend|resume methods are NOT
          portable with respect to guaranteed behavior across platforms.
 */
void suspendScheduling() noexcept;

/** This method is used to resume thread scheduling after a call to
    suspendScheduling has been made.
 */
void resumeScheduling() noexcept;



}       // end namespaces
}       // end namespace
#endif  // end header latch
