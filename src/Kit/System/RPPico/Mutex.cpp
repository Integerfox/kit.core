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

//////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------
namespace Kit {
namespace System {

extern bool g_kitInitialized;

Mutex::Mutex()
{
    // Note: Mutexes created (including static allocated instances) before
    // the Kit C++ library is initialized are initialized as part of the KIT
    // library's initialization process
    if ( g_kitInitialized )
    {
        recursive_mutex_init( &( m_mutex.m_sdkMutex ) );
    }
}

Mutex::~Mutex()
{
    // Nothing needed
}

void Mutex::lock( void )
{
    // Do nothing if the scheduler has not yet been started, i.e, if there is only
    // one thread running -->then by definition I have mutual exclusion. It also allows
    // an application to lock a mutex BEFORE the scheduler has been started (i am looking
    // at you Trace engine).
    if ( isSchedulingEnabled() )
    {
        recursive_mutex_enter_blocking( &( m_mutex.m_sdkMutex ) );
    }
}

void Mutex::unlock( void )
{
    if ( isSchedulingEnabled() )
    {
        recursive_mutex_exit( &( m_mutex.m_sdkMutex ) );
    }
}

}  // end namespace
}
//------------------------------------------------------------------------------
