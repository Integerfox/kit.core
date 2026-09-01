#ifndef KIT_DM_TSHELL_READ_H
#define KIT_DM_TSHELL_READ_H
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
#include "Kit/Dm/IModelDatabase.h"

///
namespace Kit {
///
namespace Dm {
///
namespace TShell {


/** This class implements a TShell command for Read operations on the Data
    Model. MANY instances of this command can be created, i.e. one instance per
    Data Model Database instance.  Each instance must have a different 'verb'.
    The default verb is 'dmr'.
 */
class Read : public Kit::TShell::Command::Base
{
public:
    /// Default command verb
    static constexpr const char* defaultVerb = "dmr";

    /// The command usage string
    static constexpr const char* usage =
        "dmr [- <filter>]\n"
        "dmr <mpname>";

    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                  1         2         3         4         5         6         7         8
         12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp =
        "  Lists and displays Model Points contained in the Model Database. When no\n"
        "  arguments are provided, the command lists all available model points.  The\n"
        "  <filter> argument will only list points that contain <filter>.  To display a\n"
        "  Model Point the <mpname> argument is the symbolic name of the Model Point\n"
        "  instance.";

public:
    /// Constructor
    Read( Kit::Container::OrderedList<ICommand>& commandList,
          Kit::Dm::IModelDatabase&               modelPointDatabaseInstance,
          const char*                            verb        = defaultVerb,
          Kit::TShell::Permissions_T             permissions = OPTION_KIT_TSHELL_SECURITY_DEFAULT_PERMISSIONS ) noexcept
        : Base( commandList, verb, permissions )
        , m_database( modelPointDatabaseInstance )
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
    Kit::Dm::IModelDatabase& m_database;
};

}  // end namespaces
}
}
#endif  // end header latch
