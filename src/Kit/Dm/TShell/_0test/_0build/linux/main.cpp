#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"
#include "Kit/TShell/Command/_0test/test.h"
#include "Kit/Io/Stdio/StdIn.h"
#include "Kit/Io/Stdio/StdOut.h"

#include "Kit/TShell/Command/Posix/Threads.h"

static Kit::TShell::Command::Posix::Threads  threadsCmd_( g_commandList );

static Kit::Io::Stdio::StdIn  stdin_;
static Kit::Io::Stdio::StdOut stdout_;

int main( int argc, char* argv[] )
{
    // Initialize KIT Library
    Kit::System::initialize();

    // Enable trace
    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "*LOG_" );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::TraceLevel::eINFO );

    // Run the test(s)
    shell_test( stdin_, stdout_ );
    return 0;
}
