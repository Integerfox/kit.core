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
Kit::System::Mutex::Mutex()
{
    InitializeCriticalSection( &m_mutex );
}

Kit::System::Mutex::~Mutex()
{
    DeleteCriticalSection( &m_mutex );
}

void Kit::System::Mutex::lock( void )
{
    EnterCriticalSection( &m_mutex );
}


void Kit::System::Mutex::unlock( void )
{
    LeaveCriticalSection( &m_mutex );
}


