#ifndef KIT_ITC_OPEN_CLOSE_SYNC_H_
#define KIT_ITC_OPEN_CLOSE_SYNC_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Itc/OpenSync.h"
#include "Kit/Itc/ICloseRequest.h"
#include "Kit/Itc/IClose.h"

///
namespace Kit {
///
namespace Itc {

/** This partially concrete class extends the ITC OpenSync class and implements
    the synchronous ITC close call for the CloseApi.  The subclass is still 
    required to implement the ITC request() method.

    NOTE: This class is derived from OpenSync -->if you support a close(),
          call, you better have had an open() call.
 */
class OpenCloseSync : public OpenSync, public ICloseRequest, public IClose
{
public:
    /** Constructor.  The 'myEventLoop' is the ITC server object's event loop
        reference of where the service executes uses to receive request messages.
     */
    OpenCloseSync( Kit::EventQueue::Server& myEventLoop ) noexcept;


public:
    /// See Kit::Itc::IClose
    bool close( void* args = nullptr ) noexcept override;
};




}      // end namespaces
}
#endif  // end header latch
