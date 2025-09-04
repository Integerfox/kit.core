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
#include "Tls.h"
#include "FatalError.h"
#include "Private.h"
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

bool Thread::isActive( void ) const noexcept
{
    return threadList_.find( *this );
}

Runnable& Thread::getRunnable( void ) const noexcept
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
    Thread* ptr = threadList_.first();
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

void** Thread::getTlsArray() noexcept
{
    static void* tlsArray[OPTION_KIT_SYSTEM_TLS_DESIRED_MIN_INDEXES];
    return tlsArray;
}

 void addThreadToActiveList( Thread& thread )
{
    Mutex::ScopeLock lock( PrivateLocks::sysLists() );
    threadList_.put( thread );
}

 void removeThreadFromActiveList( Thread& thread )
{
    Mutex::ScopeLock lock( PrivateLocks::sysLists() );
    threadList_.remove( thread );
}

void Thread::launchRunnable( Thread& threadHdl ) noexcept
{
    // Add to the list of active threads
    addThreadToActiveList( threadHdl );

    // Launch the Runnable object
    threadHdl.m_runnable.setThread( &threadHdl );
    // KIT_SYSTEM_SIM_TICK_THREAD_INIT_( threadHdl.m_allowSimTicks );    // TODO: Add SimTime support
    threadHdl.m_runnable.entry();
    // KIT_SYSTEM_SIM_TICK_ON_THREAD_EXIT_();

    // Remove the thread from the list of active threads
    removeThreadFromActiveList( threadHdl );
}

void traverse( Thread::Traverser& client ) noexcept
{
    Mutex::ScopeLock lock( PrivateLocks::sysLists() );
    Thread*          t = threadList_.first();
    while ( t )
    {
        if ( client.item( *t ) == Kit::Type::Traverser::eABORT )
        {
            break;
        }
        t = threadList_.next( *t );
    }
}

}  // end namespaces
}
//------------------------------------------------------------------------------