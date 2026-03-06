/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"
#include "Kit/System/IRunnable.h"
#include "Kit/System/Thread.h"
#include "Dm/Transaction/example.h"

namespace {  // Anonymous namespace

// 'main' thread for running the Example application
class MyMainThread : public Kit::System::IRunnable
{
public:
    void entry() noexcept override
    {
        Dm::Transaction::runExample();

        // Don't let the thread end if/when the application returns, just loop forever waiting for a reset/power-cycle
        for ( ;; )
            ;
    }
};


}  // end anonymous namespace
/*-----------------------------------------------------------*/
// Don't create on the stack due to how FreeRTOS handles the original stack when starting the scheduler.
static MyMainThread mainThread_;  // Static instance of the application thread

int main( void )
{
    // Initialize the board
    Bsp_initialize();

    // Initialize KIT
    Kit::System::initialize();

    // Create/start the application thread
    Kit::System::Thread::create( mainThread_, "main" );

    // Start the scheduler (Note: This method should never return)
    Kit::System::enableScheduling();
    return 0;
}
