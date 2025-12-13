/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file 

    Hardware test to demonstrate raw thread failure detection.
    
    This test verifies that when a raw thread gets stuck in a busy loop
    and exceeds its thread-specific watchdog timeout, the supervisor
    detects this failure and trips the MCU watchdog, causing a system reset.
    
    Expected behavior:
    - First run: System starts normally, raw thread gets stuck after delay
    - Supervisor detects raw thread timeout and trips the watchdog
    - Watchdog resets the MCU
    - Second run: Detects watchdog reset flag and indicates test passed

*/

#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Thread.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Watchdog/Supervisor.h"
#include "Kit/System/Watchdog/WatchedEventThread.h"
#include "Kit/System/Watchdog/WatchedRawThread.h"
#include "Kit/System/EventLoop.h"
#include "Kit/EventQueue/Server.h"
#include <cstdint>
#include <string.h>

#define SECT_ "_0test"

///
using namespace Kit::System;
using namespace Kit::System::Watchdog;
using namespace Kit::EventQueue;

//------------------------------------------------------------------------------
// Test Configuration
//------------------------------------------------------------------------------

/** Hardware watchdog timeout is 1000ms (from HAL implementation).
*/
#define HW_WATCHDOG_TIMEOUT_MS     1000

/** Supervisor thread watchdog timeout.
    Must be less than HW watchdog timeout.
*/
#define SUPERVISOR_WDOG_TIMEOUT_MS 800

/** Supervisor thread health check interval.
    Must be less than SUPERVISOR_WDOG_TIMEOUT_MS.
*/
#define SUPERVISOR_HEALTH_CHECK_MS 200

/** Raw thread watchdog timeout.
    This is shorter than the supervisor timeout to ensure the raw thread
    failure is detected and handled appropriately.
*/
#define RAW_THREAD_WDOG_TIMEOUT_MS 400

/** Time to run normally before triggering the stuck condition.
    This gives the watchdog time to initialize and start running properly.
*/
#define NORMAL_RUN_DELAY_MS        2000

/** How long the raw thread will be stuck.
    This must exceed RAW_THREAD_WDOG_TIMEOUT_MS to trigger watchdog trip.
*/
#define STUCK_DURATION_MS          ( RAW_THREAD_WDOG_TIMEOUT_MS + 500 )

//------------------------------------------------------------------------------
// Raw Thread Implementation
//------------------------------------------------------------------------------

/// Counter for raw thread to track when to get stuck
static volatile uint32_t rawThreadCounter_ = 0;

/// Flag to trigger the stuck condition
static volatile bool triggerRawThreadStuck_ = false;

/// Runnable for the raw thread
class RawThreadRunnable : public IRunnable
{
protected:
    /// Watchdog monitor for this raw thread
    RawThread m_wdog;

public:
    /** Constructor. Initializes the raw thread with watchdog monitoring.
     */
    RawThreadRunnable() noexcept
        : m_wdog( RAW_THREAD_WDOG_TIMEOUT_MS )
    {
    }

public:
    /// Thread entry point
    void entry() noexcept override
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread starting (wdog timeout: %u ms)\r\n",
                              (unsigned)RAW_THREAD_WDOG_TIMEOUT_MS );

        // Start watchdog monitoring for this thread
        KIT_SYSTEM_WATCHDOG_START_RAWTHREAD( m_wdog );

        // Run normally until we're told to get stuck
        while ( !triggerRawThreadStuck_ )
        {
            // Do some work
            Bsp_toggle_debug2();

            // Kick the watchdog to indicate we're alive
            KIT_SYSTEM_WATCHDOG_KICK_RAWTHREAD( m_wdog );

            // Wait a bit (less than our watchdog timeout)
            sleep( 100 );
            rawThreadCounter_ += 100;
        }

        // Now get stuck in a busy loop WITHOUT kicking the watchdog
        KIT_SYSTEM_TRACE_MSG( SECT_, "*** RAW THREAD STUCK - Entering busy loop ***\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Busy loop will last %u ms (thread timeout is %u ms)\r\n",
                              STUCK_DURATION_MS, RAW_THREAD_WDOG_TIMEOUT_MS );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Supervisor should detect timeout and trip watchdog\r\n" );

        uint32_t startTime = HAL_GetTick();
        while ( ( HAL_GetTick() - startTime ) < STUCK_DURATION_MS )
        {
            // Busy wait - watchdog will NOT be kicked
            Bsp_toggle_debug2();
            // Small delay to make LED visible
            for ( volatile uint32_t i = 0; i < 100000; i++ )
            {
                __NOP();
            }
        }

        // If we get here, the watchdog failed to reset us
        KIT_SYSTEM_TRACE_MSG( SECT_, "*** ERROR: Watchdog did not reset system! ***\r\n" );

        // Stop watchdog monitoring
        KIT_SYSTEM_WATCHDOG_STOP_RAWTHREAD( m_wdog );

        // Instead of terminating, go into idle loop
        for ( ;; )
        {
            sleep( 1000 );
        }
    }
};

