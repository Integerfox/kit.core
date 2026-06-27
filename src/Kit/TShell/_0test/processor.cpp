/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/IRunnable.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/TShell/Processor.h"
#include "Kit/TShell/NoSecurity.h"
#include "Kit/Container/Key.h"
#include "Kit/Io/Ram/InputOutputAllocate.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Api.h"
#include <string>

#define SECT_ "_0test"

///
using namespace Kit::System;
using namespace Kit::TShell;
using namespace Kit::Type;


namespace {

class TestCommand : public ICommand
{
public:
    TestCommand( const char*   verb,
                 Result_T      returnCode    = +Result_T::CMD_SUCCESS,
                 Permissions_T permissions   = OPTION_KIT_TSHELL_SECURITY_DEFAULT_PERMISSIONS,
                 SSize_T       oobReadLength = 0 ) noexcept
        : ICommand( verb )
        , m_returnCode( returnCode )
        , m_permissions( permissions )
        , m_executeCount( 0 )
        , m_oobReadLength( oobReadLength )
    {
        m_lastRaw[0] = '\0';
    }

    Result_T execute( Kit::TShell::IContext& context, char* rawCmdString ) noexcept override
    {
        ++m_executeCount;
        if ( rawCmdString == nullptr )
        {
            m_lastRaw[0] = '\0';
        }
        else
        {
            strncpy( m_lastRaw, rawCmdString, sizeof( m_lastRaw ) - 1 );
            m_lastRaw[sizeof( m_lastRaw ) - 1] = '\0';

            if ( m_oobReadLength > 0 )
            {
                SSize_T bytesRead = 0;
                context.oobRead( m_oobRaw, m_oobReadLength, bytesRead );
                REQUIRE( bytesRead == m_oobReadLength );
                m_oobRaw[bytesRead] = '\0';
            }
        }
        return m_returnCode;
    }

    const char*                getVerb() const noexcept override { return m_stringKeyPtr; }
    const char*                getUsage() const noexcept override { return "test"; }
    const char*                getHelp() const noexcept override { return nullptr; }
    Permissions_T              getRequiredPermissions() const noexcept override { return m_permissions; }
    const Kit::Container::Key& getKey() const noexcept override { return *this; }

    unsigned    getExecuteCount() const noexcept { return m_executeCount; }
    const char* getLastRaw() const noexcept { return m_lastRaw; }

    Result_T                         m_returnCode;
    Permissions_T                    m_permissions;
    unsigned                         m_executeCount;
    char                             m_lastRaw[64];
    SSize_T                          m_oobReadLength;
    char                             m_oobRaw[64];
};

class DenySecurity : public ISecurity
{
public:
    bool isAuthorized( Permissions_T,
                       const char*,
                       bool = false ) noexcept override
    {
        return false;
    }

    bool isSilent() noexcept override
    {
        return false;
    }
};

// UUT in different namespace -->test blocking semantics
class MyShellThread : public Kit::System::IRunnable
{
public:
    MyShellThread( Kit::TShell::Processor& processor, Kit::Io::IInput& infd, Kit::Io::IOutput& outfd )
        : m_processor( processor )
        , m_inFd( infd )
        , m_outFd( outfd )
    {
    }
public:
    void entry() noexcept override
    {
        m_processor.start( m_inFd, m_outFd, true );
    }

    Kit::TShell::Processor& m_processor;
    Kit::Io::IInput&        m_inFd;
    Kit::Io::IOutput&       m_outFd;
};

}  // end anonymous namespace

static std::string drainOutput( Kit::Io::Ram::InputOutput& stream )
{
    std::string        output;
    char               ch;
    Kit::Type::SSize_T bytesRead = 0;

    while ( stream.available() )
    {
        if ( !stream.read( &ch, 1, bytesRead ) || bytesRead != 1 )
        {
            break;
        }
        output.push_back( ch );
    }

    KIT_SYSTEM_TRACE_MSG( SECT_, "output: [%s]", output.c_str() );
    return output;
}

