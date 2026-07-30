#ifndef KIT_TSHELL_COMMAND_THREADS_H_
#define KIT_TSHELL_COMMAND_THREADS_H_
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
#include "Kit/System/Thread.h"

///
namespace Kit {
///
namespace TShell {
///
namespace Command {

/// This class implements a TShell command
class Threads : public Base, public Kit::System::Thread::ITraverser
{
public:
    /// The command verb/identifier
    static constexpr const char* verb = "threads";

    /// The command usage string
    static constexpr const char* usage =
        "threads";

    /** The command detailed help string (recommended that lines do not exceed 80 chars)
                  1         2         3         4         5         6         7         8
         12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* detailedHelp =
        "  Displays the list of KIT threads.";

public:
    /// Constructor
    Threads( Kit::Container::OrderedList<ICommand>& commandList,
             Permissions_T                          permissions = OPTION_KIT_TSHELL_SECURITY_DEFAULT_PERMISSIONS ) noexcept
        : Base( commandList, verb, permissions )
        , m_contextPtr( nullptr )
        , m_count( 0 )
        , m_io( true )
    {
    }

public:
    /// See Kit::TShell::Command
    Result_T execute( IContext& context, char* cmdString ) noexcept override;

    /// See Kit::TShell::ICommand
    const char* getUsage() const noexcept override { return usage; }

    /// See Kit::TShell::ICommand
    const char* getHelp() const noexcept override { return detailedHelp; }

public:
    /// See Kit::System::Thread::ITraverser
    Kit::Type::TraverserStatus item( Kit::System::Thread& nextThread ) noexcept override;

protected:
    /// Hook to allow derived classes to add additional columns to the header 
    virtual void hookHeaderTitle( Kit::Text::IString& text );

    /// Hook to allow derived classes to add additional columns to the thread entry
    virtual void hookThreadEntry( Kit::Text::IString& text, Kit::System::Thread& currentThread );

protected:
    /// Cache my Processor/Shell context when traversing the thread list
    Kit::TShell::IContext* m_contextPtr;

    /// Cache thread count
    unsigned m_count;

    /// Cache IO status/errors
    bool m_io;
};


}  // end namespaces
}
}
#endif  // end header latch
