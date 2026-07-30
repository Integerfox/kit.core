#ifndef KIT_TSHELL_COMMAND_FREERTOS_THREADS_H_
#define KIT_TSHELL_COMMAND_FREERTOS_THREADS_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/TShell/Command/Threads.h"

///
namespace Kit {
///
namespace TShell {
///
namespace Command {
///
namespace FreeRTOS {

/** This class implements a FreeRTOS specific TShell command for threads
 */
class Threads : public Kit::TShell::Command::Threads
{
public:
    /// Constructor
    Threads( Kit::Container::OrderedList<ICommand>& commandList,
             Permissions_T                          permissions = OPTION_KIT_TSHELL_SECURITY_DEFAULT_PERMISSIONS ) noexcept
        : Kit::TShell::Command::Threads( commandList, permissions )
    {
    }

protected:
    /// See Kit::TShell::Command::Threads
    void hookHeaderTitle( Kit::Text::IString& text ) override;

    /// See Kit::TShell::Command::Threads
    void hookThreadEntry( Kit::Text::IString& text, Kit::System::Thread& currentThread ) override;
};


}  // end namespaces
}
}
}
#endif  // end header latch
