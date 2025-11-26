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

/** This methods returns the number of times Kit::Logging::Framework::ILog::vlogf()
    was called.
 */
uint32_t getLogEntryCount() noexcept;

/** This method resets the log entry count to zero.
 */
void resetLogEntryCount() noexcept;

/** Method provides a 'mocked' IApplication() instance.  The instance
    supports a single 'Logging Package' --> the KIT Library
 */
Kit::Logging::Framework::IApplication& getMockedKitLibraryApplication() noexcept;

}  // end namespaces
}
}
}
#endif  // end header latch
