#include "Itc/Synchronous/example.h"
#include "Kit/System/Api.h"

int main( int argc, char* argv[] )
{
    // Initialize the KIT Library
    Kit::System::initialize();

    // Run the application example
    return Itc::Synchronous::runExample();
}
