#ifndef KIT_TSHELL_PROCESSOR_H_
#define KIT_TSHELL_PROCESSOR_H_
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
#include "Kit/Container/OrderedList.h"
#include "Kit/TShell/IProcessor.h"
#include "Kit/TShell/IContext.h"
#include "Kit/Framing/LineDecoder.h"
#include "Kit/Framing/EncoderWriter.h"
#include "Kit/Framing/StreamDestination.h"
#include "Kit/Framing/StreamSource.h"
#include "Kit/System/Mutex.h"
#include "Kit/Text/FString.h"

/** This symbol defines the size, in bytes, of the maximum allowed input
    string/command.
 */
#ifndef OPTION_KIT_TSHELL_PROCESSOR_INPUT_SIZE
#define OPTION_KIT_TSHELL_PROCESSOR_INPUT_SIZE 128
#endif

/** This symbol defines the size, in bytes, of the maximum allowed framed
    output string/command.
  */
#ifndef OPTION_KIT_TSHELL_PROCESSOR_OUTPUT_SIZE
#define OPTION_KIT_TSHELL_PROCESSOR_OUTPUT_SIZE 256
#endif

/** This symbols defines the Shell's greeting message
 */
#ifndef OPTION_KIT_TSHELL_PROCESSOR_GREETING
#define OPTION_KIT_TSHELL_PROCESSOR_GREETING "\n--- Your friendly neighborhood TShell. ---\n\n\n"
#endif

/** This symbols defines the Shell's farewell message
 */
#ifndef OPTION_KIT_TSHELL_PROCESSOR_FAREWELL
#define OPTION_KIT_TSHELL_PROCESSOR_FAREWELL "\n--- ...I am melting, am melting... ---\n\n"
#endif

/** This symbols defines the Shell's prompt string
 */
#ifndef OPTION_KIT_TSHELL_PROCESSOR_PROMPT
#define OPTION_KIT_TSHELL_PROCESSOR_PROMPT "$ "
#endif


///
namespace Kit {
///
namespace TShell {


/** This concrete class provides the implementation of Command Processor for
    a TShell engine.

    The implementation assumes a single threaded model, i.e. the Command
    Processor and all of its  commands run in a single thread.  It is
    APPLICATION's responsibility to provide any desired multi-threaded
    support. There are two caveats to the single-threaded model:

        o The output of the commands are mutex protected.  This allows the
          Output stream to be 'shared' with other sub-systems and/or
          threads (e.g. the shell shares the same Output stream as the
          Kit::System::Trace logging output).

        o The stop() method can be called safely from other threads.


    Commands have the following syntax:

        o A command starts with a printable ASCII character and ends with
          a newline.

        o Non-printable ASCII characters are not allowed.

        o Any line that starts with a '#' is treated as comment line and
          is ignored.

        o Format of a command is: verb [arg]*  where the verb and arguments
          are separated by spaces.  Arguments can contain spaces character
          by enclosing the argument with double quote characters.  A double
          quote character can be embedded inside a quoted string by preceding
          it the double quote character with the escape character.  The escape
          character can be embedded by escaping the escape character.

    HOW TO Enable Security:

        - Set OPTION_KIT_TSHELL_SECURITY_DEFAULT_PERMISSIONS to something
          other than zero.  This is the default constructor value for all KIT
          TShell commands.

        - Provide an implementation (other than `NoSecurity`) of the ISecurity
          interface that meets the application's security requirements.
*/
class Processor : public IProcessor, public IContext
{
    /** Constructor.

    @param commands                 Set of supported commands
    @param commandSource            Framing source for the decoder used to identify
                                    individual command strings within the raw Input
                                    stream
    @param outputDestination        Framing destination used to encapsulate the output
                                    of command in the Output stream.
    @param securityPolicy           Security policy for the shell commands.
    @param outputLock               Mutex to be used for ensuring the atomic output
                                    of the commands.
    @param convertTabs              If set to a value other than a tab character, then
                                    any tab characters in the input will be converted
                                    to this character. Default to convert tabs to a
                                    space character.
    @param commentChar              The comment character used to indicate that the
                                    input string is a comment and should not be
                                    executed.
    @param argEscape                Escape character to be used when escaping double
                                    quote characters inside a quoted argument.
    @param argDelimiter             The delimiter character used to separate the
                                    command verb and arguments.
    @param argQuote                 The quote character used to 'double quote' a
                                    argument string.
    @param argTerminator            The command terminator character.
 */
    Processor( Kit::Container::OrderedList<ICommand>& commands,
               Kit::Framing::StreamSource&            commandSource,
               Kit::Framing::StreamDestination&       outputDestination,
               ISecurity&                             securityPolicy,
               Kit::System::Mutex&                    outputLock,
               char                                   convertTabs   = ' ',
               char                                   commentChar   = '#',
               char                                   argEscape     = '`',
               char                                   argDelimiter  = ' ',
               char                                   argQuote      = '"',
               char                                   argTerminator = '\n' );
               
public:
    /// See Kit::TShell::IProcessor
    bool start( Kit::Io::IInput&  infd,
                Kit::Io::IOutput& outfd,
                bool              blocking = true ) noexcept override;

