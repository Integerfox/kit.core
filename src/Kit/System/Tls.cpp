/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/Tls.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/Mutex.h"
#include "Kit/System/Private.h"
#include "Kit/System/Shutdown.h"
#include "Thread.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace System {


static KitSystemTlsIndex_T nextTlsIndex_;  // This guarantied to be initialized to zero (per the C++ standard) because it is in the BSS segment

#define FAILURE     static_cast<KitSystemTlsIndex_T>(-1)

/////////////////////////////////////////////////////////
Tls::Tls( void )
    : m_keyIdx( FAILURE )  // Assume failure
{
    // Check if there are any TLS indexes/variables left
    Kit::System::PrivateLocks::system().lock();
    if ( nextTlsIndex_ < OPTION_KIT_SYSTEM_TLS_DESIRED_MIN_INDEXES )
    {
        m_keyIdx = nextTlsIndex_++;
    }
    Kit::System::PrivateLocks::system().unlock();


    // Generate a fatal error if I am out of TLS indexes/variables
    if ( m_keyIdx == FAILURE )
    {
        Kit::System::FatalError::logRaw( Shutdown::eOSAL, "Tls::Tls().  Exceeded TLS indexes.", nextTlsIndex_ );
    }
}


Tls::~Tls()
{
    /* Nothing needed because the current implementation DOES NOT recycle
       TLS indexes.
    */
}


/////////////////////////////////////////////////////////
void* Tls::get( void ) const noexcept
{
    // Get access to my FreeRTOS thread object.
    void** myArray = Thread::getTlsArray();
    return myArray[m_keyIdx];
}

void Tls::set( void* newValue ) noexcept
{
    // Get access to my FreeRTOS thread object.
    void** myArray = Thread::getTlsArray();
    myArray[m_keyIdx] = newValue;
}


} // end namespaces
}
//------------------------------------------------------------------------------