#ifndef KIT_LOGGING_FRAMEWORK_LOGGER_H_
#define KIT_LOGGING_FRAMEWORK_LOGGER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file declares the public facing Logging interface for managing the
    logging framework (i.e. everything except generating log entries).
    
    See the Kit/Logging/Framework/Log.h file details for the public interface
    for generating log entries.

    The interface IS thread safe.
*/

#include "Kit/Logging/Framework/IApplication.h"
#include "Kit/Logging/Framework/types.h"
#include "Kit/Logging/Pkg/Package.h"
#include "Kit/Logging/Pkg/ClassificationId.h"
#include "Kit/Logging/Pkg/SubSystemId.h"
#include "Kit/Logging/Pkg/SystemMsgId.h"

///
namespace Kit {
///
namespace Logging {
///
namespace Framework {


/*----------------------------------------------------------------------------*/
/** This method is used to initialize the Logging framework. It must be called
    before any LOGGING methods are called.
 */
void initialize( IApplication& appInstance,
                 /* Kit::Container::RingBufferMP<EntryData_T>& logEntryFIFO, */
                 uint8_t classificationIdForQueueOverflow = Kit::Logging::Pkg::ClassificationId::WARNING,
                 uint8_t packageIdForQueueOverflow        = Kit::Logging::Pkg::Package::PACKAGE_ID,
                 uint8_t subSystemIdForQueueOverflow      = Kit::Logging::Pkg::SubSystemId::SYSTEM,
                 uint8_t messageIdForQueueOverflow        = Kit::Logging::Pkg::SystemMsgId::LOGGING ) noexcept;

/*----------------------------------------------------------------------------*/
/** This method is used to enable one or more log Classifications ID, i.e
    OR'd to the existing enabled/disabled mask.  Returns the previous
    enabled/disabled mask.

    Use the classificationIdToMask() method to get the mask for a specific
    Classification ID.

    Note: Upon initialization ALL Classifications are enabled
 */
KitLoggingClassificationMask_T enableClassification( KitLoggingClassificationMask_T classificationMask ) noexcept;

/** This method is used to disable one or more log Classifications ID, i.e
    clear bits in the existing enabled/disabled mask.  Returns the previous
    enabled/disabled mask.

    Use the classificationIdToMask() method to get the mask for a specific
    Classification ID.
 */
KitLoggingClassificationMask_T disableClassification( KitLoggingClassificationMask_T classificationMask ) noexcept;

/** This method is used to explicit set the classification mask (i.e. the enable/
    disable methods are OR/AND operations, this is assignment operation)

    The method is thread safe
 */
void setClassificationMask( KitLoggingClassificationMask_T newMask ) noexcept;

/** This method returns the current enabled/disabled Classification mask
 */
KitLoggingClassificationMask_T getClassificationEnabledMask() noexcept;


/** This method is used to enable one or more log Package ID, i.e
    OR'd to the existing enabled/disabled mask.  Returns the previous
    enabled/disabled mask.

    Use the packageIdToMask() method to get the mask for a specific
    Package ID.

    Note: Upon initialization ALL Packages are enabled
 */
KitLoggingPackageMask_T enablePackage( KitLoggingPackageMask_T packageMask ) noexcept;

/** This method is used to disable one or more log Package ID, i.e
    clear bits in the existing enabled/disabled mask.  Returns the previous
    enabled/disabled mask.

    Use the packageIdToMask() method to get the mask for a specific
    Package ID.
 */
KitLoggingPackageMask_T disablePackage( KitLoggingPackageMask_T packageMask ) noexcept;

/** This method is used to explicit set the package mask (i.e. the enable/
    disable methods are OR/AND operations, this is assignment operation)

    The method is thread safe
 */
void setPackageMask( KitLoggingPackageMask_T newMask ) noexcept;

/** This method returns the current enabled/disabled Package mask
 */
KitLoggingPackageMask_T getPackageEnabledMask() noexcept;


/*----------------------------------------------------------------------------*/
/** This method is used to convert a numeric Classification ID into a filter-mask.

NOTE: If the classification ID is invalid (i.e. out of range) the method
      returns 0.
*/
KitLoggingClassificationMask_T classificationIdToMask( uint8_t classificationId ) noexcept;

/** This method converts a Classification ID filter-mask to classification
    ID.

    NOTE: If an invalid filter-mask is provided, e.g. the ID value exceeds
          the number of bits in the Mask Type, then IApplication::NULL_CLASSIFICATION_ID
          is returned.
 */
uint8_t maskToClassificationId( KitLoggingClassificationMask_T classificationMask ) noexcept;

/** This method is used to convert a numeric Package ID into a filter-mask.

    NOTE: If the package ID is invalid (i.e. out of range) the method
          returns 0.
 */
KitLoggingPackageMask_T packageIdToMask( uint8_t packageId ) noexcept;

/** This method converts a Package ID filter-mask to package
    ID.

    NOTE: If an invalid filter-mask is provided, e.g. the ID value exceeds
          the number of bits in the Mask Type, then IPackage::NULL_ID
          is returned.
 */
uint8_t maskToPackageId( KitLoggingPackageMask_T packageMask ) noexcept;


}  // end namespaces
}
}
#endif  // end header latch
