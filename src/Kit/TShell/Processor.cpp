/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Processor.h"
#include "Kit/System/GlobalLock.h"
#include "Kit/System/Api.h"
#include "Kit/System/Mutex.h"
#include "Kit/Text/Strip.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace TShell {

bool Processor::start( Kit::Io::IInput&  infd,
                       Kit::Io::IOutput& outfd,
                       bool              blocking ) noexcept
{
    // Housekeeping
    m_running  = true;
    m_blocking = blocking;
    m_streamSource.setInput( infd );
    m_streamDestination.setOutput( outfd );

    // Output the greeting message and initial prompt
    outputMessage( OPTION_KIT_TSHELL_PROCESSOR_GREETING );
    outputMessage( OPTION_KIT_TSHELL_PROCESSOR_PROMPT);

    // Run until I am requested to stop (or run once if using non-blocking semantics)
    do
    {
        int result = getAndProcessFrame();
        if ( result != 0 )
        {
            return result >= 0 ? true : false;
        }
    }
    while ( blocking );


    // If I get here, then I am running in non-blocking mode and have processed all available input
    return true;
}

int Processor::poll() noexcept
{
    // Check if I am running
    if ( !m_running )
    {
        return 1;
    }

    // Validate there is a underlying output stream
    if ( m_streamDestination.getStream() == nullptr || m_blocking )  // Polling is only valid when running in non-blocking mode
    {
        return -1;
    }

    // non-blocking read and frame decoding
    return getAndProcessFrame();
}

void Processor::requestStop() noexcept
{
    m_running = false;
}

ICommand* Processor::findCommand( const char* verb, unsigned verbLength ) noexcept
{
    ICommand* cmdPtr = m_commands.first();
    while ( cmdPtr )
    {
        // Check if the command matches
        if ( verbLength == strlen( cmdPtr->getVerb() ) && strncmp( verb, cmdPtr->getVerb(), verbLength ) == 0 )
        {
            return cmdPtr;
        }
        cmdPtr = m_commands.next( *cmdPtr );
    }

    // If I get here, then the command was not found
    return nullptr;
}

bool Processor::writeFrame( const char* text, Kit::Type::SSize_T maxBytes ) noexcept
{
    // Lock the output stream to avoid interleaving of output frames with console trace/logging
    Kit::System::Mutex::ScopeLock criticalSection( m_outLock );

    // Encode and output the text
    bool io  = true;
    io      &= m_framer.startFrame();
    io      &= m_framer.output( text, maxBytes );
    io      &= m_framer.endFrame();
    return io;
}


bool Processor::oobRead( void*               dstBuffer,
                         Kit::Type::SSize_T  numBytes,
                         Kit::Type::SSize_T& bytesRead ) noexcept
{
    return m_deframer.oobRead( static_cast<uint8_t*>( dstBuffer ), numBytes, bytesRead );
}

//////////////////////
Result_T Processor::processCommand( char* deframedInput ) noexcept
{
    const char* firstToken = Kit::Text::Strip::space( deframedInput );

    // Skip blank and comment lines
    if ( *firstToken == 0 || *firstToken == m_comment )
    {
        return +Result_T::CMD_SUCCESS;
    }

    // Lookup the command to be executed
    Result_T    result        = Result_T::CMD_ERR_NOT_SUPPORTED;
    const char* endFirstToken = Kit::Text::Strip::notSpace( firstToken );
    size_t      tokenSize     = endFirstToken - firstToken;
    ICommand*   cmdPtr        = findCommand( firstToken, tokenSize );

    // Command found
    if ( cmdPtr != nullptr )
    {
        // Authenticate the command and execute it if authorized.
        result = +Result_T::CMD_ERR_NOT_AUTHORIZED;
        if ( m_secPolicy.isAuthorized( cmdPtr->getRequiredPermissions(), deframedInput ) )
        {
            // Command is authorized --> execute the command
            result = cmdPtr->execute( *this, const_cast<char*>( firstToken ) );
        }
    }

    // If the command failed, then output the error message
    if ( result != +Result_T::CMD_SUCCESS )
    {
        outputCommandError( result, deframedInput );
    }

    return result;
}

