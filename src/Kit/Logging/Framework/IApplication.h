#ifndef KIT_LOGGING_IAPPLICATION_H_
#define KIT_LOGGING_IAPPLICATION_H_
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
#include <stdint.h>


/// The text string for the 'unknown' category ID
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_CATEGORY_ID_TEXT
#define OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_CATEGORY_ID_TEXT "UNKNOWN"
#endif

///
namespace Kit {
///
namespace Logging {
///
namespace Framework {

/** This abstract class defines a set of functionality that the Application
    is responsible for providing.  Implementation of this interface requires
    the knowledge of what Domains and Sub-systems are contained with the
    Application.
*/
class IApplication
{
    /// Null/Invalid Category ID
    static constexpr uint8_t NULL_CATEGORY_ID = 0;

    /// Null/Invalid Category ID Text
    static constexpr const char* NULL_CATEGORY_ID_TEXT = OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_CATEGORY_ID_TEXT;

public:
    /** This method is used to convert a numeric Category ID into a text string.
        NOTE: This method never 'fails'.  If an unknown/unsupported Category ID
        is used - the methods returns: OPTION_KIT_LOGGING_UNKNOWN_CATEGORY_ID_TEXT
     */
    virtual const char* categoryIdToString( uint8_t categoryId ) noexcept = 0;

    /** This method return's the IDomain instance as for the specified 'domainId'.
        If the domainId is not recognized, then the NullDomain instance is returned
     */
    virtual IDomain& getDomain( uint8_t domainId ) noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IApplication() noexcept {}
};

}  // end namespaces
}
}
#endif  // end header latch
