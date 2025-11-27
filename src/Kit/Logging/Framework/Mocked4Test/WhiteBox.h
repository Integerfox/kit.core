#ifndef KIT_LOGGING_FRAMEWORK_MOCK4TEST_MOCKED_H_
#define KIT_LOGGING_FRAMEWORK_MOCK4TEST_MOCKED_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file provides test support when mocking the Kit::Logging engine
*/

#include "Kit/Logging/Framework/IApplication.h"


///
namespace Kit {
///
namespace Logging {
///
namespace Framework {
///
namespace Mocked4Test {

/** This class defines an interface that unit test can use to have a peering
    the mocked logging framework internals.  This class requires a child
    class, i.e. inheritted by the class actually implementing the mocked logging
    framework.

    The data members are public to simplify unit test access.
*/
class WhiteBox
{
protected:
    /// Constructor
    WhiteBox() = default;

public:
    /// Resets all of the data members to their default values.
    void reset() noexcept
    {
        m_logEntryCount = 0;
    }

    /// Provides access to the Log FIFO queue.  CAUTION: Only modifying the FIFO if you REALLY KNOW what you are doing!
    // Kit::Container::RingBufferMP<Kit::Logging::EntryData_T>& getLogQueue();

    /// Returns the logging engine's 'queue full' status
    virtual bool isLogQueueFull() const noexcept = 0;

    /// Returns the current number of overflowed log entries
    virtual uint32_t getOverflowedLogEntryCount() const noexcept = 0;

public:
    /// The number of times Kit::Logging::Framework::ILog::vlogf() was called
    uint32_t m_logEntryCount = 0;
};


}  // end namespaces
}
}
}
#endif  // end header latch
