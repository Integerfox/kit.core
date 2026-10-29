#ifndef KIT_SYSTEM_RPPICO_BSEMAPHORE_T_H_
#define KIT_SYSTEM_RPPICO_BSEMAPHORE_T_H_
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
    statically allocated/created Kit::System::BSemaphore instances. The issue
    here is that 'pico semaphore' need to be initialized AFTER main() is called.

    Note: The SDK semaphore is created with a max permit count of one - this is
          what provides the binary (i.e. saturate at one) semantics.
 */
class BSemaphore_T : public Kit::System::IStartupHook
{
public:
    /** Constructor.
        Note: The registration is assumed to be done from a static allocated
              instance (i.e. before main() and Kit::System::Api::initialize() is
              called).  However, registering AFTER wards is a don't care because
              the 'start-up list' is only used during the call to Kit::System::Api::initialize()
     */
    BSemaphore_T( bool initialStateIsOne )
        : IStartupHook( SYSTEM )
        , m_initialStateIsOne( initialStateIsOne )
    {
    }


protected:
    /// This method is called when Kit::System::initialize() executes
    void notify( InitLevel init_level ) noexcept override
    {
        sem_init( &m_sdkSema, m_initialStateIsOne ? 1 : 0, 1 );
    }

public:
    /// Platform Semaphore instance. Note: Made public to simply access from the Kit::System::BSemaphore class
    semaphore_t m_sdkSema;

protected:
    /// Initial state
    bool m_initialStateIsOne;
};


}  // End namespace(s)
}
}
#endif  // end header latch
