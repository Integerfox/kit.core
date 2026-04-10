#ifndef KIT_DRIVER_ISTART_H_
#define KIT_DRIVER_ISTART_H_
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

/** This abstract class defines the method for starting a driver.  The 
    KIT driver model requires:
    
    - All drivers must be started at run time before they can be used, i.e. the
      driver implementation will fail all calls if the driver has not been
      started.

    - All drivers should be stopped (see IStop) when they are no longer needed
      or when the application is performing a orderly shutdown.

    - Drivers are implemented in a way that allows them to be started and 
      stopped multiple times during the life of the application. 

    - Calling start() on already started driver shall no effect, i.e. no actions
      taken other than returning true.

    - Calling stop() on a driver that is not started shall no effect, i.e. no actions
      taken.
 */
class IStart
{
public:
    /** This method is use to start/initialize the driver at run time.  This
        method is NOT thread-safe.  It is application responsibility to provide
        any thread-safety that it needs with respect to starting (and stopping)
        drivers.

        The 'startArgs' optional argument that provides run time information to the
        driver.  When used, the definition of what 'startArgs' is driver is specific
        to the individual concrete driver implementations. 
    
        The method return true if the driver is successfully started; else false
        is returned.
     */
    virtual bool start( void* startArgs = nullptr ) noexcept = 0;

public:
    /// Lets the make the destructor virtual
    virtual ~IStart() noexcept = default;

};

}       // end namespaces
}
#endif  // end header latch
