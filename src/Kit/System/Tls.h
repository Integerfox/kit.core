#ifndef KIT_SYSTEM_TLS_H_
#define KIT_SYSTEM_TLS_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "kit_config.h"
#include <stdint.h>

/** Default type of for TLS index.  This type only needs to be changed if the
    target OS/RTOS is providing its own TLS implementation.
 */
#ifndef KitSystemTlsIndex_T
#define KitSystemTlsIndex_T uint16_t
#endif

/** This constant defines the DESIRED minimum of number of TLS instances/index.
    However, this is only a 'suggestion' to the underlying platform
    implementation, i.e. Platforms are NOT required to honor the specified
    minimum
 */
#ifndef OPTION_KIT_SYSTEM_TLS_DESIRED_MIN_INDEXES
#define OPTION_KIT_SYSTEM_TLS_DESIRED_MIN_INDEXES       8
#endif


///
namespace Kit {
///
namespace System {

/** This concrete interface defines a interface for Thread Local Storage (TLS).
    TLS provides a mechanism for each thread to have its own instance of a
    global variable.  A canonical example of TLS is the C error code variable
    'errno'.

    The implementation assumes no OS support for TLS.  However, that said, it 
    can be used on platform where TLS is natively supported, i.e. one less 
    interface to port for a new OS/RTOS.

    NOTES:

        o The initial contents (per thread) of a TLS variable will be zero.
        o If a new TLS variable/index can NOT be created (i.e. exceeded the
          Platforms supported limits) a Fatal Error is generated.
        o A TLS variable can ONLY store a void pointer (or a size_t integer).
        o Each instance of this class represents a single TLS variable that
          is SAME across ALL threads, i.e. do NOT create a TLS instance per
          thread, create a TLS instance per global variable.
        o TLS instances can NOT be created statically, i.e. they must be
          created AFTER the Kit::System::initialize() is called.
        o TLS instances can NOT be copied.
        o It is PLATFORM SPECIFIC BEHAVIOR if deleting a TLS instance frees
          up memory for another instance.
 */
class Tls
{
public:
    /// Constructor
    Tls();

    /// Destructor
    ~Tls();

public:
    /// See Kit::System::ITls
    void* get() const noexcept;

    /// See Kit::System::ITls
    void set( void* newValue ) noexcept;


protected:
    /// Raw TLS key/handle/index
    KitSystemTlsIndex_T m_keyIdx;

private:
    /// Prevent access to the copy constructor -->TLS instances can not be copied!
    Tls( const Tls& m ) = delete;

    /// Prevent access to the assignment operator -->TLS instances can not be copied!
    const Tls& operator=( const Tls& m ) = delete;

    /// Prevent access to the move constructor -->TLS instances can not be implicitly moved!
    Tls( Tls&& m ) = delete;

    /// Prevent access to the move assignment operator -->TLS instances can not be implicitly moved!
    Tls& operator=( Tls&& m ) = delete;
};



};      // end namespaces
};
#endif  // end header latch
