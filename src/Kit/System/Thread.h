#ifndef KIT_SYSTEM_THREAD_H_
#define KIT_SYSTEM_THREAD_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "kit_map.h"
#include "Kit/System/IRunnable.h"
#include "Kit/System/ISignable.h"
#include "Kit/System/Tls.h"
#include "Kit/Type/TraverserStatus.h"
#include "Kit/Container/ListItem.h"
#include <stdlib.h>
#include <string.h>


/// Defer the definition of a thread ID (aka the native thread handle) to the application's 'platform'
#define KitSystemThreadID_T KitSystemThreadID_T_MAP


/// The highest possible thread priority
#define KIT_SYSTEM_THREAD_PRIORITY_HIGHEST KIT_SYSTEM_THREAD_PRIORITY_HIGHEST_MAP

/// The recommended/default priority for a thread
#define KIT_SYSTEM_THREAD_PRIORITY_NORMAL KIT_SYSTEM_THREAD_PRIORITY_NORMAL_MAP

/// The lowest possible thread priority
#define KIT_SYSTEM_THREAD_PRIORITY_LOWEST KIT_SYSTEM_THREAD_PRIORITY_LOWEST_MAP

/** This value can be 'added' to one of the above Priorities to increase (i.e.
    higher) the priority by one 'level. A priority can be increased multiple
    levels by adding this value multiple times.
 */
#define KIT_SYSTEM_THREAD_PRIORITY_RAISE KIT_SYSTEM_THREAD_PRIORITY_RAISE_MAP

/** This value can be 'added' to one of the above Priorities to decrease (i.e.
    lower) the priority by one 'level. A priority can be decreased multiple
    levels by adding this value multiple times.
 */
#define KIT_SYSTEM_THREAD_PRIORITY_LOWER KIT_SYSTEM_THREAD_PRIORITY_LOWER_MAP

/** WORK-AROUND for zero overhead when the target's Thread DOES NOT supporting
    Simulated Time
 */
#ifndef USE_KIT_SYSTEM_SIM_TICK
/// Allocate a member variable to hold the 'allow simulated tick' flag
#define KIT_SYSTEM_THREAD_ALLOC_SIM_TICK_FLAG()

/// Set the 'allow simulated tick' flag
#define KIT_SYSTEM_THREAD_SET_SIM_TICK_FLAG(v)
#else
#define KIT_SYSTEM_THREAD_ALLOC_SIM_TICK_FLAG() bool m_allowSimTicks
#define KIT_SYSTEM_THREAD_SET_SIM_TICK_FLAG(v) m_allowSimTicks = (v)
#endif

///
namespace Kit {
///
namespace System {


/** This mostly abstract class defines the operations that can be performed on
    a thread.

    NOTE: The thread contains a built in counting semaphore that is usable by
          the developer/application.  This semaphore can be used to assist in
          implementing inter-thread-communications and/or ISR-to-thread
          communications.

    NOTE: There are default implementations provided for several member and class
          methods. They are contained in the Thread.cpp file in this directory.
          If your Operating System port requires a different implementation, then
          omit compiling the Thread.cpp file (in this directory) and provided
          your own implementation.
 */
class Thread : public ISignable, public Kit::Container::ListItem
{
public:
    /// This method returns the name (null terminated string) of the current thread.
    virtual const char* getName() const noexcept = 0;

    /** This method returns the task's unique identifier.  The returned identifier
        IS the platform's native thread handle.

        NOTE: The returned value can NOT be used a handle/reference/pointer
              to a Thread object instance.
     */
    virtual KitSystemThreadID_T getId() const noexcept;

    /** This method returns the 'executing' state of the thread.  If the method
        returns false, the underlying thread has terminated (i.e. the IRunnable
        object's entry() method has completed) and then the Thread object/instance
        can be safely deleted using the destroy() method below.

        NOTE: Default implementation is provided in the Thread.cpp file.
     */
    virtual bool isActive() const noexcept;

    /** This method returns a reference to the thread's IRunnable object
        NOTE: Default implementation is provided in the Thread.cpp file.
     */
    virtual IRunnable& getRunnable() const noexcept;

