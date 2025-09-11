#ifndef KIT_SYSTEM_RUNNABLE_H_
#define KIT_SYSTEM_RUNNABLE_H_
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


/// Forward class reference to avoid a circular dependency.
class Thread;


/** This mostly an abstract class defines the interface for an object that is 
    "executed" when a Thread object is created.
 */
class IRunnable
{
public:
    /** This method is called when the Thread is started.  If this function 
        returns, the Thread will be terminated. A child class is responsible for
        implementing the entry() method. 
     */
    virtual void entry() noexcept = 0;

    /** This method is a request to have the runnable object terminate itself.
        This is only a request, it is not a requirement of a runnable object to
        oblige the request.  Some runnable objects are just not 'built' to
        handle a such a request.  On the other hand, a runnable object such as a
        EventServer is, and should make every attempt to comply with the request.

        NOTES:

            1) This a low-level terminate request in that it does not understand
               the "semantics" of what the thread is currently doing.  It simply
               attempts to exit the entry() method.  The system must first shutdown
               the "application usage" of the thread BEFORE calling this method.
            2) This method is an attempted work-around for terminating threads,
               by having the thread run itself to completion. Not all OSes
               support a polite way (i.e. reclaiming resource, memory, etc.) of
               killing threads.
     */
    virtual void pleaseStop() noexcept {}

    /** This method returns a pointer to the Thread object that is executing 
        the IRunnable instance.  The pointer will be nullptr if the thread has
        not started execution (i.e. before entry() is called) AND after the 
        thread has terminated (i.e. after entry() returns)
     */
    virtual Thread* getThread() noexcept
    {
        return m_parentThreadPtr_;
    }

public:
    /// Virtual destructor
    virtual ~IRunnable() noexcept = default;

protected:
    /// Constructor
    IRunnable() noexcept
        : m_parentThreadPtr_( nullptr )
    {
    }

protected:
    /// Allow the Thread access to the IRunnable object
    friend class Thread;

    /** This method allows the Thread class to provide the IRunnable object a
        reference to the thread it is executing in.
        
        NOTE: This method is intended for internal use by the Thread class ONLY.
    */
    virtual void setThread( Thread* thread ) noexcept
    {
        m_parentThreadPtr_ = thread;
    }

protected:
    /// A back-reference to the thread that the runnable object is executing in
    Thread* m_parentThreadPtr_;
};

}       // end namespaces
}
#endif  // end header latch
