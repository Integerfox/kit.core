/*-----------------------------------------------------------------------------
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



//------------------------------------------------------------------------------
namespace Kit {
namespace System {

///
static Kit::Container::SList<Shutdown::Handler> callbacks_( "invoke_special_static_constructor" );


///////////////////////
void registerHandler( Shutdown::Handler& instanceToRegister ) noexcept
{
    // TODO: FIX ME
    // Locks_::system().lock();
    callbacks_.push( instanceToRegister );
    // Locks_::system().unlock();
}


//////////////////////
int Shutdown::notifyShutdownHandlers( int exitCode ) noexcept
{
    // TODO: FIX ME
    // Locks_::system().lock();
    Shutdown::Handler* ptr = callbacks_.get();
    // Locks_::system().unlock();

    while ( ptr )
    {
        exitCode = ptr->notify( exitCode );

        // TODO: FIX ME
        // Locks_::system().lock();
        ptr = callbacks_.pop();
        // Locks_::system().unlock();
    }

    return exitCode;
}

} // end namespaces
}
//------------------------------------------------------------------------------