    /// See Kit::TShell::IProcessor
    int poll() noexcept override;

    /// See Kit::TShell::IProcessor
    void requestStop() noexcept override;

public:
    /// See Kit::TShell::IContext
    Kit::Container::OrderedList<ICommand>& getCommands() noexcept override;

    /// See Kit::TShell::IContext
    ICommand* findCommand( const char* verb, unsigned verbLength ) noexcept override;

    /// See Kit::TShell::IContext
    bool writeFrame( const char* text ) noexcept override;

    /// See Kit::TShell::IContext
    bool writeFrame( const char* text, Kit::Type::SSize_T maxBytes ) noexcept override;

    /// See Kit::TShell::IContext
    Kit::Text::IString& getOutputBuffer() noexcept override;

    /// See Kit::TShell::IContext
    Kit::Text::IString& getInputBuffer() noexcept override;

    /// See Kit::TShell::IContext
    Kit::Text::IString& getWorkBuffer() noexcept override;

    /// See Kit::TShell::IContext
    bool oobRead( void*               dstBuffer,
                  Kit::Type::SSize_T  numBytes,
                  Kit::Type::SSize_T& bytesRead ) noexcept override;

    /// See Kit::TShell::IContext
    char getEscapeChar() noexcept override;

    /// See Kit::TShell::IContext
    char getDelimiterChar() noexcept override;

    /// See Kit::TShell::IContext
    char getQuoteChar() noexcept override;

    /// See Kit::TShell::IContext
    char getTerminatorChar() noexcept override;

    /// See Kit::TShell::IContext
    ISecurity& getSecurity() noexcept override;

protected:
    /// List of supported commands
    Kit::Container::OrderedList<ICommand>& m_commands;

    /// Decoder to identify individual command strings within the raw Input stream
    Kit::Framing::LineDecoder m_deframer;

    /// Encoder to encapsulate the output of command in the Output stream
    Kit::Framing::EncoderWriter m_framer;

    /// Security policy for the shell commands
    ISecurity& m_secPolicy;

    /// Output lock
    Kit::System::Mutex& m_outLock;

    /// Current frame size
    Kit::Type::SSize_T m_frameSize;

    /// Shared work buffer
    Kit::Text::FString<OPTION_KIT_TSHELL_PROCESSOR_OUTPUT_SIZE> m_workOutputBuffer;

    /// Shared work buffer
    Kit::Text::FString<OPTION_KIT_TSHELL_PROCESSOR_INPUT_SIZE> m_workInputBuffer;

    /// Shared work buffer
    Kit::Text::FString<OPTION_KIT_TSHELL_PROCESSOR_INPUT_SIZE> m_workBuffer;

    /// Input Frame buffer
    char m_inputBuffer[OPTION_KIT_TSHELL_PROCESSOR_INPUT_SIZE + 1];

    /// Comment character
    char m_comment;

    /// Argument Escape character
    char m_esc;

    /// Argument delimiter
    char m_del;

    /// Argument quote character
    char m_quote;

    /// Argument terminator character
    char m_term;

    /// My run state
    bool m_running;
};


}  // end namespaces
}
#endif  // end header latch
