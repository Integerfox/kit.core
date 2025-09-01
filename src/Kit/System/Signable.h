#ifndef KIT_SYSTEM_SIGNABLE_H_
#define KIT_SYSTEM_SIGNABLE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


///
namespace Kit {
///
namespace System {

/** This abstract class defines the interface by which a client
    can cause an object that is waiting-on-a-signal to become
    unblocked.
 */
class Signable
{
public:
    /** This method is called by client to cause the server to become
        unblock and ready to run.  This method can ONLY be called from
        a thread context. The method returns zero on success.
     */
    virtual int signal( void ) noexcept = 0;

    /** Same as signal(), EXCEPT this method can ONLY be called from
        supervisor mode and/or ISR contexts.  The method returns zero on
        success.
     */
    virtual int su_signal( void ) noexcept = 0;

public:
    /// Virtual destructor
    virtual ~Signable() = default;
};


}       // end namespaces
}
#endif  // end header latch
