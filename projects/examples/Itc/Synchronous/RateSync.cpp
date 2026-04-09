/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "RateSync.h"
#include "Kit/Itc/SyncReturnHandler.h"

//------------------------------------------------------------------------------
namespace Itc {
namespace Synchronous {

///////////////////
RateSync::RateSync( Kit::EventQueue::IQueue& myEventQueue ) noexcept
    : m_eventQueue( myEventQueue )
{
}

///////////////////
bool RateSync::set( uint32_t flashRateMs ) noexcept
{
    RatePayload_T               payload( flashRateMs );
    Kit::Itc::SyncReturnHandler srh;
    RateMsg                     msg( *this, payload, srh );
    m_eventQueue.postSync( msg );
    return payload.success;
}

}  // end namespace
}
//------------------------------------------------------------------------------