//------------------------------------------------------------------------------
// Test Monitor Thread
//------------------------------------------------------------------------------

/// Test monitor runnable that coordinates the test
class TestMonitor : public IRunnable
{
public:
    void entry() noexcept override
    {
        KIT_SYSTEM_TRACE_MSG( SECT_, "Test monitor starting\r\n" );
        
        // Give supervisor thread time to start, initialize, and have its
        // first health check timer fire
        sleep( 1000 );

        if ( !Supervisor::enableWdog() )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "FAILED to enable hardware watchdog!\r\n" );
            FatalError::logf( Shutdown::eFAILURE, "Failed to enable hardware watchdog\r\n" );
        }

        KIT_SYSTEM_TRACE_MSG( SECT_, "Hardware watchdog enabled successfully\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Running normally for %u ms...\r\n", NORMAL_RUN_DELAY_MS );
        // Run normally for a bit to establish that watchdog is working
        uint32_t mainCounter = 0;
        while ( mainCounter < NORMAL_RUN_DELAY_MS )
        {
            Bsp_toggle_debug1();
            sleep( 500 );
            mainCounter += 500;
        }

        // Now trigger the stuck condition in the raw thread
        KIT_SYSTEM_TRACE_MSG( SECT_, "Triggering raw thread stuck condition...\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread will stop kicking its watchdog\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Waiting for supervisor to detect and trip watchdog...\r\n" );

        triggerRawThreadStuck_ = true;

        // Wait for the watchdog to reset us
        for ( int32_t i = 0; i < 20; ++i )
        {
            sleep( 500 );
            Bsp_toggle_debug1();
            KIT_SYSTEM_TRACE_MSG( SECT_, "Still waiting for reset... (%d)\r\n", (int)i );
        }

        // If we get here, something went wrong
        KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Test FAILED\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Watchdog did not reset the system\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "Raw thread timeout was not detected\r\n" );
        KIT_SYSTEM_TRACE_MSG( SECT_, "==================================================\r\n" );
        // Blink LED rapidly to indicate failure
        for ( ;; )
        {
            Bsp_toggle_debug1();
            sleep( 100 );
        }
    }
};

//------------------------------------------------------------------------------
// Static Storage for Objects (to avoid stack corruption)
//------------------------------------------------------------------------------

static uint8_t supervisorWdogStorage_[sizeof(WatchedEventThread)] __attribute__((aligned(8)));
static uint8_t eventLoopStorage_[sizeof(Server)] __attribute__((aligned(8)));
static uint8_t rawRunnableStorage_[sizeof(RawThreadRunnable)] __attribute__((aligned(8)));
static uint8_t testMonitorStorage_[sizeof(TestMonitor)] __attribute__((aligned(8)));

//------------------------------------------------------------------------------
// Main Application
//------------------------------------------------------------------------------

