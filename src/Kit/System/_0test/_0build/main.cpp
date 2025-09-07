#include "Kit/System/api.h"
#include "Kit/System/Trace.h"
#include "catch2/catch_session.hpp"

extern uint32_t getNativeTimestamp( void );

int main( int argc, char* argv[] )
{
    // Initialize KIT Library
    Kit::System::initialize();

    // Enable trace
    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eVERBOSE );

    // Run the test(s)
    return Catch::Session().run( argc, argv );
}

// Native timestamp - since KIT's elapsed time uses the IStartupHook interface.  
// Must support a duration of at least 10s, with at least 1ms resolution
uint32_t getNativeTimestamp( void )
{
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    
    // Return milliseconds since epoch (at least 1ms resolution, supports >10s)
    return static_cast<uint32_t>(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}