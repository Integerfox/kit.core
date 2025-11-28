#ifndef KIT_LOGGING_FRAMEWORK_MOCKED4TEST_KIT_ONLY_H_
#define KIT_LOGGING_FRAMEWORK_MOCKED4TEST_KIT_ONLY_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/Framework/IApplication.h"
#include "Kit/Logging/Pkg/Package.h"

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
class KitOnly : public Kit::Logging::Framework::IApplication
{
public:
    /// Constructor
    KitOnly() noexcept;

public:
    /// See Kit::Logging::Framework::IApplication
    const char* classificationIdToString( uint8_t classificationId ) noexcept override;

    /// See Kit::Logging::Framework::IApplication
    IPackage& getPackage( uint8_t packageId ) noexcept override;

public:
    /// The KIT Package instance (is public to allow unit test access)
    Kit::Logging::Pkg::Package m_kitPackage;

    /// The Log entry FIFO (is public to allow unit test access)
    // Kit::Container::RingBufferMP<Kit::Logging::EntryData_T> m_logQueue;

public:
    //---------------- WhiteBox support -----------------

    /// This method resets the 'internals' for a known default state, i.e. clears the log queue, resets the overflow count, etc.
    void reset() noexcept;

    /// Returns the number of currently queued logged entries
    // uint32_t getLogQueueCount() const noexcept;

    /// Clears all entries from the log queue
    // void clearLogQueue() noexcept;

    /// Returns the logging framework's 'queue full' status
    bool isLogQueueFull() const noexcept;

    /// Returns the current number of overflowed log entries
    uint32_t getOverflowedLogEntryCount() const noexcept;

    /// Returns the number of times Kit::Logging::Framework::Log::vlogf() was called
    uint32_t getLogCallCount() const noexcept;
};

}  // end namespaces
}
}
}
#endif  // end header latch