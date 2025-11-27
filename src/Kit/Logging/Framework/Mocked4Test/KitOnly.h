#ifndef KIT_LOGGING_FRAMEWORK_MOCKED4TEST_LOGSOURCE_H_
#define KIT_LOGGING_FRAMEWORK_MOCKED4TEST_LOGSOURCE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/Framework/LogSource.h"
#include "Kit/Logging/Pkg/ClassificationId.h"
#include "Kit/Logging/Pkg/SubSystemId.h"
#include "Kit/Logging/Pkg/Package.h"
#include "Kit/Logging/Framework/Mocked4Test/WhiteBox.h"
#include "Kit/Logging/Pkg/SubSystemId.h"
#include "Kit/Logging/Pkg/SystemMsgId.h"
///
namespace Kit {
///
namespace Logging {
///
namespace Framework {
///
namespace Mocked4Test {

/** This concrete class implements the ILog interface, provides a concrete
    IApplication instance, and the Log entry FIFO. The IApplication instance
    ONLY supports a single Logging Package -->the KIT library
    */
class KitOnly : public Kit::Logging::Framework::IApplication, public Kit::Logging::Framework::LogSource, public WhiteBox
{
public:
    /// Constructor
    KitOnly() noexcept
        : LogSource( *this,
                     /* m_logFifo, */
                     Kit::Logging::Pkg::ClassificationId::WARNING,
                     Kit::Logging::Pkg::Package::PACKAGE_ID,
                     Kit::Logging::Pkg::SubSystemId::SYSTEM,
                     Kit::Logging::Pkg::SystemMsgId::LOGGING )
    {
    }

public:
    /// See Kit::Logging::Framework::IApplication
    const char*
    classificationIdToString( uint8_t classificationId ) noexcept override
    {
        return Kit::Type::betterEnumToString<Kit::Logging::Pkg::ClassificationId, uint8_t>(
            classificationId,
            OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_CLASSIFICATION_ID_TEXT );
    }

    /// See Kit::Logging::Framework::IApplication
    IPackage& getPackage( uint8_t packageId ) noexcept override
    {
        // Only support the KIT Package
        return m_kitPackage;
    }

public:
    /// The KIT Package instance
    Kit::Logging::Pkg::Package m_kitPackage;

public:
    //---------------- WhiteBox support -----------------

    /// Intercept to collect called metrics
    void vlogf( uint8_t     classificationId,
                uint8_t     packageId,
                uint8_t     subSystemId,
                uint8_t     messageId,
                const char* formatInfoText,
                va_list     ap ) noexcept override
    {
        m_logEntryCount++;
        LogSource::vlogf( classificationId,
                          packageId,
                          subSystemId,
                          messageId,
                          formatInfoText,
                          ap );
    }

    /// See Kit::Logging::Framework::Mocked4Test::WhiteBox
    bool isLogQueueFull() const noexcept override { return m_queueFull; }

    /// See Kit::Logging::Framework::Mocked4Test::WhiteBox
    uint32_t getOverflowedLogEntryCount() const noexcept override { return m_overflowCount; }
};

}  // end namespaces
}
}
}
#endif  // end header latch