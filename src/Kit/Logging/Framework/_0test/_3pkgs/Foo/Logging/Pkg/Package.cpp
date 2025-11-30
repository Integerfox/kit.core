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
#include "Kit/Logging/Framework/types.h"

//------------------------------------------------------------------------------
namespace Foo {
namespace Logging {
namespace Pkg {

// Compile-time validation of PACKAGE_ID
static_assert( Package::PACKAGE_ID > 0 && Package::PACKAGE_ID <= sizeof( KitLoggingPackageMask_T ) * 8, "PACKAGE_ID cannot be zero or exceeds the number of bits in KitLoggingPackageMask_T" );

// Compile-time validation that SubSystemId/MessageId does not contain a symbol with value 255
// Note: Using _size_constant to make this scalable - validates the max underlying value is < 255
static_assert( SubSystemId::_size_constant > 0 && SubSystemId::_size_constant <= 255, "SubSystemId enum size must not exceed 255, and since enums are 0-indexed, no value can be 255" );
static_assert( UiMsgId::_size_constant > 0 && UiMsgId::_size_constant <= 255, "UiMsgId enum size must not exceed 255, and since enums are 0-indexed, no value can be 255" );
static_assert( DatabaseMsgId::_size_constant > 0 && DatabaseMsgId::_size_constant <= 255, "DatabaseMsgId enum size must not exceed 255, and since enums are 0-indexed, no value can be 255" );
static_assert( ApiMsgId::_size_constant > 0 && ApiMsgId::_size_constant <= 255, "ApiMsgId enum size must not exceed 255, and since enums are 0-indexed, no value can be 255" );


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
