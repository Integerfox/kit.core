/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "pico/time.h"
#include <inttypes.h>
static inline void set_led( bool led_on )
{
    cyw43_arch_gpio_put( CYW43_WL_GPIO_LED_PIN, led_on );
}

int main()
{
    stdio_init_all();

    // LED is controlled by the cyw43 driver on the Pico W / Pico 2 W
    cyw43_arch_init();

    while ( true )
    {
        uint32_t now = (uint32_t)( to_ms_since_boot( get_absolute_time() ) );
        printf( "Hello, world! Again. now=%" PRIu32 "\n", now );
        set_led( true );
        sleep_ms( 500 );
        set_led( false );
        sleep_ms( 500 );
    }
}

