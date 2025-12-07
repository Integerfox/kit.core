/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Thread.h"
#include "Kit/System/Assert.h"
#include "Kit/System/FatalError.h"
#include <new>

#define SECT_ "Kit::System::BareMetal::Thread"

static Kit::System::Thread* theOneAndOnlyThreadPtr_ = nullptr;

//------------------------------------------------------------------------------
namespace Kit {
namespace System {
namespace BareMetal {


////////////////////////////////////
Thread::Thread( Kit::System::IRunnable& applicationRunnable, const char* name ) noexcept
    : Kit::System::Thread( applicationRunnable )
    , m_name( name )
{
}


Thread::~Thread() noexcept
{
    // Nothing needed
}

/////////////////////////////
int Thread::signal() noexcept
{
    return m_syncSema.signal();
}
int Thread::su_signal() noexcept
{
    return m_syncSema.su_signal();
}

const char* Thread::getName() const noexcept
{
    return m_name;
}

void Thread::entryPoint_() noexcept
{
    KIT_SYSTEM_ASSERT( theOneAndOnlyThreadPtr_ != nullptr );

    // Go Execute the "IRunnable" object
    launchRunnable( *theOneAndOnlyThreadPtr_ );
}

}  // end namespace
}
}

//------------------------------------------------------------------------------
namespace Kit {
namespace System {


//////////////////////////////
Kit::System::Thread* Kit::System::Thread::tryGetCurrent() noexcept
{
    return theOneAndOnlyThreadPtr_;
}

void Kit::System::Thread::wait() noexcept
{
    KIT_SYSTEM_ASSERT( theOneAndOnlyThreadPtr_ != nullptr );
    ( static_cast<Kit::System::BareMetal::Thread*>( &getCurrent() ) )->m_syncSema.wait();
}

bool Kit::System::Thread::tryWait() noexcept
{
    KIT_SYSTEM_ASSERT( theOneAndOnlyThreadPtr_ != nullptr );
    return ( static_cast<Kit::System::BareMetal::Thread*>( &getCurrent() ) )->m_syncSema.tryWait();
}

bool Kit::System::Thread::timedWait( uint32_t timeout ) noexcept
{
    KIT_SYSTEM_ASSERT( theOneAndOnlyThreadPtr_ != nullptr );
    return ( static_cast<Kit::System::BareMetal::Thread*>( &getCurrent() ) )->m_syncSema.timedWait( timeout );
}


//////////////////////////////
Kit::System::Thread* Kit::System::Thread::create( IRunnable&  runnable,
                                                  const char* name,
                                                  int         priority,
                                                  int         stackSize,
                                                  void*       stackPtr,
                                                  bool        allowSimTicks ) noexcept
{
    if ( theOneAndOnlyThreadPtr_ != nullptr )
    {   
        FatalError::logf( Shutdown::eOSAL, "Kit::System::Thread::create().  Only one BareMetal thread is allowed." );
        return nullptr;
    }
    theOneAndOnlyThreadPtr_ = new ( std::nothrow ) Kit::System::BareMetal::Thread( runnable, name );
    return theOneAndOnlyThreadPtr_;
}


void Kit::System::Thread::destroy( Thread& threadToDestroy, uint32_t delayTimeMsToWaitIfActive ) noexcept
{
    delete &threadToDestroy;
    theOneAndOnlyThreadPtr_ = nullptr;
}


}  // end namespace
}
//------------------------------------------------------------------------------