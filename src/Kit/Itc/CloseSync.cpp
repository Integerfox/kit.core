#if 0
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "CloseSync.h"
#include "SyncReturnHandler.h"

///
using namespace Cpl::Itc;


///////////////////
CloseSync::CloseSync( PostApi& myMbox ) noexcept
    :OpenSync( myMbox )
{
}


///////////////////
bool CloseSync::close( void* args )
{
    ClosePayload        payload(args);
    SyncReturnHandler   srh;
    CloseMsg 	        msg( *this, payload, srh );
    m_mbox.postSync( msg );
    return payload.m_success;
}
#endif