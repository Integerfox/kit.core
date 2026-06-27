#ifndef KIT_TSHELL_COMMAND_BASE_H_
#define KIT_TSHELL_COMMAND_BASE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/TShell/ICommand.h"
#include "Kit/Container/OrderedList.h"
#include "Kit/TShell/IContext.h"

///
namespace Kit {
///
namespace TShell {
///
namespace Command {

/** This partially concrete class implements infrastructure and/or common
    functionality for a TShell command.

*/
class Base : public ICommand
{
protected:
    /// Constructor
    Base( Kit::Container::OrderedList<ICommand>& commandList,
          const char*                            verb,
          Permissions_T                          permissions ) noexcept
        : ICommand( verb )
        , m_permissions( permissions )
    {
        // Self-register the command with the list of commands
        commandList.insert( *this );
    }

public:
    /// See Kit::TShell::ICommand
    const char* getVerb() const noexcept override { return m_stringKeyPtr; }

    /// See Kit::TShell::ICommand
    Permissions_T getRequiredPermissions() const noexcept override { return m_permissions; }

    /// See Kit::Container::KeyedItem
    const Kit::Container::Key& getKey() const noexcept override { return *this; }

protected:
    /** Convience method that outputs the 'last frame' of the command's output
        and manages the command's final return/result value based on success or
        fail of writeFrame() operations.

        Returns CMD_SUCCESS on success, or CMD_ERR_IO if there was IO failure
     */
    inline Result_T writeLastFrame( IContext& context, const char* text, bool previousIoStatus = true ) noexcept
    {
        previousIoStatus &= context.writeFrame( text );
        return previousIoStatus ? Result_T::CMD_SUCCESS : Result_T::CMD_ERR_IO;
    }

protected:
    /// Security permissions required to execute the command
    Permissions_T m_permissions;
};


}  // end namespaces
}
}
#endif  // end header latch
