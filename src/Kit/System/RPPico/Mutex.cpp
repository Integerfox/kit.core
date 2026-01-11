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
    recursive_mutex_enter_blocking( &( m_mutex.m_sdkMutex ) );
}

void Mutex::unlock( void )
{
    recursive_mutex_exit( &( m_mutex.m_sdkMutex ) );
}

}  // end namespace
}
//------------------------------------------------------------------------------
