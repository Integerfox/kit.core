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

Mutex::Mutex()
{
    // Nothing needed.  The initialization of the mutex is done when the Kit C++ library is initialized
}

Mutex::~Mutex()
{
    // Nothing needed
}

void Mutex::lock( void )
{
    if ( m_mutex.m_sdkMutex )
    {
        recursive_mutex_enter_blocking( m_mutex.m_sdkMutex );
    }
}

void Mutex::unlock( void )
{
    if ( m_mutex.m_sdkMutex )
    {
        recursive_mutex_exit( m_mutex.m_sdkMutex );
    }
}

}  // end namespace
}
//------------------------------------------------------------------------------
