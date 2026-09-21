#include "Examples/Job/Temperature/example.h"
#include "Kit/System/Api.h"
#include "Kit/Io/Stdio/StdIn.h"
#include "Kit/Io/Stdio/StdOut.h"

#include "Kit/TShell/Command/Win32/Threads.h"

static Kit::TShell::Command::Win32::Threads  threadsCmd_( Examples::Job::Temperature::g_commandList );
static Kit::Io::Stdio::StdIn  stdin_;
static Kit::Io::Stdio::StdOut stdout_;


int main( int argc, char* argv[] )
{
    // Initialize the KIT Library
    Kit::System::initialize();

    // Run the application example
    return Examples::Job::Temperature::runExample( stdin_, stdout_ );
}
