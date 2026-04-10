#ifndef KIT_DRIVER_ISTOP_H_
#define KIT_DRIVER_ISTOP_H_
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
namespace Driver {

/** This abstract class defines the method for stopping a driver. See the
    IStart class definition for more details.
 */
class IStop
{
public:
    /** This method is use to stop the driver at run time.  This
        method is NOT thread-safe.  It is application responsibility to provide
        any thread-safety that it needs with respect to stopping (and starting)
        drivers.
     */
    virtual void stop() noexcept = 0;

public:
    /// Lets the make the destructor virtual
    virtual ~IStop() noexcept = default;

};

}       // end namespaces
}
#endif  // end header latch
