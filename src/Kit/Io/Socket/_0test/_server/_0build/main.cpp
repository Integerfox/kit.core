#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"

extern void echoServer( int portToListenOn, const char* exitString ) noexcept;

int main( int argc, char* argv[] )
{
    // Initialize KIT Library
    Kit::System::initialize();

    // Enable trace
    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "*LOG_" );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eVERBOSE );

    // Run the test(s)
    echoServer( 5002, "bobs-your-uncle");
}
