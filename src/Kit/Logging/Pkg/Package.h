#ifndef KIT_LOGGING_API_PACKAGE_H_
#define KIT_LOGGING_API_PACKAGE_H_
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
#include "Kit/Logging/Framework/IPackage.h"

/// Defer the actual Package numeric value for the Kit Package to the Application (via the kit_config.h file).
#define KIT_LOGGING_PKG_PACKAGE_ID KIT_LOGGING_PKG_PACKAGE_ID_MAPCFG

/// The Kit Package ID Text String
#ifndef OPTION_KIT_LOGGING_PKG_PACKAGE_ID_TEXT
#define OPTION_KIT_LOGGING_PKG_PACKAGE_ID_TEXT "KIT"
#endif


///
namespace Kit {
///
namespace Logging {
///
namespace Pkg {
/** This concrete class implements the Logging Package interface for the Kit Logging
    Package
*/
class Package : public Kit::Logging::Framework::IPackage
{
    public:
    /// The Package's ID
    static constexpr uint8_t PACKAGE_ID = KIT_LOGGING_PKG_PACKAGE_ID;

    /// The Package's ID Text String
    static constexpr const char* PACKAGE_ID_TEXT = OPTION_KIT_LOGGING_PKG_PACKAGE_ID_TEXT;
    
public:
    /// Constructor
    Package() = default;

public:
    /// See Kit::Logging::Framework::IPackage
    uint8_t packageId() noexcept override;

    /// See Kit::Logging::Framework::IPackage
    const char* packageIdString() noexcept override;

    /// See Kit::Logging::Framework::IPackage
    const char* subSystemIdToString( uint8_t subSystemId ) noexcept override;

    /// See Kit::Logging::Framework::IPackage
    const char* messageIdToString( uint8_t subSystemId, uint8_t messageId ) noexcept override;
};

}  // end namespaces
}
}
#endif  // end header latch
