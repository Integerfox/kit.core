#ifndef KIT_SYSTEM_RPPICO_SEMAPHORE_T_H_
#define KIT_SYSTEM_RPPICO_SEMAPHORE_T_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/PrivateStartup.h"
#include "pico/sem.h"
#include <new>

///
namespace Kit {
///
namespace System {
///
namespace RPPico {

/** This class implements a brute force approach to initializing a 'pico
    semaphore' when the Kit C++ library is initialized.  This is to support
    statically allocated/created Kit::System::Semaphore instances. The issue here
    is that 'pico semaphore' need to be initialized AFTER main() is called.  
 */
class Semaphore_T : public Kit::System::IStartupHook
{
public:
    /** Constructor.
        Note: The registration is assumed to be done from a static allocated
              instance (i.e. before main() and Kit::System::Api::initialize() is
              called).  However, registering AFTER wards is a don't care because
              the 'start-up list' is only used during the call to Kit::System::Api::initialize()
     */
    Semaphore_T( unsigned initialCount ) 
        : IStartupHook( SYSTEM )
        , m_sdkSema( nullptr )
        , m_initialCount( initialCount ) 
    {
    }


protected:
    /// This method is called when Kit::System::initialize() executes
    void notify( InitLevel init_level )
    {
        m_sdkSema = new semaphore_t;
        if ( m_sdkSema )
        {
            sem_init( m_sdkSema, m_initialCount, 0x7FFF );
        }
    }

public:
    /// Platform Semaphore instance. Note: Made public to simply access from the Kit::System::Semaphore class
    semaphore_t*    m_sdkSema;

protected:
    /// Initial count
    unsigned        m_initialCount;
};


}   // End namespace(s)
}
}
#endif  // end header latch
