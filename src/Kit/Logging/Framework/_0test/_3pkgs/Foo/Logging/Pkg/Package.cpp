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
namespace Foo {
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
    case SubSystemId::UI:
        dstSubSystemText = ( +SubSystemId::UI )._to_string();
        found            = isValidMessage<UiMsgId>( messageId, dstMessageText );
        break;

    case SubSystemId::DATABASE:
        dstSubSystemText = ( +SubSystemId::DATABASE )._to_string();
        found            = isValidMessage<DatabaseMsgId>( messageId, dstMessageText );
        break;

    case SubSystemId::API:
        dstSubSystemText = ( +SubSystemId::API )._to_string();
        found            = isValidMessage<ApiMsgId>( messageId, dstMessageText );
        break;

    default:
        break;
    }

    return found;
}

}  // end namespaces
}
}
//------------------------------------------------------------------------------
