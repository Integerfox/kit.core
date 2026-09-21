#ifndef KIT_JOB_TSHELL_CMD_H
#define KIT_JOB_TSHELL_CMD_H
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "Kit/TShell/Command/Base.h"
#include "Kit/Job/IManager.h"

/// Maximum number of Job instances that can be queried at once
#ifndef OPTION_KIT_JOB_TSHELL_MAX_INSTANCES
#define OPTION_KIT_JOB_TSHELL_MAX_INSTANCES 8
#endif

///
namespace Kit {
///
namespace Job {
///
namespace TShell {


/** This class implements a TShell command for managing IJobs at runtime. There
    should one instance of this class per instances of concrete IManager instances.
 */
class Cmd : public Kit::TShell::Command::Base
{
public:
    /// Default command verb
    static constexpr const char* defaultVerb = "jobs";

    /// The command usage string
    static constexpr const char* usage =
        "jobs\n"
        "jobs start <jobname> [<args...]\n"
        "jobs stop <jobname>|ALL\n"
        "jobs ls\n"
        "jobs help <jobname>";

    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                  1         2         3         4         5         6         7         8
         12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp =
        "  Asynchronously starts/stops individual Jobs (micro applications). Issuing\n"
        "  the command without arguments displays the list of running Jobs. The\n"
        "  'ls' subcommand lists all available Jobs.";

public:
    /// Constructor
    Cmd( Kit::Container::OrderedList<ICommand>& commandList,
         Kit::Job::IManager&                    jobManagerInstance,
         const char*                            verb        = defaultVerb,
         Kit::TShell::Permissions_T             permissions = OPTION_KIT_TSHELL_SECURITY_DEFAULT_PERMISSIONS ) noexcept
        : Base( commandList, verb, permissions )
        , m_jobManager( jobManagerInstance )
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
    /// Model Point Database to access
    Kit::Job::IManager& m_jobManager;

    /// Array for holding pointers to queried Job instances
    Kit::Job::IJob* m_jobList[OPTION_KIT_JOB_TSHELL_MAX_INSTANCES];
};

}  // end namespaces
}
}
#endif  // end header latch
