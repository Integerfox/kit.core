#ifndef KIT_TSHELL_ICOMMAND_H_
#define KIT_TSHELL_ICOMMAND_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/KeyedItem.h"
#include "Kit/TShell/ISecurity.h"
#include "Kit/Type/BetterEnum.h"

///
namespace Kit {
///
namespace TShell {

/** This enumeration defines the possible return codes when executing a TShell command.

@param Result_T             Enum
@param SUCCESS              Command executed successfully
@param ERROR_NOT_SUPPORTED  Unrecognized command or command is not supported in the current context
@param ERROR_NOT_AUTHORIZED Command is not authorized for the current user or context
@param ERROR_BAD_SYNTAX     The syntax of the command is invalid, e.g. missing required arguments, invalid arguments, out-of-range values, etc.
@param ERROR_IO             Command failed due to an error writing to the Output stream
@param ERROR_CMD_FAILED     Command failed to complete one or more of actions.
*/
// clang-format off
BETTER_ENUM( Result_T, uint8_t,
             SUCCESS,
             ERROR_NOT_SUPPORTED,
             ERROR_NOT_AUTHORIZED,
             ERROR_BAD_SYNTAX,
             ERROR_IO,
             ERROR_CMD_FAILED
);
// clang-format on

/// Forward class reference to avoid circular header includes
class IContext;


/** This abstract class defines the interface for a TShell Command.

    The interface is NOT thread safe and assumes all commands are executed in
    the same thread the IProcessor instance.
*/
class ICommand: public Kit::Container::KeyedItem
{
public:
    /// This method executes the command.
    virtual Result_T execute( IContext& context,
                              char*     rawCmdString ) noexcept = 0;

    /// This method returns the command's verb string
    virtual const char* getVerb() const noexcept = 0;

    /// This method returns the command's usage string
    virtual const char* getUsage() const noexcept = 0;

    /** This method returns the command's detailed help string.  Detailed
        help is optional.  If the command does not support detailed help,
        then nullptr is returned.
     */
    virtual const char* getHelp() const noexcept = 0;

    /// Returns the required permissions needed to execute the command
    virtual Permissions_T getRequiredPermissions() const noexcept = 0;

    public:
    /// Virtual destructor
    virtual ~ICommand() = default;
};


}  // end namespaces
}
#endif  // end header latch