////////////////////////////////////
TEST_CASE( "Processor" )
{
    ShutdownUnitTesting::clearAndUseCounter();
    constexpr char testTerminator = '~';

    SECTION( "Start executes command and emits prompt flow" )
    {
        Kit::Container::OrderedList<ICommand> commands;
        TestCommand                           cmd( "echo" );
        commands.insert( cmd );

        Kit::Io::Ram::InputOutputAllocate<256> inFd;
        Kit::Io::Ram::InputOutputAllocate<512> outFd;
        Kit::Framing::StreamSource             src;
        Kit::Framing::StreamDestination        dst;
        NoSecurity                             security;
        Kit::System::Mutex                     lock;
        Processor                              uut( commands, src, dst, security, lock, ' ', '#', '`', ' ', '"', testTerminator );

        inFd.write( "echo hello\n" );
        REQUIRE( uut.start( inFd, outFd, false ) == true );
        REQUIRE( cmd.getExecuteCount() == 1u );
        REQUIRE( strcmp( cmd.getLastRaw(), "echo hello" ) == 0 );

        std::string output = drainOutput( outFd );
        REQUIRE( output.find( OPTION_KIT_TSHELL_PROCESSOR_GREETING ) != std::string::npos );
        REQUIRE( output.find( OPTION_KIT_TSHELL_PROCESSOR_PROMPT ) != std::string::npos );
    }

    SECTION( "Comments and blank lines are ignored" )
    {
        Kit::Container::OrderedList<ICommand> commands;
        TestCommand                           cmd( "go" );
        commands.insert( cmd );

        Kit::Io::Ram::InputOutputAllocate<256> inFd;
        Kit::Io::Ram::InputOutputAllocate<512> outFd;
        Kit::Framing::StreamSource             src;
        Kit::Framing::StreamDestination        dst;
        NoSecurity                             security;
        Kit::System::Mutex                     lock;
        Processor                              uut( commands, src, dst, security, lock, ' ', '#', '`', ' ', '"', testTerminator );

        inFd.write( "\n# comment\n  # spaced\n" );
        REQUIRE( uut.start( inFd, outFd, false ) == true );
        REQUIRE( cmd.getExecuteCount() == 0u );

        std::string output = drainOutput( outFd );
        REQUIRE( output.find( "ERROR_" ) == std::string::npos );
    }

    SECTION( "Unknown command reports not supported" )
    {
        Kit::Container::OrderedList<ICommand> commands;

        Kit::Io::Ram::InputOutputAllocate<256> inFd;
        Kit::Io::Ram::InputOutputAllocate<512> outFd;
        Kit::Framing::StreamSource             src;
        Kit::Framing::StreamDestination        dst;
        NoSecurity                             security;
        Kit::System::Mutex                     lock;
        Processor                              uut( commands, src, dst, security, lock, ' ', '#', '`', ' ', '"', testTerminator );

        inFd.write( "unknown\n" );
        REQUIRE( uut.start( inFd, outFd, false ) == true );

        std::string output = drainOutput( outFd );
        REQUIRE( output.find( "CMD_ERR_NOT_SUPPORTED" ) != std::string::npos );
        REQUIRE( output.find( "unknown" ) != std::string::npos );
    }

    SECTION( "Authorization failure blocks execution and reports error" )
    {
        Kit::Container::OrderedList<ICommand> commands;
        TestCommand                           cmd( "secure", +Result_T::CMD_SUCCESS, 99u );
        commands.insert( cmd );

        Kit::Io::Ram::InputOutputAllocate<256> inFd;
        Kit::Io::Ram::InputOutputAllocate<512> outFd;
        Kit::Framing::StreamSource             src;
        Kit::Framing::StreamDestination        dst;
        DenySecurity                           security;
        Kit::System::Mutex                     lock;
        Processor                              uut( commands, src, dst, security, lock, ' ', '#', '`', ' ', '"', testTerminator );

        inFd.write( "secure run\n" );
        REQUIRE( uut.start( inFd, outFd, false ) == true );
        REQUIRE( cmd.getExecuteCount() == 0u );

        std::string output = drainOutput( outFd );
        REQUIRE( output.find( "CMD_ERR_NOT_AUTHORIZED" ) != std::string::npos );
        REQUIRE( output.find( "secure run" ) != std::string::npos );
    }

    SECTION( "findCommand and stop via poll" )
    {
        Kit::Container::OrderedList<ICommand> commands;
        TestCommand                           cmd( "verb" );
        commands.insert( cmd );

        Kit::Io::Ram::InputOutputAllocate<256> inFd;
        Kit::Io::Ram::InputOutputAllocate<512> outFd;
        Kit::Framing::StreamSource             src;
        Kit::Framing::StreamDestination        dst;
        NoSecurity                             security;
        Kit::System::Mutex                     lock;
        Processor                              uut( commands, src, dst, security, lock, ' ', '#', '`', ' ', '"', testTerminator );

        REQUIRE( uut.findCommand( "verb", 4 ) == &cmd );
        REQUIRE( uut.findCommand( "missing", 7 ) == nullptr );
        REQUIRE( uut.poll() == 1 );

        REQUIRE( uut.start( inFd, outFd, false ) == true );
        inFd.write( "verb\n" );
        REQUIRE( uut.poll() == 0 );
        REQUIRE( cmd.getExecuteCount() == 1u );

        uut.requestStop();
        REQUIRE( uut.poll() == 1 );

        drainOutput( outFd );
    }

    SECTION( "Blocking Reads" )
    {
        Kit::Container::OrderedList<ICommand> commands;
        TestCommand                           cmd( "verb" );
        commands.insert( cmd );

        Kit::Io::Ram::InputOutputAllocate<256> inFd;
        Kit::Io::Ram::InputOutputAllocate<512> outFd;
        Kit::Framing::StreamSource             src;
        Kit::Framing::StreamDestination        dst;
        NoSecurity                             security;
        Kit::System::Mutex                     lock;
        Processor                              uut( commands, src, dst, security, lock, ' ', '#', '`', ' ', '"', testTerminator );
        MyShellThread                          uutThread( uut, inFd, outFd );

        auto* t1 = Kit::System::Thread::create( uutThread, "UTT" );
        REQUIRE( t1 != nullptr );
        Kit::System::sleep( 200 );  // Allow time for the thread to start
        inFd.write( "verb echo\n" );
        Kit::System::sleep( 200 );  // Allow time for the thread to process
        REQUIRE( cmd.getExecuteCount() == 1u );
        std::string output = drainOutput( outFd );
        REQUIRE( strncmp( cmd.getLastRaw(), "verb echo", 9 ) == 0 );
        uut.requestStop();
        inFd.write( "unsupported command\n" );  // This will cause the thread to exit since the processor will return an error code
        Kit::System::sleep( 200 );              // Allow time for the thread to process

        uutThread.pleaseStop();
        Kit::System::Thread::destroy( *t1 );
    }

    SECTION( "misc" )
    {
        Kit::Container::OrderedList<ICommand> commands;
        TestCommand                           cmd( "verb" );
        commands.insert( cmd );
        TestCommand cmd2( "billy" );
        commands.insert( cmd2 );
        TestCommand cmd3( "goat" );
        commands.insert( cmd3 );

        Kit::Io::Ram::InputOutputAllocate<256> inFd;
        Kit::Io::Ram::InputOutputAllocate<512> outFd;
        Kit::Framing::StreamSource             src;
        Kit::Framing::StreamDestination        dst;
        NoSecurity                             security;
        Kit::System::Mutex                     lock;
        Processor                              uut( commands, src, dst, security, lock, ' ', '#', '`', ' ', '"', testTerminator );

        // Sorted commands
        auto& list = uut.getCommands();
        REQUIRE( list.first() == &cmd2 );
        REQUIRE( list.next( cmd2 ) == &cmd3 );
        REQUIRE( list.next( cmd3 ) == &cmd );

        REQUIRE( uut.getDelimiterChar() == ' ' );
        REQUIRE( uut.getQuoteChar() == '"' );
        REQUIRE( uut.getTerminatorChar() == testTerminator );
        REQUIRE( uut.getEscapeChar() == '`' );

        Kit::Text::IString& outBuf = uut.getOutputBuffer();
        REQUIRE( outBuf.availLength() == OPTION_KIT_TSHELL_PROCESSOR_OUTPUT_SIZE );
        Kit::Text::IString& inBuf = uut.getInputBuffer();
        REQUIRE( inBuf.availLength() == OPTION_KIT_TSHELL_PROCESSOR_INPUT_SIZE );
        Kit::Text::IString& workBuf = uut.getWorkBuffer();
        REQUIRE( workBuf.availLength() == OPTION_KIT_TSHELL_PROCESSOR_INPUT_SIZE );

        REQUIRE( &(uut.getSecurity()) == &security );

        uut.requestTShellExit();
        REQUIRE( uut.poll() == 1 );
    }

    SECTION( "OOB Read" )
    {
        Kit::Container::OrderedList<ICommand> commands;
        TestCommand                           cmd( "verb", +Result_T::CMD_SUCCESS, OPTION_KIT_TSHELL_SECURITY_DEFAULT_PERMISSIONS, 4u );
        commands.insert( cmd );

        Kit::Io::Ram::InputOutputAllocate<256> inFd;
        Kit::Io::Ram::InputOutputAllocate<512> outFd;
        Kit::Framing::StreamSource             src;
        Kit::Framing::StreamDestination        dst;
        NoSecurity                             security;
        Kit::System::Mutex                     lock;
        Processor                              uut( commands, src, dst, security, lock, ' ', '#', '`', ' ', '"', testTerminator );

        REQUIRE( uut.start( inFd, outFd, false ) == true );
        inFd.write( "verb\n1234" );
        REQUIRE( uut.poll() == 0 );
        REQUIRE( cmd.getExecuteCount() == 1u );
        REQUIRE( strncmp( cmd.getLastRaw(), "verb", 4 ) == 0 );
        REQUIRE( strncmp( cmd.m_oobRaw, "1234", 4 ) == 0 );
        drainOutput( outFd );
    }
    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
