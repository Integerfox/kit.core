/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file contains default implementations of several methods in the Thread
    class.  You may override these implementations by not compiling/linking this
    file - and providing your own implementations in your OS-specific code
*/


#include "Thread.h"
#include "Kit/System/Shutdown.h"
#include "FatalError.h"
#include "Private.h"
#include "SimTick.h"
#include "Kit/Container/SList.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

static Kit::Container::SList<Thread> threadList_( "StaticConstructor" );


//////////////////////////
KitSystemThreadID_T Thread::getId() const noexcept
{
    return m_nativeThreadHdl;
}

bool Thread::isActive() const noexcept
{
    return threadList_.find( *this );
}

IRunnable& Thread::getRunnable() const noexcept
{
    return m_runnable;
}

Thread& Thread::getCurrent() noexcept
{
    Thread* curThread = tryGetCurrent();

    // Trap potential error
    if ( !curThread )
    {
        FatalError::logRaw( Shutdown::eOSAL, "Thread::getCurrent().  Current thread is NOT a Kit::System::Thread." );
    }

    return *curThread;
}

//////////////////////////
bool Thread::isActiveThread( Thread* threadPtr ) noexcept
{
    // NOTE: I have to walk the list of active threads because 'threadPtr' may or may not be valid Thread pointer
    Mutex::ScopeLock lock( PrivateLocks::sysLists() );
    Thread*          ptr = threadList_.first();
    while ( ptr )
    {
        if ( ptr == threadPtr )
        {
            return true;
        }
        ptr = threadList_.next( *ptr );
    }
    return false;
}

void Thread::addThreadToActiveList( Thread& thread ) noexcept
{
    Mutex::ScopeLock lock( PrivateLocks::sysLists() );
    threadList_.put( thread );
}

void Thread::removeThreadFromActiveList( Thread& thread ) noexcept
{
    Mutex::ScopeLock lock( PrivateLocks::sysLists() );
    threadList_.remove( thread );
}

void Thread::launchRunnable( Thread& threadHdl ) noexcept
{
    // Add to the list of active threads
    addThreadToActiveList( threadHdl );

    // Launch the IRunnable object
    auto& runnable = threadHdl.getRunnable();
    runnable.setThread( &threadHdl );
    KIT_SYSTEM_SIM_TICK_THREAD_INIT_( threadHdl.m_allowSimTicks );
    runnable.entry();
    runnable.setThread( nullptr );
    KIT_SYSTEM_SIM_TICK_ON_THREAD_EXIT_();

    // Remove the thread from the list of active threads
    removeThreadFromActiveList( threadHdl );
}

void Thread::traverse( Thread::ITraverser& client ) noexcept
{
    Mutex::ScopeLock lock( PrivateLocks::sysLists() );
    Thread*          t = threadList_.first();
    while ( t )
    {
        if ( client.item( *t ) == Kit::Type::TraverserStatus::eABORT )
        {
            break;
        }
        t = threadList_.next( *t );
    }
}

}  // end namespaces
}
//------------------------------------------------------------------------------