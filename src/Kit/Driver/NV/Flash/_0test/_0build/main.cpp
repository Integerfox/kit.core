#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"
#include "catch2/catch_session.hpp"

// Increase default stack size on Windows for large MockFlash test objects
#ifdef _WIN32
#pragma comment(linker, "/STACK:8388608")
#endif

int main( int argc, char* argv[] )
{
    Kit::System::initialize();
    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "*LOG_" );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eINFO );
    return Catch::Session().run( argc, argv );
}
