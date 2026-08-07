#ifndef KIT_LOGGING_FRAMEWORK_TSHELL_MANAGE_H
#define KIT_LOGGING_FRAMEWORK_TSHELL_MANAGE_H
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
#include "Kit/Persistence/Record/Journal/IReset.h"
#include "Kit/Logging/Framework/IApplication.h"

///
namespace Kit {
///
namespace Logging {
///
namespace Framework {
///
namespace TShell {


/** This class implements a TShell command for managing log entries stored in persistent
    storage.
 */
class Manage : public Kit::TShell::Command::Base
{
public:
    /// Default command verb
    static constexpr const char* verb = "logw";

    /// The command usage string
    static constexpr const char* usage =
        "logw CLEAR\n"
        "logw create <classId> <pkgid> <catid> <msgid> \"<text>\"";

    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                  1         2         3         4         5         6         7         8
         12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp =
        "  Logically clears all log records and creates log entries.";

public:
    /// Constructor
    Manage( Kit::Container::OrderedList<ICommand>&      commandList,
            Kit::Logging::Framework::IApplication&      appLogInfo,
            Kit::Persistence::Record::Journal::IReset& logReset,
            Kit::TShell::Permissions_T                  permissions = OPTION_KIT_TSHELL_SECURITY_DEFAULT_PERMISSIONS ) noexcept
        : Base( commandList, verb, permissions )
        , m_appLogInfo( appLogInfo )
        , m_logReset( logReset )
    {
    }

public:
    /// See Kit::TShell::Command
    Kit::TShell::Result_T execute( Kit::TShell::IContext& context, char* cmdString ) noexcept override;

    /// See Kit::TShell::ICommand
    const char* getUsage() const noexcept override { return usage; }

    /// See Kit::TShell::ICommand
    const char* getHelp() const noexcept override { return detailedHelp; }

protected:
    /// Application specific log information (used to get Package instances and Classification info)
    Kit::Logging::Framework::IApplication& m_appLogInfo;

    /// API to read log entries
    Kit::Persistence::Record::Journal::IReset& m_logReset;
};

}  // end namespaces
}
}
}
#endif  // end header latch
