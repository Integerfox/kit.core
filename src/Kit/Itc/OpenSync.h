#ifndef KIT_ITC_OPEN_SYNC_H_
#define KIT_ITC_OPEN_SYNC_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Itc/IOpenRequest.h"
#include "Kit/Itc/IOpen.h"
#include "Kit/EventQueue/IQueue.h"


///
namespace Kit {
///
namespace Itc {


/** This partially concrete class implements the synchronous ITC open
    call for the OpenApi.  The subclass is still required to implement the ITC
    request() method.
 */
class OpenSync : public IOpenRequest, public IOpen
{
public:
    /** Constructor.  The 'myEventLoop' is the ITC server object's event loop
        reference of where the service executes uses to receive request messages.
     */
    OpenSync( Kit::EventQueue::IQueue& myEventQueue ) noexcept;


public:
    /// See Kit::Itc::IOpen
    bool open( void* args = nullptr ) noexcept override;

protected:
    /// Mailbox (of the server implementing the ITC request() method
    Kit::EventQueue::IQueue& m_eventQueue;
};


}  // end namespaces
}
#endif  // end header latch