    /// Virtual destructor.
    virtual ~Thread() noexcept = default;

public:
    /** This method returns a reference to the currently executing thread.
        NOTE: Default implementation is provided in the Thread.cpp file.
     */
    static Thread& getCurrent() noexcept;

    /** Special version of getCurrent().  getCurrent() will trigger a FATAL error
        when called from a NON CPL thread.  tryGetCurrent() will return a nullptr
        when it detects that the current thread is NOT a CPL thread.

        This method should ONLY be used when your application has mix of CPL and
        non-CPL threads, i.e. the default should be to use getCurrent().
     */
    static Thread* tryGetCurrent() noexcept;

    /** This method causes the CURRENT thread to wait until its 'thread
        semaphore' is signal/set.
     */
    static void wait() noexcept;

    /** This method is similar to Semaphore::tryWait(), except that it operates
        on the thread semaphore
     */
    static bool tryWait() noexcept;

    /** Same as wait(), except the call will return after 'timeoutInMsec' has
        expired without the thread being signaled.  The method return true if
        the Thread was signaled; else false is returned if the timeout period
        expired.
     */
    static bool timedWait( uint32_t timeoutInMsec ) noexcept;

    /** This method is the similar to the isActive() method, but it operates
        on the specified thread pointer.  If the 'threadPtr' is nullptr or
        invalid (i.e. not a 'real' Thread*), then the method will return false.

        NOTE: Default implementation is provided in the Thread.cpp file.
     */
    static bool isActiveThread( Thread* threadPtr ) noexcept;

public:
    /// Returns the name for the current thread
    static inline const char* myName() noexcept
    {
        return getCurrent().getName();
    }

    /// Returns the ID for the current thread
    static inline KitSystemThreadID_T myId() noexcept
    {
        return getCurrent().getId();
    }

    /// This method returns a reference to the current thread' runnable instance.
    static inline IRunnable& myRunnable() noexcept
    {
        return getCurrent().getRunnable();
    }


public:
    /** This abstract class defines the client interface for walking the list of
        threads, i.e. defines the callback method for when walking/traversing
        the list of active threads.
     */
    class ITraverser
    {
    public:
        /// Virtual Destructor
        virtual ~ITraverser() {}

    public:
        /** This method is called once for ever item in the "traversee's" list.
            The return code from the method is used by the traverser to continue
            the traversal (eCONTINUE), or abort the traversal (eABORT).
         */
        virtual Kit::Type::TraverserStatus item( Thread& nextThread ) noexcept = 0;
    };

    /** Internal Iterator that allows the Client to traverse the list
        active threads.

        NOTES:
            o There is difference between an "active" thread and a Thread object
              instance.  The underlying platform/OS determines if the a thread is
              "active", i.e. the thread is created and running.  The Thread object
              gets created BEFORE the thread is active and exists AFTER thread
              terminates/end.  Why the explanation, this method only includes
              "active" threads.  Threads that have terminated, but that their
              associated Thread object instance has not been delete/destroyed
              will NOT show up in the active list.
            o This call is mutex protected.  It will prevent new threads from
              being created/deleted until it has completed.
            o A C++ iterator was purposefully not used because access to the list
              of threads has to be mutex protected.  By using an
              custom-iterator-with-a-callback approach - the Thread class can
              enforce proper mutex/critical-section handling.

        NOTE: Default implementation is provided in the Thread.cpp file.
     */
    static void traverse( Thread::ITraverser& client ) noexcept;


public:
    /** This method creates a thread according to the specified parameters.  The
        thread/task begins execution immediately upon its creation.

        NOTE: Not all platform support all options, e.g. The C++11 multi threading
              support does not directly support thread priorities.


        @returns A pointer to the Thread created, OR 0 if the was an error.

        @param runnable       a runnable object which is started when the task is created
        @param name           a null terminated string that represent that thread's name
        @param priority       a priority ranging from highest priority (KIT_SYSTEM_THREAD_PRIORITY_HIGHEST)
                              to lowest priority (KIT_SYSTEM_THREAD_PRIORITY_LOWEST).
                              Priorities are defined in relative 'levels' to the max/normal/min
                              settings because there is no standardized priority settings
                              across Operating Systems.  If the priority value exceeds the
                              upper/lower bounds the platform's priority range, the thread's
                              priority will be clamped to the upper/lower bound of the priority
                              range.
        @param stackSize      Requested size, in bytes, of the stack for the thread.  A value of
                              of zero will size the thread's stack per the platform's default
                              stack size setting.
        @param stackPtr       Pointer to the stack memory to be used for the thread's stack. If
                              zero is passed, then the underlying platform/OS will allocate the
                              stack memory.
        @param allowSimTicks  With this argument is true the thread will be created to
                              use/support simulated time when 'Simulate ticks' are enabled.
                              A value false will force the thread to always be a 'real time'
                              thread.  See Cpl::System::SimTicks for more details about
                              simulated time.
     */
    static Thread* create( IRunnable&   runnable,
                           const char* name,
                           int         priority      = KIT_SYSTEM_THREAD_PRIORITY_NORMAL,
                           int         stackSize     = 0,
                           void*       stackPtr      = 0,
                           bool        allowSimTicks = true ) noexcept;


