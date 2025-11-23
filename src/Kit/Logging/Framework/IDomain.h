#ifndef KIT_LOGGING_IDOMAIN_H_
#define KIT_LOGGING_IDOMAIN_H_
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
#include <stdint.h>


/// The text string for the 'unknown' SubSystem ID
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_SUBSYSTEM_ID_TEXT
#define OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_SUBSYSTEM_ID_TEXT "UNKNOWN"
#endif

/// The text string for the 'unknown' Message ID
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_MESSAGE_ID_TEXT
#define OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_MESSAGE_ID_TEXT "UNKNOWN"
#endif

/// The text string for the 'unknown' Domain ID
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_DOMAIN_ID_TEXT
#define OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_DOMAIN_ID_TEXT "UNKNOWN"
#endif


///
namespace Kit {
///
namespace Logging {
///
namespace Framework {

/** This abstract class defines a set of functionality that a Domain
    is responsible for providing.
*/
class IDomain
{
public:
    /// Null/Invalid Domain ID
    static constexpr uint8_t NULL_DOMAIN_ID = 0;

    /// Null/Invalid Domain ID Text
    static constexpr const char* NULL_DOMAIN_ID_TEXT = "OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_DOMAIN_ID_TEXT";

public:
    /// This method returns the Domain's ID as a numeric value
    virtual uint8_t domainId() noexcept = 0;

    /// This method returns the Domain's ID as text string
    virtual const char* domainIdString() noexcept = 0;

    /** This method is used to convert a numeric Sub-system ID into a text string.
        NOTE: This method never 'fails'.  If an unknown/unsupported Sub-system ID
        is used - the methods returns: OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_SUBSYSTEM_ID_TEXT
     */
    virtual const char* subSystemIdToString( uint8_t subSystemId ) noexcept = 0;

    /** This method is used to convert a numeric Message ID into a text string.
        NOTE: This method never 'fails'.  If an unknown/unsupported Message ID
        is used - the methods returns: OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_MESSAGE_ID_TEXT
     */
    virtual const char* messageIdToString( uint8_t subSystemId, uint8_t messageId ) noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IDomain() noexcept {}
};

}  // end namespaces
}
}
#endif  // end header latch
