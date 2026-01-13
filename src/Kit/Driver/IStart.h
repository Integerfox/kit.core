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

/** This abstract class defines a start operation that is intended to start and/or
    activate a driver.  Once a driver has been started it, it should be 'stopped'
    (by calling its IStop interface) when/if the application performs a graceful
    shutdown.

    Note: It is possible for a concrete driver class to implement more than one
          abstract driver interfaces.  This use case introduces the classic C++
          'diamond' inheritance problem.  The diamond inheritance is resolved
          by requiring each abstract driver interface to use 'virtual' inheritance
          when inheriting from IStart.  This ensures that there is only one
          start() method in the concrete child classes
 */
class IStart
{
public:
    /** This method starts the driver.  It supports passing optional start
        arguments via the startArgs parameter.  The actual start arguments are
        platform and driver specific.

        The method returns true if the driver was started successfully,
        otherwise it returns false.

        Note: Calling start() on an already started driver has no effect.
     */
    virtual bool start( void* startArgs = nullptr ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IStart() noexcept = default;
};

}  // end namespaces
}
#endif  // end header latch
