/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "ResetSync.h"
#include "IResetRequest.h"
#include "Kit/Itc/SyncReturnHandler.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Persistence {
namespace Record {
namespace Journal {

void ResetSync::logicalReset() noexcept
{
    LogicalResetRequest::Payload       msgPayload;
    Kit::Itc::SyncReturnHandler        srh;
    LogicalResetRequest::LogicalResetMsg msg( *this, msgPayload, srh );
    m_myEventQueue.postSync( msg );
}


}  // end namespace
}
}
}
//------------------------------------------------------------------------------
