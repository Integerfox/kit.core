#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Thread.h"
#include "Kit/TShell/Command/_0test/test.h"
#include "Kit/TShell/Command/FreeRTOS/Threads.h"
#include "Kit/Bsp/Api.h"
#include "Kit/Io/IInputOutput.h"

#define SECT_ "_0test"
static Kit::TShell::Command::FreeRTOS::Threads threadsCmd_( g_commandList );
extern Kit::Io::IInputOutput&                  g_bspConsoleStream;

////////////////////////////////////////////////////////////////////////////////
namespace {


class TestRunnable : public Kit::System::IRunnable
{
public:
    ///
    TestRunnable() = default;

public:
    ///
    void entry() noexcept override
    {
        shell_test( g_bspConsoleStream, g_bspConsoleStream );
    }
};

}  // end anonymous namespace

static TestRunnable testRunnable_;  // NOTE: Can't be a local variable because FreeRTOS 'messes' with the native stack when starting the scheduler

int main( int argc, char* argv[] )
{
    // Initialize BSP and KIT Library
    Bsp_initialize();
    Kit::System::initialize();
    KIT_SYSTEM_TRACE_MSG( SECT_, "**** KIT-SYSTEM TEST APPLICATION STARTED ****" );

    // Enable trace
    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "*LOG_" );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::TraceLevel::eINFO );

    // Create a thread
    auto*        testThreadHandle = Kit::System::Thread::create( testRunnable_, "TEST" );
    if ( testThreadHandle == nullptr )
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "FAILED to create test thread" );
        for ( ;; )
        {
            Kit::System::sleep( 1000 );
        }
    }
    else
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, ( "Starting scheduler..." ) );
        Kit::System::enableScheduling();
    }
}