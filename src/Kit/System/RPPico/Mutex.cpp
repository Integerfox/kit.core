/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/System/Mutex.h"

//////////////////////////////////////////////////////////////////////////////
Cpl::System::Mutex::Mutex()
{
    // Nothing needed.  The initialization of the mutex is done when the Cpl C++ library is initialized
}

Cpl::System::Mutex::~Mutex()
{
    // Nothing needed
}

void Cpl::System::Mutex::lock( void )
{
    if ( m_mutex.m_rp2040Mutex )
    {
        recursive_mutex_enter_blocking( m_mutex.m_rp2040Mutex );
    }
}

void Cpl::System::Mutex::unlock( void )
{
    if ( m_mutex.m_rp2040Mutex )
    {
        recursive_mutex_exit( m_mutex.m_rp2040Mutex );
    }
}


