#ifndef KIT_SYSTEM_BSEMAPHORE_H_
#define KIT_SYSTEM_BSEMAPHORE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_map.h"
#include "Kit/System/ISignable.h"
#include <stdint.h>


/// Defer the definition of the a raw binary semaphore type to the application's 'platform'
#define KitSystemBSema_T          KitSystemBSema_T_MAP


///
namespace Kit {
///
namespace System {

/** This concrete class defines the interface for a Binary Semaphore with
    non-Priority Inheritance semantics.

    NOTES:

        o Unlike a Counting Semaphore, the signaled state saturates at 'one',
          i.e. redundant signal() calls are silently discarded.
        o Most RTOSes do not provide PI for semaphores, however that is not true
          for ALL RTOSes.  It is the responsibility of the platform to ensure
          that the non-PI semantics are honored.
        o The class inherits from 'ISignable' which contains/defines the
          "signal()" methods.
 */
class BSemaphore : public ISignable
{
public:
    /** Constructor.  The semaphore is created with a count of 'one' when
        `initialStateIsOne` is set to true.  A count of zero, will cause an
        immediate call to 'wait()' to block.
     */
    BSemaphore( bool initialStateIsOne = false ) noexcept;

    /// Destructor
    ~BSemaphore() noexcept;


public:
    /** This method causes the current thread to block and wait till the
        semaphore is signaled/set.  NOTE: Can only be called from a Thread
        context!
     */
    void wait() noexcept;

    /** This method is the same as wait(), except that 'timeoutMs' specifies the
        maximum amount of time, in milliseconds, will block if the semaphore
        count is zero.  The method return true if the semaphore was signaled
        (i.e. count > 0); else false is returned if the timeout period
        expired.
     */
    bool timedWait( uint32_t timeoutMs ) noexcept;

    /** This method is the same as wait(), except that if the semaphore
        count is zero, the method returns immediately and has a return code
        of false.  If the semaphore count is greater than zero, the count
        is decrement and the method returns immediately and has a return code
        of true.
     */
    bool tryWait() noexcept;


public:
    /// ISignable API
    int signal() noexcept override;

    /// ISignable API
    int su_signal() noexcept override;


protected:
    /// Raw Semaphore handle/instance/pointer
    KitSystemBSema_T m_sema;


protected:
    /// Helper method for supporting SimTicks and 'real' tick in the same build
    void waitInRealTime() noexcept;

    /// Helper method for supporting SimTicks and 'real' tick in the same build
    bool timedWaitInRealTime( uint32_t timeoutMs ) noexcept;

private:
    /// Prevent access to the copy constructor -->semaphores can not be copied!
    BSemaphore( const BSemaphore& m ) = delete;

    /// Prevent access to the assignment operator -->semaphores can not be copied!
    const BSemaphore& operator=( const BSemaphore& m ) = delete;

    /// Prevent access to the move constructor -->semaphores can not be implicitly moved!
    BSemaphore( BSemaphore&& m ) = delete;

    /// Prevent access to the move assignment operator -->semaphores can not be implicitly moved!
    BSemaphore& operator=( BSemaphore&& m ) = delete;


    /// Friends(s)
    friend class SimTick;
};


}  // end namespaces
}
#endif  // end header latch
