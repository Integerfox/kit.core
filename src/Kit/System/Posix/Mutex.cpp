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
Kit::System::Mutex::Mutex() noexcept
{
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init( &mutex_attr );
    pthread_mutexattr_settype( &mutex_attr, PTHREAD_MUTEX_RECURSIVE );
    pthread_mutex_init( &m_mutex, &mutex_attr );
}

Kit::System::Mutex::~Mutex() noexcept
{
    // Guarantee that the mutex is unlocked before it is "destroyed"
    pthread_mutex_unlock( &m_mutex );
    pthread_mutex_destroy( &m_mutex );
}

void Kit::System::Mutex::lock() noexcept
{
    pthread_mutex_lock( &m_mutex );
}


void Kit::System::Mutex::unlock() noexcept
{
    pthread_mutex_unlock( &m_mutex );
}
