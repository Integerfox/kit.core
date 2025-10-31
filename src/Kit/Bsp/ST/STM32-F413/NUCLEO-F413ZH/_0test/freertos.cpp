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
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/FreeRTOS/Thread.h"
#include <stdio.h>

class Thread1 : public Kit::System::IRunnable
{
public:
    Thread1() {};

protected:
    void entry() noexcept override
    {
        Kit::System::Thread& t1 = Kit::System::Thread::getCurrent();
        printf( "Thread Running: %s, %p\n", t1.getName(), t1.getId() );
 
        uint32_t timeMark1 = Kit::System::ElapsedTime::milliseconds();
        uint32_t timeMark2 = timeMark1;
        for ( ;;)
        {
            uint32_t now = Kit::System::ElapsedTime::milliseconds();
            if ( now - timeMark1 >= 100 )
            {
                Bsp_toggle_debug1();
                timeMark1 = now;
            }
            if ( now - timeMark2 >= 1000 )
            {
                Bsp_toggle_debug2();
                timeMark2 = now;
                printf( "now=%lu\n", now );
            }

            uint32_t now2 = Kit::System::ElapsedTime::milliseconds();
            Kit::System::sleep( 10 - ( now2 - now ) );
        }
    }
};

/* NOTE: The way FreeRTOS start the 'first' task - corrupts the raw main
         stack -->this means we cannot allocate anything on the raw main
         stack and expect it to stay in scope for duration of the application
*/
static Thread1 runnable_;

int main( void )
{
    // Initialize the board
    Bsp_initialize();
    printf( "\n**** BSP TEST APPLICATION STARTED ****\n\n" );

    //// Initialize CPL
    Kit::System::initialize();

    //// Create my test thread
    Kit::System::Thread* t1 = Kit::System::FreeRTOS::Thread::create( runnable_, "TEST", KIT_SYSTEM_THREAD_PRIORITY_NORMAL );
    if ( t1 == nullptr )
    {
        printf( "Failed to created thread!!\n" );
        while ( 1 );
    }
    else
    {
        printf( "Thread created %s thread\n", t1->getName() );
    }

    //// Start the scheduler
    Kit::System::enableScheduling();
    while ( 1 );    // Enable scheduling should NEVER return!
}


