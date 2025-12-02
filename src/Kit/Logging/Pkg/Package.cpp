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
namespace Kit {
namespace Logging {
namespace Pkg {

// Compile-time validation of PACKAGE_ID
static_assert( Package::PACKAGE_ID > 0 && Package::PACKAGE_ID <=  sizeof(KitLoggingPackageMask_T) * 8, "PACKAGE_ID cannot be zero or exceeds the number of bits in KitLoggingPackageMask_T" );

// Compile-time validation that SubSystemId/MessageId does not contain a symbol with value 255
// Note: Using _size_constant to make this scalable - validates the max underlying value is < 255
static_assert( SubSystemId::_size_constant > 0 && SubSystemId::_size_constant <= 255, "SubSystemId enum size must not exceed 255, and since enums are 0-indexed, no value can be 255" );
static_assert( SystemMsgId::_size_constant > 0 && SystemMsgId::_size_constant <= 255, "SystemMsgId enum size must not exceed 255, and since enums are 0-indexed, no value can be 255" );
static_assert( DriverMsgId::_size_constant > 0 && DriverMsgId::_size_constant <= 255, "DriverMsgId enum size must not exceed 255, and since enums are 0-indexed, no value can be 255" );
static_assert( LoggingMsgId::_size_constant > 0 && LoggingMsgId::_size_constant <= 255, "LoggingMsgId enum size must not exceed 255, and since enums are 0-indexed, no value can be 255" );

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
