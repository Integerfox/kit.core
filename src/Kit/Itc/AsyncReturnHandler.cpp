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

#include "AsyncReturnHandler.h"
#include "Message.h"
#include "PostApi.h"

//
using namespace Cpl::Itc;


//////////////////////////////////////
AsyncReturnHandler::AsyncReturnHandler( PostApi& mbox, Message& msg )
    :m_mbox( mbox ),
    m_msg( msg )
{
}

void AsyncReturnHandler::rts() noexcept
{
    m_mbox.post( m_msg );
}

#endif