//#include "Cpl/System/Api.h"
#include "catch2/catch_session.hpp"

int main( int argc, char* argv[] )
{
    // Initialize Colony
    //Cpl::System::Api::initialize();

    // Run the test(s)
    return Catch::Session().run( argc, argv );
}
