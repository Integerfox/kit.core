#ifndef KIT_TSHELL_ICONTEXT_H_
#define KIT_TSHELL_ICONTEXT_H_
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
#include "Kit/Text/IString.h"
#include "Kit/Type/SSize.h"
#include "Kit/Io/IOutput.h"

///
namespace Kit {
///
namespace TShell {


/** This Private Namespace abstract class defines a "Context" for a TShell
    command. The Context provide common infrastructure, information, buffers,
    etc. that facilitates interaction between the ICommand Processor and individual
    commands.  The application SHOULD NEVER directly access this interface.
*/
class IContext
{
public:
    /// This method returns the list of implemented commands
    virtual Kit::Container::OrderedList<ICommand>& getCommands() noexcept = 0;

    /// Lookup a command by its verb.  Returns a nullptr if the command is not found
    virtual ICommand* findCommand( const char* verb, unsigned verbLength ) noexcept = 0;

public:
    /** This method encodes and outputs the specified message/text. The method
        returns false if there was Output Stream error.

        NOTE: An ENTIRE frame must be written with a single call to this method
              to avoid interleaving of output frames with console trace/logging.
    */
    inline bool writeFrame( const char* text ) noexcept
    {
        return writeFrame( text, strlen( text ) );
    }

    /** Same as writeFrame(), but only outputs (at most) 'N' bytes as the content
        of the frame
    */
    virtual bool writeFrame( const char* text, Kit::Type::SSize_T maxBytes ) noexcept = 0;

public:
    /** This method returns a working buffer for a command to format its
        output prior to 'writing the frame'.  The buffer is only 'valid' for
        duration of the command's execute() method. The string buffer is
        guaranteed to be at least size of the largest supported output frame.
     */
    virtual Kit::Text::IString& getOutputBuffer() noexcept = 0;

    /** This method returns a 'working' buffer for use by a command when
        parsing input arguments.  The buffer is only 'valid' for
        duration of the command's execute() method. The string buffer is
        guaranteed to be at least size of the largest supported input frame.
     */
    virtual Kit::Text::IString& getInputBuffer() noexcept = 0;

    /** This method returns a 'working' buffer for use by a command.  The buffer
        is only 'valid' for duration of the command's execute() method. The
        buffer is guaranteed to be at least size the same size as the 'Input
        buffer'
    */
    virtual Kit::Text::IString& getWorkBuffer() noexcept = 0;

public:
    /** This method allows a command access to the input stream, i.e. consume
        additional input data that is 'out-of-band' of the nominal newline
        delineated command syntax.

        Attempts to read the specified number of bytes from the stream into the
        supplied buffer.  The actual number of bytes read is returned via
        'bytesRead'. Returns true if successful, or false if End-of-Stream
        was encountered.
     */
    virtual bool oobRead( void*               dstBuffer,
                          Kit::Type::SSize_T  numBytes,
                          Kit::Type::SSize_T& bytesRead ) noexcept = 0;

public:
    /// Returns reference to the Processor's Security instance
    virtual ISecurity& getSecurity() noexcept = 0;

public:
    /** Returns a pointer to the Processor's output stream.  If the TShell is
        has not been started, then nullptr is returned. 
     */
    virtual Kit::Io::IOutput* getOutputStream() noexcept = 0;

public:
    /** Request to terminated/exit the TShell.  When/if the TShell is exited
        is application specific behavior.
     */
    virtual void requestTShellExit() noexcept = 0;

public:
    /// Virtual destructor
    virtual ~IContext() = default;
};


}  // end namespaces
}
#endif  // end header latch
