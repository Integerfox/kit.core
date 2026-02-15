/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include <inttypes.h>

static inline void init_led()
{
    // Initialize the LED pin
    gpio_init( PICO_DEFAULT_LED_PIN );

    // Set the LED pin to output mode
    gpio_set_dir( PICO_DEFAULT_LED_PIN, GPIO_OUT );
}

int main()
{
    stdio_init_all();
    init_led();

    while ( true )
    {
        uint32_t now = (uint32_t)( to_ms_since_boot( get_absolute_time() ) );
        printf( "Hello, world! Again. now=%" PRIu32 "\n", now );
        gpio_xor_mask(1U << PICO_DEFAULT_LED_PIN);
        sleep_ms( 500 );
        gpio_xor_mask(1U << PICO_DEFAULT_LED_PIN);
        sleep_ms( 500 );
    }
}
