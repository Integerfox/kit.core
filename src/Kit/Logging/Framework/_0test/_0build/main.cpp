#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"
#include "catch2/catch_session.hpp"

int main( int argc, char* argv[] )
{
    // Initialize KIT Library
    Kit::System::initialize();

    // Enable trace
    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "*LOG_" );
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "FATAL" );
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "WARNING" );
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "EVENT" );
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "INFO" );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eINFO );

    // Run the test(s)
    return Catch::Session().run( argc, argv );
}
