#ifndef KIT_LOGGING_FRAMEWORK_TSHELL_VIEWER_H
#define KIT_LOGGING_FRAMEWORK_TSHELL_VIEWER_H
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
#include "Kit/Persistence/Record/Journal/IReader.h"
#include "Kit/Logging/Framework/IApplication.h"

///
namespace Kit {
///
namespace Logging {
///
namespace Framework {
///
namespace TShell {


/** This class implements a TShell command for View log entries stored in persistent
    storage.
 */
class Viewer : public Kit::TShell::Command::Base
{
public:
    /// Default command verb
    static constexpr const char* verb = "log";

    /// The command usage string
    static constexpr const char* usage =
        "log [*|<max>]\n"
        "log <skip> *\n"
        "log <skip> <max>\n"
        "log class\n"
        "log pkg";


    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                  1         2         3         4         5         6         7         8
         12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp =
        "  Displays stored log records entries and Classification/Package Info. Records\n"
        "  are always displayed newest entry first. The output format is:\n"
        "  \"[<entrykey>] (<entryTimestamp>) <PACKID>:<CATID>:<MSGID>. <text>\"\n"
        "  - 'log' with no arguments displays the newest entry.\n"
        "  - 'log *'        displays all entries.\n"
        "  - 'log <max>'    displays at most <max> entries.\n"
        "  - 'log <skip> *'     skips the first <skip> entries, the displays all\n"
        "                       remaining log entries.\n"
        "  - 'log <skip> <max>' skips the first <skip> entries, then displays at most\n"
        "                       <max> log entries.\n"
        "  - 'log class' displays classifications IDs and mask status.\n"
        "  - 'log pkg'   displays packages IDs and mask status.";

public:
    /// Constructor
    Viewer( Kit::Container::OrderedList<ICommand>&      commandList,
            Kit::Logging::Framework::IApplication&      appLogInfo,
            Kit::Persistence::Record::Journal::IReader& logReader,
            Kit::TShell::Permissions_T                  permissions = OPTION_KIT_TSHELL_SECURITY_DEFAULT_PERMISSIONS ) noexcept
        : Base( commandList, verb, permissions )
        , m_appLogInfo( appLogInfo )
        , m_logReader( logReader )
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
    Kit::Persistence::Record::Journal::IReader& m_logReader;
};

}  // end namespaces
}
}
}
#endif  // end header latch
