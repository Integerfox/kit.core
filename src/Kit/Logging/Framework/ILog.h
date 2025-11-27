#ifndef KIT_LOGGING_FRAMEWORK_ILOG_H_
#define KIT_LOGGING_FRAMEWORK_ILOG_H_
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
#include "Kit/System/printfchecker.h"
#include <stdint.h>
#include <stdarg.h>

/** This data type determines the range of Classification IDs.  The range will
    be 1 to the number-of-bits in KitLoggingClassificationMask_T.

    Yes I am lazy here and use the LConfig pattern, instead of the LHeader pattern
    to define this type.
 */
#ifndef KitLoggingClassificationMask_T
#define KitLoggingClassificationMask_T uint32_t
#endif

/** This data type determines the range of Package IDs.  The range will
    be 1 to the number-of-bits in KitLoggingPackageMask_T.
 */
#ifndef KitLoggingPackageMask_T
#define KitLoggingPackageMask_T uint32_t
#endif

///
namespace Kit {
///
namespace Logging {
///
namespace Framework {

/** This abstract class defines the interface for clients to created/generate
    a log entry

    There are 2 run-time filters - one for Classification ID and one for Package
    ID. Only log entries that 'pass' BOTH filters are placed into the Log Entry
    FIFO.

    The inteface IS thread safe. Client's can call the logf() from any thread.
    However, because log entries can be echoed to the TRACE engine - the calling
    thread MUST be a KIT thread (see Kit::System::Trace interface for details
    on this restriction).
*/
class ILog
{
public:
    /** This method creates a log entry and adds it to the log entry queue.

        NOTE: The expectation is that EACH Package provides a type-safe (with
              respect to IDs) wrapper function that then calls this method.
     */
    virtual void vlogf( uint8_t     classificationId,
                        uint8_t     packageId,
                        uint8_t     subSystemId,
                        uint8_t     messageId,
                        const char* formatInfoText,
                        va_list     ap ) noexcept = 0;

public:
    /** This method is used to enable one or more log Classifications ID, i.e
        OR'd to the existing enabled/disabled mask.  Returns the previous
        enabled/disabled mask.

        Use the classificationIdToMask() method to get the mask for a specific
        Classification ID.

        Note: Upon initialization ALL Classifications are enabled
     */
    virtual KitLoggingClassificationMask_T enableClassification( KitLoggingClassificationMask_T classificationMask ) noexcept = 0;

    /** This method is used to disable one or more log Classifications ID, i.e
        clear bits in the existing enabled/disabled mask.  Returns the previous
        enabled/disabled mask.

        Use the classificationIdToMask() method to get the mask for a specific
        Classification ID.
     */
    virtual KitLoggingClassificationMask_T disableClassification( KitLoggingClassificationMask_T classificationMask ) noexcept = 0;

    /** This method is used to explicit set the classification mask (i.e. the enable/
        disable methods are OR/AND operations, this is assignment operation)

        The method is thread safe
     */
    virtual void setClassificationMask( KitLoggingClassificationMask_T newMask ) noexcept = 0;

    /** This method returns the current enabled/disabled Classification mask
     */
    virtual KitLoggingClassificationMask_T getClassificationEnabledMask() noexcept = 0;

public:
    /** This method is used to enable one or more log Package ID, i.e
        OR'd to the existing enabled/disabled mask.  Returns the previous
        enabled/disabled mask.

        Use the packageIdToMask() method to get the mask for a specific
        Package ID.

        Note: Upon initialization ALL Packages are enabled
     */
    virtual KitLoggingPackageMask_T enablePackage( KitLoggingPackageMask_T packageMask ) noexcept = 0;
    /** This method is used to disable one or more log Package ID, i.e
        clear bits in the existing enabled/disabled mask.  Returns the previous
        enabled/disabled mask.

        Use the packageIdToMask() method to get the mask for a specific
        Package ID.
     */
    virtual KitLoggingPackageMask_T disablePackage( KitLoggingPackageMask_T packageMask ) noexcept = 0;
    /** This method is used to explicit set the package mask (i.e. the enable/
        disable methods are OR/AND operations, this is assignment operation)

        The method is thread safe
     */
    virtual void setPackageMask( KitLoggingPackageMask_T newMask ) noexcept = 0;

    /** This method returns the current enabled/disabled Package mask
     */
    virtual KitLoggingPackageMask_T getPackageEnabledMask() noexcept = 0;

public:
    /** This method is used to convert a numeric Classification ID into a filter-mask.
        NOTE: If the classification ID is invalid (i.e. out of range) the method
        returns 0.
     */
    static KitLoggingClassificationMask_T classificationIdToMask( uint8_t classificationId ) noexcept;

    /** This method converts a Classification ID filter-mask to classification
        ID.

        NOTE: If an invalid filter-mask is provided, e.g. the ID value exceeds
              the number of bits in the Mask Type, then IApplication::NULL_CLASSIFICATION_ID
              is returned.
     */
    static uint8_t maskToClassificationId( KitLoggingClassificationMask_T classificationMask ) noexcept;

    /** This method is used to convert a numeric Package ID into a filter-mask.
        NOTE: If the package ID is invalid (i.e. out of range) the method
        returns 0.
     */
    static KitLoggingPackageMask_T packageIdToMask( uint8_t packageId ) noexcept;

    /** This method converts a Package ID filter-mask to package
        ID.

        NOTE: If an invalid filter-mask is provided, e.g. the ID value exceeds
              the number of bits in the Mask Type, then IPackage::NULL_ID
              is returned.
     */
    static uint8_t maskToPackageId( KitLoggingPackageMask_T packageMask ) noexcept;

public:
    /// Virtual destructor
    virtual ~ILog() noexcept {}
};

}  // end namespaces
}
}
#endif  // end header latch
