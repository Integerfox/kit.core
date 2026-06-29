#ifndef KIT_TSHELL_COMMAND_WAIT_H_
#define KIT_TSHELL_COMMAND_WAIT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/TShell/Command/Base.h"
#include "Kit/TShell/ISecurity.h"

///
namespace Kit {
///
namespace TShell {
///
namespace Command {

/// This class implements a TShell command
class Wait : public Base
{
public:
    /// The command verb/identifier
    static constexpr const char* verb = "wait";

    /// The command usage string
    static constexpr const char* usage = "wait <delay>";

    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                  1         2         3         4         5         6         7         8
         12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp =
        "  Waits for 'delay' milliseconds before returning.  This is useful when issuing\n"
        "  a batch of console commands and you need a delay between the execution of \n"
        "  commands.";

public:
    /// Constructor
    Wait( Kit::Container::OrderedList<ICommand>& commandList,
          Permissions_T                          permissions = OPTION_KIT_TSHELL_SECURITY_DEFAULT_PERMISSIONS ) noexcept
        : Base( commandList, verb, permissions )
    {
    }

public:
    /// See Kit::TShell::Command
    Result_T execute( IContext& context, char* cmdString ) noexcept override;

    /// See Kit::TShell::ICommand
    const char* getUsage() const noexcept override { return usage; }

    /// See Kit::TShell::ICommand
    const char* getHelp() const noexcept override { return detailedHelp; }
};


}  // end namespaces
}
}
#endif  // end header latch