    /** This method is used to destroy a thread that was created by the Thread
        Factory. If you are going to delete a thread that was created by the
        Thread Factory, you MUST use the Factory's destroy method to delete it
        to properly insure that the memory used to create the task is released
        correctly.

        When the 'delayTimeMsToWaitIfNotStopped' argument is NOT zero, AND the
        thread is still active. This method will call pleaseStop() on the IRunnable
        object associated with the thread.  After the delay time (in milliseconds)
        has expired - will attempt to unconditionally terminate the thread.

        NOTES:

            o The application should only delete/destroy a thread AFTER the
              thread's IRunnable object.run() method has ended/terminated.  If
              the thread is destroy/delete before run() has completed, there is
              NO GUARANTEE with respect to whether or not the thread has
              released all acquired resources!
            o NOT ALL Platforms support brute-force termination of threads,
              e.g. in C++11 there is no method/interface to forcibly terminate
              a thread.  Once again -->it is a HIGHLY DISCOURAGED to design
              your application where you need to forcibly terminate threads.
            o This method only deletes/destroys the Thread instance -- it does
              NOT delete/destroy the associated IRunnable instance.
     */
    static void destroy( Thread& threadToDestroy, uint32_t delayTimeMsToWaitIfActive = 0 ) noexcept;

protected:
    /// Constructor
    Thread( IRunnable& runnable ) noexcept
        : m_runnable( runnable )
    {
        memset( m_tlsArray, 0, sizeof( m_tlsArray ) );
    }
    

    /** Helper method that should be called from the native's entry function.
        This method is responsible for various action such as maintaining
        the list of active threads, launching the IRunnable object, and supporting
        simulated time for thread (but only when SIM-TIME is enabled).

        The method returns when the IRunnable object's entry() method
        has completed.
        NOTE: Default implementation is provided in the Thread.cpp file.
     */
    static void launchRunnable( Thread& threadHdl ) noexcept;

    /**  Helper method to safe add to the active thread list
         NOTE: Default implementation is provided in the Thread.cpp file.
     */
    static void addThreadToActiveList( Thread& thread ) noexcept;

    /** Helper method to safely remove from the active thread list
        NOTE: Default implementation is provided in the Thread.cpp file.
     */
    static void removeThreadFromActiveList( Thread& thread ) noexcept;

protected:
    /// Reference to the thread's runnable object
    IRunnable& m_runnable;

    /// Native file handle for the thread instance
    KitSystemThreadID_T m_nativeThreadHdl;

    /// Thread Local storage
    void* m_tlsArray[OPTION_KIT_SYSTEM_TLS_DESIRED_MIN_INDEXES];

    /// Optional: Flag to allow simulated ticks for this thread
    KIT_SYSTEM_THREAD_ALLOC_SIM_TICK_FLAG();

    /// Allow access to the TLS array
    friend class Tls;
};

}  // end namespaces
}
#endif  // end header latch