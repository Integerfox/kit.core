#ifndef ITC_SYNCHRONOUS_IRATE_H_
#define ITC_SYNCHRONOUS_IRATE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include <stdint.h>

///
namespace Itc {
///
namespace Synchronous {

/** This abstract class defines the interface for setting the application's
    LED flash rate. 

    Note: The intended implementation of this interface is to use Synchronous ITC.
          This means that the caller needs to be a different thread that the
          thread that physically flashes the LED. 

 */
class IRate
{
public:
    /** Sets the flash rate of the LED. Returns true if the operation was
        successful.  See IRateRequest::RatePayload_T for details on the valid
        flash rate range.
     */
    virtual bool set( uint32_t flashRateMs = 500) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IRate() = default;
};


}      // end namespaces
}
#endif  // end header latch
