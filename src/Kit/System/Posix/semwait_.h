#ifndef KIT_SYSTEM_POSIX_SEMWAIT_H_
#define KIT_SYSTEM_POSIX_SEMWAIT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file provides the timed-wait helpers shared by the POSIX Semaphore and
    BSemaphore implementations.  It is PRIVATE to the Kit::System POSIX port.
 */

#include <errno.h>
#include <semaphore.h>
#include <stdint.h>
#include <time.h>

/* sem_clockwait() is a glibc(>=2.30) extension that accepts a CLOCK_MONOTONIC
   deadline, i.e. a timed wait that is immune to wall-clock changes.  Plain
   sem_timedwait() is CLOCK_REALTIME only.

   NOTE: musl also provides sem_clockwait(), but does not define __GLIBC_PREREQ
         -->it falls back to the (still correct) CLOCK_REALTIME implementation.
 */
#ifdef __GLIBC_PREREQ
#if __GLIBC_PREREQ( 2, 30 )
#define HAVE_SEM_CLOCKWAIT_
#endif
#endif

///
namespace Kit {
///
namespace System {
///
namespace Posix {

/// Nanoseconds per second
constexpr long nsecPerSec_ = 1000000000L;

/// Milliseconds per second
constexpr long msecPerSec_ = 1000L;

/// Nanoseconds per millisecond
constexpr long nsecPerMsec_ = 1000000L;


// Note: The clock selection and the wait call MUST agree -->they are defined as a pair
#ifdef HAVE_SEM_CLOCKWAIT_
/// Clock used for the timed-wait deadline
constexpr clockid_t timedWaitClock_ = CLOCK_MONOTONIC;

/// Waits for the semaphore, or until the absolute 'deadline' expires
inline int semWaitUntil_( sem_t& sema, const struct timespec& deadline ) noexcept
{
    return sem_clockwait( &sema, timedWaitClock_, &deadline );
}
#else
/// Clock used for the timed-wait deadline
constexpr clockid_t timedWaitClock_ = CLOCK_REALTIME;

/// Waits for the semaphore, or until the absolute 'deadline' expires
inline int semWaitUntil_( sem_t& sema, const struct timespec& deadline ) noexcept
{
    return sem_timedwait( &sema, &deadline );
}
#endif


/** Waits up to 'timeoutMs' for the semaphore.  Returns true when the semaphore
    was acquired; false when the timeout expired.
 */
inline bool semTimedWait_( sem_t& sema, uint32_t timeoutMs ) noexcept
{
    // Convert the relative timeout to the absolute deadline required by POSIX
    struct timespec deadline;
    clock_gettime( timedWaitClock_, &deadline );
    deadline.tv_sec  += static_cast<time_t>( timeoutMs / msecPerSec_ );
    deadline.tv_nsec += static_cast<long>( ( timeoutMs % msecPerSec_ ) * nsecPerMsec_ );
    if ( deadline.tv_nsec >= nsecPerSec_ )
    {
        deadline.tv_nsec -= nsecPerSec_;
        deadline.tv_sec++;
    }

    // Note: an interrupted wait MUST be resumed against the same deadline - else
    //       the caller's timeout silently restarts on every signal delivery
    int result;
    do
    {
        result = semWaitUntil_( sema, deadline );
    }
    while ( result != 0 && errno == EINTR );

    return result == 0;
}

}  // end namespaces
}
}
#endif  // end header latch
