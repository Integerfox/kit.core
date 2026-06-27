#ifndef KIT_TSHELL_COMMAND_BYE_H_
#define KIT_TSHELL_COMMAND_BYE_H_
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
class Bye : public Base
{
public:
    /// The command verb/identifier
    static constexpr const char* verb = "bye";

    /// The command usage string
    static constexpr const char* usage =
        "bye [<exitcode>]\n"
        "bye tshell";

    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                  1         2         3         4         5         6         7         8
         12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp =
        "  Requests to exit the application. The default <exitcode> is '0'. Using the\n"
        "  the 'tshell' argument only exits the TShell.  NOTE: The <exitcode> and\n"
        "  what happens when the only the TShell is exited is application specific";

protected:
    /// Constructor
    Bye( Kit::Container::OrderedList<ICommand>& commandList,
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
