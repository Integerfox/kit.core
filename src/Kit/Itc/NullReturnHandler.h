#ifndef KIT_ITC_NULL_RETURN_HANDLER_H_
#define KIT_ITC_NULL_RETURN_HANDLER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Itc/IReturnHandler.h"

///
namespace Kit {
///
namespace Itc {

/** This class implements a Null ReturnHandler. This means the message
    is NEVER returned to the calling thread.
 */
class NullReturnHandler : public IReturnHandler
{
public:
    /// Constructor
    NullReturnHandler() noexcept
    {
    }

public:
    /// See Kit::Itc::IReturnHandler
    void rts() noexcept override
    {
    }
};


}  // end namespaces
}
#endif  // end header latch
