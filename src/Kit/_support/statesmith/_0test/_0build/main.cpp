#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"
#include "../ContextMyFsm.h"

static Foo::Bar::ContextMyFsm myWidget_;

int main( int argc, char* argv[] )
{
    // Initialize Colony
    Kit::System::initialize();

    // Enable trace
    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "*Foo::Bar" );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eINFO );

    // Start my test widget, aka start the State Machine
    myWidget_.start();

    // Generate a complete arbitrary sequence of events
    myWidget_.notifyAbcChanged();
    myWidget_.tick();
    myWidget_.notifyXyzStarted();
    myWidget_.tick();
    myWidget_.notifyXyzCompleted();
    myWidget_.tick();
    myWidget_.notifyAbcChanged();
    myWidget_.tick();
    myWidget_.notifyXyzStarted();
    myWidget_.tick();
    myWidget_.notifyXyzCompleted();
}
