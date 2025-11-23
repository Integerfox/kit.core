#ifndef KIT_LOGGING_FRAMEWORK_NULLDOMAIN_H_
#define KIT_LOGGING_FRAMEWORK_NULLDOMAIN_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/Framework/IDomain.h"


///
namespace Kit {
///
namespace Logging {
///
namespace Api {

/** This concrete class implements the Logging Domain interface for 'Null'
    domain instances, i.e. used when unsupported Domain ID is encountered
*/
class NullDomain : public Kit::Logging::Framework::IDomain
{
public:
    /// Constructor
    NullDomain() = default;

public:
    /// See Kit::Logging::Framework::IDomain
    uint8_t domainId() noexcept override { return NULL_DOMAIN_ID; };

    /// See Kit::Logging::Framework::IDomain
    const char* domainIdString() noexcept override { return NULL_DOMAIN_ID_TEXT; };

    /// See Kit::Logging::Framework::IDomain
    const char* subSystemIdToString( uint8_t subSystemId ) noexcept override { return OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_SUBSYSTEM_ID_TEXT; };

    /// See Kit::Logging::Framework::IDomain
    const char* messageIdToString( uint8_t subSystemId, uint8_t messageId ) noexcept override { return OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_MESSAGE_ID_TEXT; };
};

}  // end namespaces
}
}
#endif  // end header latch
