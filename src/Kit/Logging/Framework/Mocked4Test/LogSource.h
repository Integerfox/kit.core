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
class LogSource : public Kit::Logging::Framework::LogSource, public Kit::Logging::Framework::IApplication
{
public:
    /// Constructor
    LogSource( uint8_t     classificationIdForQueueOverflow,
               const char* classificationTextForQueueOverflow,
               uint8_t     packageIdForQueueOverflow,
               const char* packageTextForQueueOverflow,
               uint8_t     subSystemIdForQueueOverflow,
               const char* subSystemTextForQueueOverflow ) noexcept;


public:
    /// See Kit::Logging::Framework::IApplication
    const char* classificationIdToString( uint8_t classificationId ) noexcept override;

    /// See Kit::Logging::Framework::IApplication
    IPackage& getPackage( uint8_t packageId ) noexcept override;

    protected:
    /// Log entry FIFO
    // Kit::Container::RingBufferMP<EntryData_T>& m_logFifo;

};


}  // end namespaces
}
}
#endif  // end header latch