void Processor::outputCommandError( Result_T result, const char* deframedInput ) noexcept
{
    // Suppress error output based on the security policy
    if ( !m_secPolicy.isSilent() )
    {
        // Lock the output stream to avoid interleaving of output frames with console trace/logging
        Kit::System::Mutex::ScopeLock criticalSection( m_outLock );

        m_workOutputBuffer.format( "%s: [%s]", result._to_string(), deframedInput );
        IContext::writeFrame( m_workOutputBuffer.getString() );
    }
}

int Processor::getAndProcessFrame() noexcept
{
    // Check for stop request
    if ( !m_running )
    {
        outputMessage( OPTION_KIT_TSHELL_PROCESSOR_FAREWELL );
        Kit::System::sleep( 250 );  // Allow time for the farewell message to be outputted
        return 1;
    }


    // Get the next command string from my input stream
    Kit::Type::SSize_T frameSize;
    int                readResult = readInput( frameSize );
    if ( readResult < 0 )
    {
        // Input stream error
        return -1;
    }
    else if ( readResult == 1 )
    {
        // House keeping
        m_workOutputBuffer.clear();
        m_workInputBuffer.clear();
        m_workBuffer.clear();
        m_frameBuffer[frameSize] = '\0';  // Ensure the raw input buffer gets null terminated

        // Execute the command
        Result_T result = processCommand( reinterpret_cast<char*>( m_frameBuffer ) );
        if ( result == +Result_T::CMD_ERR_IO )
        {
            // Output stream error
            return -1;
        }

        // Output the prompt
        outputMessage( OPTION_KIT_TSHELL_PROCESSOR_PROMPT );
    }

    // If I get here - then no error occurred
    return 0;
}

int Processor::readInput( Kit::Type::SSize_T& frameSize ) noexcept
{
    frameSize = 0;

    // Read a frame from the input stream
    if ( m_blocking )
    {
        // Blocking read
        if ( !m_deframer.scan( OPTION_KIT_TSHELL_PROCESSOR_INPUT_SIZE, m_frameBuffer, frameSize ) )
        {
            // Error reading raw input -->exit the Command processor
            return -1;
        }
        return 1;  // Return 1, i.e. blocking read always returns a full frame (or error)
    }
    else
    {
        // Non-blocking read
        bool isEof;
        if ( !m_deframer.scan( OPTION_KIT_TSHELL_PROCESSOR_INPUT_SIZE, m_frameBuffer, frameSize, isEof ) )
        {
            // Error reading raw input -->exit the Command processor
            return -1;
        }
        return isEof ? 1 : 0;  // Return 1 if a full frame was found, else 0 if input frame is incomplete
    }
}

void Processor::outputMessage( const char* textString ) noexcept
{
    if ( !m_secPolicy.isSilent() )
    {
        // Lock the output stream to avoid interleaving of output frames with console trace/logging
        Kit::System::Mutex::ScopeLock criticalSection( m_outLock );
        auto outfd = m_streamDestination.getStream();
        if ( outfd != nullptr )
        {
            outfd->write( textString );
        }
    }
}

/////////////////////
Kit::Container::OrderedList<ICommand>& Processor::getCommands() noexcept
{
    return m_commands;
}

ISecurity& Processor::getSecurity() noexcept
{
    return m_secPolicy;
}

Kit::Io::IOutput* Processor::getOutputStream() noexcept
{
    return m_streamDestination.getStream();
}       

void Processor::requestTShellExit() noexcept
{
    requestStop();
}

Kit::Text::IString& Processor::getOutputBuffer() noexcept
{
    return m_workOutputBuffer;
}

Kit::Text::IString& Processor::getInputBuffer() noexcept
{
    return m_workInputBuffer;
}

Kit::Text::IString& Processor::getWorkBuffer() noexcept
{
    return m_workBuffer;
}



}  // end namespace
}
//------------------------------------------------------------------------------