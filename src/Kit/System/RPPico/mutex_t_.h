#ifndef KIT_SYSTEM_RPPICO_MUTEX_T_H_
#define KIT_SYSTEM_RPPICO_MUTEX_T_H_
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
#include "pico/mutex.h"
#include <new>

///
namespace Kit {
///
namespace System {
///
namespace RPPico {

/** This class implements a brute force approach to initializing a 'pico
    mutex' when the Kit C++ library is initialized.  This is to support
    statically allocated/created Kit::System::Mutex instances. The issue here
    is that 'pico mutex' need to be initialized AFTER main() is called.  The 
    Pico-SDK solution for statically allocating mutex instances is C solution 
    that does not work inside the Kit::System::Mutex object.
 */
class Mutex_T : public Kit::System::IStartupHook
{
public:
    /** Constructor.
        Note: The registration is assumed to be done from a static allocated
              instance (i.e. before main() and Kit::System::Api::initialize() is
              called).  However, registering AFTER wards is a don't care because
              the 'start-up list' is only used during the call to Kit::System::Api::initialize() 
     */
    Mutex_T() :IStartupHook( SYSTEM ) {}


protected:
    /// This method is called when Kit::System::initialize() executes
    void notify( InitLevel init_level ) noexcept override
    {
        recursive_mutex_init( &m_sdkMutex );
    }

public:
    /// Platform Mutex instance. Note: Made public to simply access from the Kit::System::Mutex class
    recursive_mutex_t   m_sdkMutex;
};


}   // End namespace(s)
}
}
#endif  // end header latch
