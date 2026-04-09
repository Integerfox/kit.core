#ifndef ITC_SYNCHRONOUS_RATESYNC_H_
#define ITC_SYNCHRONOUS_RATESYNC_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Itc/Synchronous/IRateRequest.h"
#include "Itc/Synchronous/IRate.h"
#include "Kit/EventQueue/IQueue.h"


///
namespace Itc {
///
namespace Synchronous {


/** This partially concrete class provides the synchronous semantics for ITC 
    IRateRequest message, i.e. it provides functional call semantics to the ITC
    transaction.  A subclass is still required to implement the IRateRequest
    request() method.
 */
class RateSync : public IRateRequest, public IRate
{
public:
    /** Constructor.  The 'myEventLoop' is the ITC server object's event loop
        reference of where the service executes uses to receive request messages.
     */
    RateSync( Kit::EventQueue::IQueue& myEventQueue ) noexcept;


public:
    /// See Kit::Itc::IRate
    bool set( uint32_t flashRateMs = 500 ) noexcept override;

protected:
    /// Mailbox (of the server implementing the ITC request() method
    Kit::EventQueue::IQueue& m_eventQueue;
};


}  // end namespaces
}
#endif  // end header latch