int main( void )
{
    // Initialize the board (HAL, clocks, GPIOs, UART)
    Bsp_initialize();
    
    // Small delay to let UART stabilize
    HAL_Delay( 200 );
    
    const char* msg = "\r\n\r\n**** RAW THREAD STUCK TEST START ****\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), 1000);
    HAL_Delay( 100 );

    // Initialize the KIT system
    Kit::System::initialize();

    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( SECT_ );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::Trace::eVERBOSE );

    // Check if this is a watchdog reset
    if ( __HAL_RCC_GET_FLAG( RCC_FLAG_IWDGRST ) != RESET )
    {
        const char* successMsg = "*** WATCHDOG RESET DETECTED ***\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t*)successMsg, strlen(successMsg), 1000);
        const char* successMsg2 = "Test PASSED - System was reset by watchdog when raw thread timed out\r\n";
        HAL_UART_Transmit(&huart3, (uint8_t*)successMsg2, strlen(successMsg2), 1000);
        
        __HAL_RCC_CLEAR_RESET_FLAGS();

        // Keep both LEDs on to indicate success
        Bsp_turn_on_debug1();
        Bsp_turn_on_debug2();

        // Infinite loop - test is complete
        while ( 1 )
        {
            HAL_Delay( 1000 );
        }
    }

    const char* msg2 = "KIT System initialized\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t*)msg2, strlen(msg2), 1000);

    char buf[200];
    snprintf(buf, sizeof(buf), "Raw Thread Stuck Test Starting\r\nHW Watchdog Timeout: %u ms\r\nRaw Thread Timeout: %u ms\r\nStuck Duration: %u ms\r\n", 
             HW_WATCHDOG_TIMEOUT_MS, RAW_THREAD_WDOG_TIMEOUT_MS, STUCK_DURATION_MS);
    HAL_UART_Transmit(&huart3, (uint8_t*)buf, strlen(buf), 1000);

    // First run - set up the test
    const char* msg3 = "First run - setting up watchdog test\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t*)msg3, strlen(msg3), 1000);

    // Configure the supervisor thread (event thread) using placement new
    WatchedEventThread* supervisorWdog = new (supervisorWdogStorage_) WatchedEventThread(
        SUPERVISOR_WDOG_TIMEOUT_MS,
        SUPERVISOR_HEALTH_CHECK_MS,
        true );  // This is the supervisor thread

    // Create the event loop for the supervisor thread using placement new
    Server* eventLoop = new (eventLoopStorage_) Server(
        OPTION_KIT_SYSTEM_EVENT_LOOP_TIMEOUT_PERIOD,
        nullptr,
        supervisorWdog );

    // Create and start the supervisor thread
    auto* supervisorThread = Thread::create( *eventLoop, "SUPERVISOR" );
    if ( !supervisorThread )
    {
        FatalError::logf( Shutdown::eFAILURE, "Failed to create supervisor thread" );
    }
    const char* msg4 = "Supervisor thread created\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t*)msg4, strlen(msg4), 1000);

    // Create and start the raw thread using placement new
    RawThreadRunnable* rawRunnable = new (rawRunnableStorage_) RawThreadRunnable();
    auto* rawThread = Thread::create( *rawRunnable, "RAW_THREAD" );
    if ( !rawThread )
    {
        FatalError::logf( Shutdown::eFAILURE, "Failed to create raw thread" );
    }
    const char* msg5 = "Raw thread created\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t*)msg5, strlen(msg5), 1000);

    // Create and start the test monitor thread using placement new
    TestMonitor* testMonitor = new (testMonitorStorage_) TestMonitor();
    auto* monitorThread = Thread::create( *testMonitor, "TEST_MONITOR" );
    if ( !monitorThread )
    {
        FatalError::logf( Shutdown::eFAILURE, "Failed to create test monitor thread" );
    }
    const char* msg6 = "Test monitor thread created\r\n";
    HAL_UART_Transmit(&huart3, (uint8_t*)msg6, strlen(msg6), 1000);
    
    KIT_SYSTEM_TRACE_MSG( SECT_, "Starting scheduler...\r\n" );
    enableScheduling();

    // Should never reach here
    return 0;
}
