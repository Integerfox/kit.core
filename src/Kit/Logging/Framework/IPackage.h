#ifndef KIT_LOGGING_FRAMEWORK_IPACKAGE_H_
#define KIT_LOGGING_FRAMEWORK_IPACKAGE_H_
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


/// The text string for the 'unknown' Package ID
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_PACKAGE_ID_TEXT
#define OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_PACKAGE_ID_TEXT "UNKNOWN"
#endif

/// The text string for the 'unknown' SubSystem ID
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_SUBSYSTEM_ID_TEXT
#define OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_SUBSYSTEM_ID_TEXT "UNKNOWN"
#endif

/// The text string for the 'unknown' Message ID
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_MESSAGE_ID_TEXT
#define OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_MESSAGE_ID_TEXT "UNKNOWN"
#endif


///
namespace Kit {
///
namespace Logging {
///
namespace Framework {

/** This abstract class defines a set of functionality that a Package
    is responsible for providing.
*/
class IPackage
{
public:
    /// Null/Invalid Package ID
    static constexpr uint8_t NULL_PKG_ID = 0;

    /// Null/Invalid Package ID
    static constexpr const char* NULL_PKG_ID_TEXT = "?";

    /// Null/Invalid SubSystem ID
    static constexpr uint8_t NULL_SUBSYS_ID = 0xFF;

    /// Null/Invalid SubSystem Text
    static constexpr const char* NULL_SUBSYS_ID_TEXT = "?";

    /// Null/Invalid Message ID
    static constexpr uint8_t NULL_MSG_ID = 0xFF;
    
    /// Null/Invalid Message Text
    static constexpr const char* NULL_MSG_ID_TEXT = "?";

public:
    /// This method returns the Package's ID as a numeric value
    virtual uint8_t packageId() noexcept = 0;

    /** This method returns the Package's ID as text string.  The returned
        string must be a null-terminated string and a string length of less or
        equal to OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_PACKAGE_ID_TEXT.
     */
    virtual const char* packageIdString() noexcept = 0;

    /** This method is used to convert a numeric Sub-system and Message IDs into
        a text strings.

        The returned Sub-System text will be a null-terminated string and a string
        length of less or equal to OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_SUBSYSTEM_ID_TEXT.

        The returned Message text  must be a null-terminated string and a string
        length of less or equal to OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_MESSAGE_ID_TEXT.

        The method returns true if the Sub-system and Message IDs are valid for this
        Package; otherwise false is returned.  When false is returned the return 
        text pointer is set to nullptr if its ID is invalid.
     */
    virtual bool subSystemAndMessageIdsToString( uint8_t      subSystemId,
                                                 const char*& dstSubSystemText,
                                                 uint8_t      messageId,
                                                 const char*& dstMessageText ) noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IPackage() noexcept {}
};

}  // end namespaces
}
}
#endif  // end header latch
