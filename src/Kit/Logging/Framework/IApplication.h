#ifndef KIT_LOGGING_FRAMEWORK_IAPPLICATION_H_
#define KIT_LOGGING_FRAMEWORK_IAPPLICATION_H_
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
#include <stdint.h>


/// The text string for the 'unknown' category ID
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_CLASSIFICATION_ID_TEXT
#define OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_CLASSIFICATION_ID_TEXT "UNKNOWN"
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
public:
    /// Null/Invalid Classification ID
    static constexpr uint8_t NULL_CLASSIFICATION_ID = 0;

    /// Null/Invalid Classification ID Text
    static constexpr const char* NULL_CLASSIFICATION_ID_TEXT = OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_CLASSIFICATION_ID_TEXT;

public:
    /** This method returns true if specific Classification ID is supported by
        the application
     */
    virtual bool isClassificationIdValid( uint8_t classificationId ) noexcept = 0;

    /** This method returns true if the specific Package ID is supported by
        the application
     */
    virtual bool isPackageIdValid( uint8_t packageId ) noexcept = 0;

public:
    /** This method is used to convert a numeric Classification ID into a text
        string.  The returned string must be a null-terminated string and astring
        length of less or equal to OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_CLASSIFICATION_ID_TEXT.

        NOTE: This method never 'fails'.  If an unknown/unsupported Classification ID
        is used - the methods returns: OPTION_KIT_LOGGING_UNKNOWN_CLASSIFICATION_ID_TEXT
     */
    virtual const char* classificationIdToString( uint8_t classificationId ) noexcept = 0;

    /** This method return's the IPackage instance as for the specified 'packageId'.
        If the packageId is not recognized, then the NullPackage instance is returned
     */
    virtual IPackage& getPackage( uint8_t packageId ) noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IApplication() noexcept {}
};

}  // end namespaces
}
}
#endif  // end header latch
