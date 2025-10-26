
#include "Bsp/Api.h"
#include "Kit/System/Api.h"
#include <stdio.h>
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/FreeRTOS/Thread.h"

class Thread1 : public Kit::System::IRunnable
{
public:
    Thread1() {};

protected:
    void appRun()
    {
        Kit::System::Thread& t1 = Kit::System::Thread::getCurrent();
        printf( "Thread Running: %s, %x, %p\n", t1.getName(), t1.getId(), t1.getNativeHandle() );
 
        uint32_t timeMark1 = Kit::System::ElapsedTime::milliseconds();
        uint32_t timeMark2 = timeMark1;
        for ( ;;)
        {
            uint32_t now = Kit::System::ElapsedTime::milliseconds();
            if ( now - timeMark1 >= 100 )
            {
                Bsp_Api_toggle_debug1();
                timeMark1 = now;
            }
            if ( now - timeMark2 >= 1000 )
            {
                Bsp_Api_toggle_debug2();
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
    Bsp_Api_initialize();
    printf( "\n**** BSP TEST APPLICATION STARTED ****\n\n" );

    //// Initialize CPL
    Kit::System::Api::initialize();

    //// Create my test thread
    Kit::System::Thread* t1 = Kit::System::FreeRTOS::Thread::create( runnable_, "TEST", CPL_SYSTEM_THREAD_PRIORITY_NORMAL );
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


