/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Package.h"
#include "MsgId.h"
#include "SubSystemId.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Logging {
namespace Pkg {


// Helper method
template <typename MSG_ENUM>
static bool isValidMessage( uint8_t messageId, const char*& dstMessageText ) noexcept
{
    auto maybe = MSG_ENUM::_from_integral_nothrow( messageId );
    if ( maybe )
    {
        dstMessageText = maybe->_to_string();
    }
    return maybe;
}

/////////////
uint8_t Package::packageId() noexcept
{
    return PACKAGE_ID;
}

const char* Package::packageIdString() noexcept
{
    return PACKAGE_ID_TEXT;
}


bool Package::subSystemAndMessageIdsToString( uint8_t      subSystemId,
                                              const char*& dstSubSystemText,
                                              uint8_t      messageId,
                                              const char*& dstMessageText ) noexcept
{
    dstSubSystemText = nullptr;
    dstMessageText   = nullptr;
    bool found       = false;
    switch ( subSystemId )
    {
    case SubSystemId::SYSTEM:
        dstSubSystemText = ( +SubSystemId::SYSTEM )._to_string();
        found            = isValidMessage<SystemMsgId>( messageId, dstMessageText );
        break;

    case SubSystemId::DRIVER:
        dstSubSystemText = ( +SubSystemId::DRIVER )._to_string();
        found            = isValidMessage<DriverMsgId>( messageId, dstMessageText );
        break;

    case SubSystemId::LOGGING:
        dstSubSystemText = ( +SubSystemId::LOGGING )._to_string();
        found            = isValidMessage<LoggingMsgId>( messageId, dstMessageText );
        break;

    default:
        break;
    }

    return found;
}

}  // end namespace
}
}
//------------------------------------------------------------------------------
