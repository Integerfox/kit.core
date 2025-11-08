#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"

extern void bsp_test_compile_and_link( void );

int main( int argc, char* argv[] )
{
    // Initialize Colony
    Bsp_initialize();
    Kit::System::initialize();
    bsp_test_compile_and_link();
}
