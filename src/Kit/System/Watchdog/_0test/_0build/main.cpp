/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"
#include "catch2/catch_session.hpp"
#include <chrono>
#include <cstdint>

extern uint32_t getNativeTimestamp();

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

uint32_t getNativeTimestamp()
{
    // Return a nanosecond timestamp (as uint32_t for embedded compatibility)
    std::chrono::high_resolution_clock::time_point now         = std::chrono::high_resolution_clock::now();
    auto                                           nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>( now.time_since_epoch() ).count();
    return static_cast<uint32_t>( nanoseconds );
}