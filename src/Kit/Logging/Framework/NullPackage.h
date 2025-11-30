#ifndef KIT_LOGGING_FRAMEWORK_NULLPACKAGE_H_
#define KIT_LOGGING_FRAMEWORK_NULLPACKAGE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/Framework/IPackage.h"


///
namespace Kit {
///
namespace Logging {
///
namespace Framework {

/** This concrete class implements the Logging Domain interface for 'Null'
    domain instances, i.e. used when unsupported Domain ID is encountered
*/
class NullPackage : public Kit::Logging::Framework::IPackage
{
public:
    /// Constructor
    NullPackage() = default;
    
public:
    /// See Kit::Logging::Framework::IPackage
    uint8_t packageId() noexcept override { return NULL_PKG_ID; };

    /// See Kit::Logging::Framework::IPackage
    const char* packageIdString() noexcept override { return NULL_PKG_ID_TEXT; };

    /// See Kit::Logging::Framework::IPackage
    bool subSystemAndMessageIdsToString( uint8_t      subSystemId,
                                         const char*& dstSubSystemText,
                                         uint8_t      messageId,
                                         const char*& dstMessageText ) noexcept override
    {
        dstSubSystemText = nullptr;
        dstMessageText   = nullptr;
        return false;
    }
};

}  // end namespaces
}
}
#endif  // end header latch
