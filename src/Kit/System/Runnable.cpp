/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Runnable.h"
#include "Private.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace System {


/////////////////////////////////////////////////////////////
void Runnable::entry( void ) noexcept
{
    // Set my running state
    PrivateLocks::system().lock();
    m_running = true;
    PrivateLocks::system().unlock();

    // Run my application code
    run();

    // Clear my running state
    PrivateLocks::system().lock();
    m_running = false;
    PrivateLocks::system().unlock();
}

bool Runnable::isRunning( void ) const noexcept
{
    Kit::System::Mutex::ScopeLock lock( PrivateLocks::system() );
    return m_running;
}

}  // end namespaces
}
//------------------------------------------------------------------------------