/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "ListenerRunnable.h"
#include "Kit/System/Thread.h"  // Needed for the signal method because IRunnable class only has forward reference to the Thread class
#include "Kit/System/Api.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Io {
namespace Socket {

///////////////////////////////
ListenerRunnable::ListenerRunnable() noexcept
    : m_clientPtr( 0 )
    , m_startCalled( false )
{
}


///////////////////////////////
void ListenerRunnable::startListening( IListenerClient& client, int portNumToListenOn ) noexcept
{
    // Critical section because the caller can be in a different thread
    // NOTE: Purposefully exiting the critical section before signaling the thread
    //       to avoid a potential deadlock
    m_lock.lock();

    if ( !m_startCalled )
    {
        m_startCalled = true;
        m_port        = portNumToListenOn;
        m_clientPtr   = &client;
        m_lock.unlock();
        m_parentThreadPtr_->signal();
    }
    else
    {
        m_lock.unlock();
    }
}

void ListenerRunnable::terminate() noexcept
{
    // Critical section because the caller can be in a different thread
    m_lock.lock();
    bool started  = m_startCalled;
    m_startCalled = false;
    m_lock.unlock();

    // Do nothing if the listener was never started (or terminate has already been called)
    if ( started )
    {
        stopListener();
    }
}


////////////////////////////////////////
void ListenerRunnable::entry() noexcept
{
    // Critical section because the m_startCalled flag can be modified by another thread
    m_lock.lock();
    bool started = m_startCalled;
    m_lock.unlock();

    // Block here until the startListening() method is called
    if ( !started )
    {
        Kit::System::Thread::wait();
    }

    // Listen for incoming connection requests.  Only returns (if then) after terminate() is called
    listen();
}

void ListenerRunnable::pleaseStop() noexcept
{
    terminate();
}

}  // end namespace
}
}
//------------------------------------------------------------------------------
