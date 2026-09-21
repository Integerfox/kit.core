#ifndef EXAMPLES_JOB_TEMPERATURE_EXAMPLE_H_
#define EXAMPLES_JOB_TEMPERATURE_EXAMPLE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/IInput.h"
#include "Kit/Io/IOutput.h"
#include "Kit/Container/OrderedList.h"
#include "Kit/TShell/ICommand.h"

///
namespace Examples {
///
namespace Job {
///
namespace Temperature {

/** Entry point for the Job Temperature example. Assumes that the KIT library
    has been initialized and the OSAL's scheduler is 'running'
 */
int runExample( Kit::Io::IInput& infd, Kit::Io::IOutput& outfd ) noexcept;


/// Expose the TShell command list to allow platform specific commands to be registered
extern Kit::Container::OrderedList<Kit::TShell::ICommand> g_commandList;


}  // end namespace(s)
}
}
#endif  // end header latch