#ifndef KIT_SYSTEM_SHELL_H_
#define KIT_SYSTEM_SHELL_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


///
namespace Kit {
///
namespace System {

/** This class defines methods for interfacing with the Platform native OS to
    execute a native OS system/shell command.  Support for this interface is
    target/platform specific, i.e. support for this interface is OPTIONAL. If
    the target does not support this interface, then the 'execute' method
    does NOTHING (and returns -1); and the method isAvailable() return false.
 */
class Shell
{
public:
    /** Executes the System Shell command as specified by 'cmdstring'.  If
        noEchoStdOut is set to true then the standard output of the shell command
        is set to the system's NUL device.  If noEchoStdErr is set to true then
        the standard error output of the shell command is set to the system's
        NUL device. The method returns -1 if unable to execute the command,
        else returns the result code of the command executed.
     */
    static int execute( const char* cmdstring, bool noEchoStdOut = true, bool noEchoStdErr = true ) noexcept;

    /** Returns true if the system command shell is available.
     */
    static bool isAvailable() noexcept;
};

}  // end namespaces
}
#endif  // end header latch
