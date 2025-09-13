#ifndef KIT_SYSTEM_GLOBAL_LOCK_H_
#define KIT_SYSTEM_GLOBAL_LOCK_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


/// 
namespace Kit {
/// 
namespace System {


/** This class defines an interface for a "Global Lock".  A Global Lock provides
    mutual exclusion and/or a critical section protection similar to a Mutex
    except that is it intended to be lighter weight and/or have faster
    performance than a traditional Mutex.  However, the performance gain comes
    with the  following constraints:

        o Non recursive semantics.  The calling thread CANNOT attempt to acquire
          the lock a second time once it has already acquire the lock.

        o The code that is protected by this lock MUST BE VERY SHORT time wise
          and NOT call an operating system methods (e.g. any Kit::System methods).

    Why the above the constraints?  The GlobalLock interface is intended to
    be an abstraction for disable/enable all interrupts when running on a RTOS
    platform. So, use GlobalLocks with care and always honor the above constraints.
 */
class GlobalLock
{

public:
    /** This method is invoked prior to entering a critical section.  If another
        thread currently "owns" the lock, the current thread will "wait" until it
        can obtain ownership before proceeding.
     */
    void static begin();

    /** This method is invoke at the end of a critical section.  This call will
        release the ownership of the lock.
     */
    void static end();

};

}       // end namespaces
}
#endif  // end header latch

