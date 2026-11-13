#ifndef KIT_TIME_TSHELL_TIME_H_
#define KIT_TIME_TSHELL_TIME_H_
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
namespace Time {
///
namespace TShell {

/** This class implements a TShell command that displays the current elapsed
    time, wall clock time, and boot time.

    NOTE: The boot time counter is only meaningful when the application has
          called Kit::Time::initializeBootTime(); else the counter is reported
          as zero.
 */
class Time : public Kit::TShell::Command::Base
{
public:
    /// The command verb/identifier
    static constexpr const char* verb = "time";

    /// The command usage string
    static constexpr const char* usage = "time";

    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                  1         2         3         4         5         6         7         8
         12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp =
        "  Displays elapsed time (ms), wall clock time (UTC formatted),\n"
        "  and boot time breakdown (boot counter and elapsed ms since boot).";

public:
    /// Constructor
    Time( Kit::Container::OrderedList<Kit::TShell::ICommand>& commandList,
          Kit::TShell::Permissions_T                          permissions = OPTION_KIT_TSHELL_SECURITY_DEFAULT_PERMISSIONS ) noexcept
        : Base( commandList, verb, permissions )
    {
    }

public:
    /// See Kit::TShell::ICommand
    Kit::TShell::Result_T execute( Kit::TShell::IContext& context, char* cmdString ) noexcept override;

    /// See Kit::TShell::ICommand
    const char* getUsage() const noexcept override { return usage; }

    /// See Kit::TShell::ICommand
    const char* getHelp() const noexcept override { return detailedHelp; }
};


}  // end namespaces
}
}
#endif  // end header latch
