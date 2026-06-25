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

#include "Kit/TShell/ICommand.h"
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

/// Size, in bytes, of internal raw-read-buffer.  The buffer size does NOT limit the frame size
#ifndef OPTION_KIT_TSHELL_PROCESSOR_RAW_READ_BUFFER_SIZE
#define OPTION_KIT_TSHELL_PROCESSOR_RAW_READ_BUFFER_SIZE 16
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
public:
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
               char                                   argTerminator = '\n' )
        : m_commands( commands )
        , m_deframer( commandSource, m_rawInputBuffer, sizeof( m_rawInputBuffer ), convertTabs )
        , m_framer( outputDestination, argTerminator+1, argTerminator, argEscape, true ) // Line-based output does not require an SOF marker; use an arbitrary distinct SOF and skip transmitting it.
        , m_secPolicy( securityPolicy )
        , m_outLock( outputLock )
        , m_streamSource( commandSource )
        , m_streamDestination( outputDestination )
        , m_frameSize( 0 )
        , m_comment( commentChar )
        , m_esc( argEscape )
        , m_del( argDelimiter )
        , m_quote( argQuote )
        , m_term( argTerminator )
        , m_blocking( true )
        , m_running( false )
    {
        KIT_SYSTEM_ASSERT( commentChar >= ' ' && commentChar <= '~' );
        KIT_SYSTEM_ASSERT( argEscape >= ' ' && argEscape <= '~' );
        KIT_SYSTEM_ASSERT( argDelimiter >= ' ' && argDelimiter <= '~' );
        KIT_SYSTEM_ASSERT( argQuote >= ' ' && argQuote <= '~' );
        KIT_SYSTEM_ASSERT( argTerminator >= ' ' && argTerminator <= '~' );
    }

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
    /** Helper method that attempts to execute the content of the de-framed/decoded
        'inputString'.  The method returns the result code of the execute
        command.  If 'inputString' is not a valid command, then the appropriate
        error/result code is returned.
     */
    virtual Result_T processCommand( char* deframedInput ) noexcept;

    /// Helper method
    void outputCommandError( Result_T result, const char* deframedInput ) noexcept;

    /** Helper method that performs a 'single' read cycle of the input stream.
        Returns 0 if successful. Returns 1 if exiting. Returns -1 on error
     */
    int getAndProcessFrame() noexcept;

    /** Helper method that executes the decoder, i.e. logic to parse the incoming
        text.  Returns 1 if a full/valid frame was found. Returns 0 if input frame
        is incomplete. Return -1 if an error occurred.
     */
    virtual int readInput( Kit::Type::SSize_T& frameSize ) noexcept;

    /// Helper method.  Outputs text without any framing
    void outputMessage( const char* textString ) noexcept;

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

    /// Reference to the LineDecoder's input source
    Kit::Framing::StreamSource& m_streamSource;

    /// Reference to the EncoderWriter's output destination
    Kit::Framing::StreamDestination& m_streamDestination;

    /// Current frame size
    Kit::Type::SSize_T m_frameSize;

    /// Shared work buffer
    Kit::Text::FString<OPTION_KIT_TSHELL_PROCESSOR_OUTPUT_SIZE> m_workOutputBuffer;

    /// Shared work buffer
    Kit::Text::FString<OPTION_KIT_TSHELL_PROCESSOR_INPUT_SIZE> m_workInputBuffer;

    /// Shared work buffer
    Kit::Text::FString<OPTION_KIT_TSHELL_PROCESSOR_INPUT_SIZE> m_workBuffer;

    /// Buffer for reading the raw input (does NOT limit the size of the input frame)
    uint8_t m_rawInputBuffer[OPTION_KIT_TSHELL_PROCESSOR_RAW_READ_BUFFER_SIZE];

    /// Input Frame buffer (used to store the deframed input command)
    uint8_t m_frameBuffer[OPTION_KIT_TSHELL_PROCESSOR_INPUT_SIZE + 1];

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

    /// Blocking semantic flag.  When true the read operations are blocking until a complete frame is found
    bool m_blocking;

    /// My run state
    bool m_running;
};


}  // end namespaces
}
#endif  // end header latch
