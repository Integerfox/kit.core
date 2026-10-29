/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Shutdown.h"
#include "Kit/Container/SList.h"
#include "Kit/System/Mutex.h"
#include "Private.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace System {

///
static Kit::Container::SList<Shutdown::IHandler> callbacks_( "invoke_special_static_constructor" );


///////////////////////
void Shutdown::registerHandler( Shutdown::IHandler& instanceToRegister ) noexcept
{
    Mutex::ScopeLock lock( PrivateLocks::system() );
    callbacks_.push( instanceToRegister );
}


//////////////////////
int Shutdown::notifyShutdownHandlers( int exitCode ) noexcept
{
    PrivateLocks::system().lock();
    Shutdown::IHandler* ptr = callbacks_.get();
    PrivateLocks::system().unlock();

    while ( ptr )
    {
        exitCode = ptr->notify( exitCode );

        PrivateLocks::system().lock();
        ptr = callbacks_.pop();
        PrivateLocks::system().unlock();
    }

    return exitCode;
}

}  // end namespaces
}
//------------------------------------------------------------------------------