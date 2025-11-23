#ifndef KIT_LOGGING_API_DOMAIN_H_
#define KIT_LOGGING_API_DOMAIN_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "kit_map.h"
#include "Kit/Logging/Framework/IDomain.h"
#include <stdint.h>

/// Defer the actual Domain numeric value for the Kit Domain to the Application
#define KIT_LOGGING_DOMAIN_DOMAIN_ID 1 // KIT_LOGGING_DOMAIN_DOMAIN_ID_MAP

/// The Kit Domain ID Text String
#ifndef OPTION_KIT_LOGGING_DOMAIN_DOMAIN_ID_TEXT
#define OPTION_KIT_LOGGING_DOMAIN_DOMAIN_ID_TEXT "KIT"
#endif


///
namespace Kit {
///
namespace Logging {
///
namespace Api {

/** This concrete class implements the Logging Domain interface for the Kit Logging
    Domain
*/
class Domain : public Kit::Logging::Framework::IDomain
{
public:
    /// Constructor
    Domain() = default;

public:
    /// See Kit::Logging::Framework::IDomain
    uint8_t domainId() noexcept override;

    /// See Kit::Logging::Framework::IDomain
    const char* domainIdString() noexcept override;

    /// See Kit::Logging::Framework::IDomain
    const char* subSystemIdToString( uint8_t subSystemId ) noexcept override;

    /// See Kit::Logging::Framework::IDomain
    const char* messageIdToString( uint8_t subSystemId, uint8_t messageId ) noexcept override;
};

}  // end namespaces
}
}
#endif  // end header latch
