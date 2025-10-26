/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/System/Mutex.h"
#include "Kit/System/Api.h"
#include "Kit/System/FatalError.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

//////////////////////////////////////////////////////////////////////////////
Mutex::Mutex()
{
    m_mutex = xSemaphoreCreateRecursiveMutex();
    if ( m_mutex == NULL )
    {
        FatalError::logf( Shutdown::eOSAL, "Kit::System::Mutex::Mutex().  Failed to create mutex" );
    }
}

Mutex::~Mutex()
{
    vSemaphoreDelete( m_mutex );
}

void Mutex::lock( void )
{
    // Do nothing if the scheduler has not yet been started, i.e, if there is only
    // one thread running -->then by definition I have mutual exclusion. It also allows
    // an application to lock a mutex BEFORE the scheduler has been started (i am looking
    // at you Trace engine).
    if ( isSchedulingEnabled() )
    {
        xSemaphoreTakeRecursive( m_mutex, portMAX_DELAY );
    }
}


void Mutex::unlock( void )
{
    if ( isSchedulingEnabled() )
    {
        xSemaphoreGiveRecursive( m_mutex );
    }
}

}  // end namespace
}
//------------------------------------------------------------------------------