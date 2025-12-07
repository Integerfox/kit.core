#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"
#include "Kit/System/IRunnable.h"
#include "Kit/System/Thread.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include <stdio.h>

extern void runtests( size_t maxLoopCount );

// Anonymous namespace for local definitions
namespace {

class NullRunnable : public Kit::System::IRunnable
{
public:
    ///
    NullRunnable() = default;

public:
    ///
    void entry() noexcept override
    {
    }
};

}  // end anonymous namespace


/////////////////////////////////////////
int main( int argc, char* argv[] )
{
    // Initialize KIT Library
    Kit::System::initialize();

    // Enable trace
    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "*LOG_" );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eVERBOSE );

    runtests( 10 );
    size_t fatalErrors = Kit::System::ShutdownUnitTesting::getAndClearCounter();
    if ( fatalErrors != 0 )
    {
        printf( "*** ERROR: TESTS FAILED! (%zu)\n", fatalErrors );
        return 1;
    }

    // This should generate a fatal error because there the one-and-only  thread has already been created
    NullRunnable t1;
    auto threadPtr = Kit::System::Thread::create( t1, "Apple2" );
    if ( threadPtr == nullptr )
    {
        printf( "*** ERROR: TESTS FAILED! (Thread creation should unexpectedly failed)\n" );
        return 1;
    }
    NullRunnable t2;
    threadPtr = Kit::System::Thread::create( t2, "Orange" );
    if ( threadPtr != nullptr )
    {
        printf( "*** ERROR: TESTS FAILED! (Allowed to create a second thread)\n" );
        return 1;
    }

    printf( "TESTS PASSED!\n" );
    return 0;
}
