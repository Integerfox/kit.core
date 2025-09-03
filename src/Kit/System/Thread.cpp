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

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

//////////////////////////
KitSystemThreadID_T Thread::getId() const noexcept
{
    return m_threadHandle;
}

bool Thread::isRunning( void ) const noexcept
{
    return false;
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
void** Thread::getTlsArray() noexcept
{
    static void* tlsArray[OPTION_KIT_SYSTEM_TLS_DESIRED_MIN_INDEXES];
    return tlsArray;
}

static void traverse( Thread::Traverser& client ) noexcept;


}  // end namespaces
}
//------------------------------------------------------------------------------