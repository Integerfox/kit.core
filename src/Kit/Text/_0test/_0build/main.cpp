//#include "Kit/System/Api.h"
#include "catch2/catch_session.hpp"

int main( int argc, char* argv[] )
{
    // Initialize KIT Library
    //Kit::System::initialize();

    // Run the test(s)
    return Catch::Session().run( argc, argv );
}
