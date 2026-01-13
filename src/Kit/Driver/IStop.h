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

/** This abstract class defines a stop operation that is intended to stop and/or
    shutdown a driver.  Once a driver has been stopped, it can be restarted
    by calling the its IStart interface.

    Note: It is possible for a concrete driver class to implement more than one
          abstract driver interfaces.  This use case introduces the classic C++
          'diamond' inheritance problem.  The diamond inheritance is resolved
          by requiring each abstract driver interface to use 'virtual' inheritance
          when inheriting from IStop.  This ensures that there is only one
          stop() method in the concrete child classes
 */
class IStop
{
public:
    /** This method stops the driver.  The stopped 'state' is platform specific.
        Once a driver has been stopped it can be restarted by calling its IStart
        interface.

        Note: Calling stop() on an already stopped driver has no effect.
     */
    virtual void stop() noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IStop() noexcept = default;
};

}  // end namespaces
}
#endif  // end header latch
