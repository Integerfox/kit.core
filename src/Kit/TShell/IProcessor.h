#ifndef KIT_TSHELL_IPROCESSOR_H_
#define KIT_TSHELL_IPROCESSOR_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/IInput.h"
#include "Kit/Io/IOutput.h"


///
namespace Kit {
///
namespace TShell {


/** This abstract class defines the interface a TShell Command Processor.  The
    Command Processor is responsible for scanning the input, determining
    what command (if any) to execute; and then executing the command.

    The design of the interface supports running the Command Processor with
    blocking-thread and with cooperative scheduling semantics.
*/
class IProcessor
{
public:
    /** This method is used to start the Command Processor, i.e. it will
        begin to process commands.

        When 'blocking' is set to true, the method command will not return until
        the Command Processor self terminates or a Input/Output stream error was
        encounter.  The method returns true if the Command Processor self
        terminated or was requested to stop; else false is returned e.g. a
        Input/Output stream error was encounter).

        When 'blocking' is set to false, the command processor will be started
        and the method returns immediately.  After start() is the called the
        Application is responsible for calling poll() to provide the command
        processor with CPU time to process commands. The method returns true if
        the Command Processor was successfully started; else false is returned
        (e.g. a Input/Output stream error was encounter).

        NOTE: This method is an 'in-thread' initialization, i.e. not thread safe.
              The application is RESPONSIBLE for managing threading issues.
    */
    virtual bool start( Kit::Io::IInput&  infd,
                        Kit::Io::IOutput& outfd,
                        bool              blocking = true ) noexcept = 0;


    /** This method is used to provide the command processor 'CPU cycles' to
        parse/process/execute commands.  This command should ONLY be called when
        the application called the start() method with the 'blocking' argument
        set to false.  This method should be called as often as possible, e.g.
        every pass of the 'main loop'

        NOTE: This method must ALWAYS be called from the same 'thread context'
              that called the start() method (i.e. not thread safe).

        The method returns 0 if the method executed without errors.  The method
        returns -1 if an error occurred (e.g. a Input/Output stream error was
        encounter).  The method returns 1 if the command processor self terminated,
        not started, or was requested to stop.
     */
    virtual int poll() noexcept = 0;

    /** This non-blocking method requests the Command Processor to stop.  When
        (or if) the Command Processor actually stops depends on the target's
        implementation, health of the Shell, current command(s) executing, etc.
        This method returns immediately.  There is no feedback/confirmation
        when the Command Processor stops.

        This command IS thread safe and can be called from any thread context.
     */
    virtual void requestStop() noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IProcessor() = default;
};


}  // end namespaces
}
#endif  // end header latch
