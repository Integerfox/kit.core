/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file 

    Hardware Abstraction Layer (HAL) implementation for STM32F4 Independent
    Watchdog (IWDG) functionality.

    This implementation provides platform-specific watchdog control using the
    STM32F4 IWDG peripheral. The IWDG is clocked by the LSI (Low Speed Internal)
    oscillator at approximately 32 kHz.

*/

#include "Kit/System/Watchdog/Hal.h"
#include "Kit/Bsp/Api.h"
#include "stm32f4xx_hal.h"

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------

/** Hardware watchdog timeout period in milliseconds.
    This defines the maximum time the watchdog can go without being refreshed
    before triggering a system reset. The supervisor must kick the watchdog
    more frequently than this timeout.
    
    Calculation:
    - LSI frequency: ~32 kHz (typically 32000 Hz)
    - Prescaler: 32
    - Counter clock: 32000 / 32 = 1000 Hz (1 ms per tick)
    - Reload value: 1000 ticks
    - Timeout: 1000 ms = 1 second
*/
#define WATCHDOG_TIMEOUT_MS 1000

/** IWDG prescaler divider.
    IWDG_PRESCALER_32 provides a good balance between timeout resolution
    and maximum timeout period for the LSI clock frequency.
*/
#define WATCHDOG_PRESCALER IWDG_PRESCALER_32

/** IWDG reload counter value.
    Based on LSI frequency of ~32kHz and prescaler of 32:
    Counter frequency = 32000 / 32 = 1000 Hz
    For 1 second timeout: 1000 ticks
*/
#define WATCHDOG_RELOAD_VALUE 1000

//------------------------------------------------------------------------------
// Private Data
//------------------------------------------------------------------------------

/// IWDG handle for the hardware watchdog peripheral
static IWDG_HandleTypeDef iwdgHandle_;

/// Flag indicating if watchdog has been initialized and enabled
static bool watchdogEnabled_ = false;

//------------------------------------------------------------------------------
// HAL Implementation
//------------------------------------------------------------------------------

bool Kit_System_Watchdog_hal_enable_wdog( void )
{
    // Only enable once
    if ( watchdogEnabled_ )
    {
        return true;
    }

    // Enable LSI (Low Speed Internal) oscillator for IWDG
    __HAL_RCC_LSI_ENABLE();

    // Wait for LSI to be ready
    uint32_t timeout = 1000;
    while ( __HAL_RCC_GET_FLAG( RCC_FLAG_LSIRDY ) == RESET )
    {
        if ( --timeout == 0 )
        {
            return false;  // LSI failed to start
        }
    }

    // Configure IWDG
    iwdgHandle_.Instance  = IWDG;
    iwdgHandle_.Init.Prescaler = WATCHDOG_PRESCALER;
    iwdgHandle_.Init.Reload    = WATCHDOG_RELOAD_VALUE;

    // Initialize IWDG (this also starts it)
    if ( HAL_IWDG_Init( &iwdgHandle_ ) != HAL_OK )
    {
        return false;  // Initialization failed
    }

    watchdogEnabled_ = true;
    return true;
}

void Kit_System_Watchdog_hal_kick_wdog( void )
{
    if ( watchdogEnabled_ )
    {
        // Refresh the IWDG counter to prevent reset
        HAL_IWDG_Refresh( &iwdgHandle_ );
    }
}

void Kit_System_Watchdog_hal_trip_wdog( void )
{
    // To trip the watchdog, we simply stop refreshing it.
    // The IWDG will timeout and reset the MCU.
    // We enter an infinite loop to ensure we don't accidentally refresh it.
    
    // Disable interrupts to prevent any other code from running
    __disable_irq();

    // Infinite loop - watchdog will expire and reset the MCU
    while ( 1 )
    {
        // Wait for watchdog to timeout and reset the system
        __NOP();
    }
}
