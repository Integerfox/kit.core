/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Bool.h"

///
using namespace Cpl::Dm::Mp;


///////////////////////////////////////////////////////////////////////////////
void Bool::attach( Observer& observer, uint16_t initialSeqNumber ) noexcept
{
    ModelPointCommon_::attachSubscriber( observer, initialSeqNumber );
}

void Bool::detach( Observer& observer ) noexcept
{
    ModelPointCommon_::detachSubscriber( observer );
}

const char* Bool::getTypeAsText() const noexcept
{
    return "Cpl::Dm::Mp::Bool";
}

void Bool::setJSONVal( JsonDocument& doc ) noexcept
{
    doc["val"] = m_data;
}

bool Bool::fromJSON_( JsonVariant& src, LockRequest_T lockRequest, uint16_t& retSequenceNumber, Cpl::Text::String* errorMsg ) noexcept
{
    if ( src.is<bool>() )
    {
        retSequenceNumber = write( src.as<bool>(), lockRequest );
        return true;
    }
    if ( errorMsg )
    {
        *errorMsg = "Invalid syntax for the 'val' key/value pair";
    }
    return false;
}
