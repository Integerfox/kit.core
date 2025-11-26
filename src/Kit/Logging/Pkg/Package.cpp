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
#include "SubSystemId.h"
#include "Kit/Logging/Pkg/OsalMsgId.h"
#include "Kit/Logging/Pkg/DriverMsgId.h"
#include "Kit/Logging/Framework/IPackage.h"
#include "SubSystemId.h"

using namespace Kit::Logging::Pkg;


// Helper method to convert BETTER ENUM to a string
template <typename ENUM>
static const char* enumToString( uint8_t numericValue, const char* unknownText ) noexcept
{
    auto maybe = ENUM::_from_integral_nothrow( numericValue );
    if ( !maybe )
    {
        return unknownText;
    }
    return maybe->_to_string();
}

/////////////
uint8_t Package::packageId() noexcept
{
    return KIT_LOGGING_PKG_PACKAGE_ID;
}

const char* Package::packageIdString() noexcept
{
    return OPTION_KIT_LOGGING_PKG_PACKAGE_ID_TEXT;
}

const char* Package::subSystemIdToString( uint8_t subSystemId ) noexcept
{
    return enumToString<SubSystemId>( subSystemId, OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_SUBSYSTEM_ID_TEXT );
}

const char* Package::messageIdToString( uint8_t subSystemId, uint8_t messageId ) noexcept
{
    switch ( subSystemId )
    {
    case SubSystemId::OSAL:
        return enumToString<OsalMsgId>( messageId, OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_MESSAGE_ID_TEXT );

    case SubSystemId::DRIVER:
        return enumToString<DriverMsgId>( messageId, OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_MESSAGE_ID_TEXT );

    default:
        return OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_MESSAGE_ID_TEXT;
    }
}
