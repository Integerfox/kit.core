/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Domain.h"
#include "SubSystemId.h"
#include "Kit/Logging/Api/OsalMsgId.h"
#include "Kit/Logging/Api/DriverMsgId.h"
#include "Kit/Logging/Framework/IDomain.h"
#include "SubSystemId.h"

using namespace Kit::Logging::Api;


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
uint8_t Domain::domainId() noexcept
{
    return KIT_LOGGING_DOMAIN_DOMAIN_ID;
}

const char* Domain::domainIdString() noexcept
{
    return OPTION_KIT_LOGGING_DOMAIN_DOMAIN_ID_TEXT;
}

const char* Domain::subSystemIdToString( uint8_t subSystemId ) noexcept
{
    return enumToString<SubSystemId>( subSystemId, OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_SUBSYSTEM_ID_TEXT );
}

const char* Domain::messageIdToString( uint8_t subSystemId, uint8_t messageId ) noexcept
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
