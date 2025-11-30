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

#include "Kit/Logging/Framework/NullPackage.h"
#include "kit_config.h"
#include "Kit/Logging/Framework/IApplication.h"
#include "Kit/Logging/Framework/EntryData.h"
#include "Kit/Logging/Pkg/Package.h"
#include "Kit/Container/RingBufferAllocate.h"  // TODO: Needs to be Kit::Container::RingBufferAllocateMP

/// Define the maximum number of log entries in the FIFO used for unit testing.
/// Note: The actually allocated FIFO size will be this value + 1 (to support
///       the ring buffer implementation)
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_MOCKED4TEST_MAX_LOG_ENTRIES
#define OPTION_KIT_LOGGING_FRAMEWORK_MOCKED4TEST_MAX_LOG_ENTRIES 5
#endif

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
    bool isClassificationIdValid( uint8_t classificationId ) noexcept override;

    /// See Kit::Logging::Framework::IApplication
    bool isPackageIdValid( uint8_t packageId ) noexcept override;

    /// See Kit::Logging::Framework::IApplication
    const char* classificationIdToString( uint8_t classificationId ) noexcept override;

    /// See Kit::Logging::Framework::IApplication
    IPackage& getPackage( uint8_t packageId ) noexcept override;

protected:
    /// Internal Log entry FIFO storage
    Kit::Logging::Framework::EntryData_T* m_logFifoStorage;

    /// Null Package instance (for unsupported Package ID requests)
    Kit::Logging::Framework::NullPackage m_nullPkg;

    public:
    /// The KIT Package instance (is public to allow unit test access)
    Kit::Logging::Pkg::Package m_kitPackage;


    /// The Log entry FIFO (is public to allow unit test access). TODO: Needs to be Kit::Container::RingBufferMP
    Kit::Container::RingBufferAllocate<Kit::Logging::Framework::EntryData_T, OPTION_KIT_LOGGING_FRAMEWORK_MOCKED4TEST_MAX_LOG_ENTRIES + 1> m_logFifo;


public:
    //---------------- WhiteBox support -----------------

    /// This method resets the 'internals' for a known default state, i.e. clears the log queue, resets the overflow count, etc.
    void reset() noexcept;

    /// Returns the number of currently queued logged entries
    uint32_t getLogQueueCount() const noexcept { return m_logFifo.getNumItems(); }

    /// Clears all entries from the log queue
    void clearLogQueue() noexcept { m_logFifo.clearTheBuffer(); }

    /// Returns the logging framework's 'queue overflowed' status
    bool isLogQueOverflowed() const noexcept;

    /// Returns the current number of overflowed log entries
    uint32_t getOverflowedLogEntryCount() const noexcept;

    /// Returns the number of times Kit::Logging::Framework::Log::vlogf() was called
    uint32_t getVLogfCallCount() const noexcept;
};

}  // end namespaces
}
}
}
#endif  // end header latch