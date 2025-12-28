/*-----------------------------------------------------------------------------
* This file is part of the Colony.Core Project.  The Colony.Core Project is an
* open source project with a BSD type of licensing agreement.  See the license
* agreement (license.txt) in the top/ directory or on the Internet at
* http://integerfox.com/colony.core/license.txt
*
* Copyright (c) 2022  John T. Taylor
*
* Redistributions of the source code must retain the above copyright notice.
*----------------------------------------------------------------------------*/

#include "Kit/Bsp/Api.h"
#include "pico/stdio.h"
#include "pico/critical_section.h"
#include "pico/cyw43_arch.h"
#include <ios>

#define SECT_   "bsp"


critical_section_t g_bspGlobalCritSec_;

#ifdef USE_BSP_KIT_IO_STREAM
#define INIT_STDIO() Bsp_startConsoleStream()   // Accept default parameters: 115200, default TX/RX pins, 8N1
#else
#define INIT_STDIO() stdio_init_all()
#endif

///////////////////////////////////////////
void Bsp_initialize( void )
{
    // Initialize the global critical section
    critical_section_init( &g_bspGlobalCritSec_ );

    // Initialize STDIO
    INIT_STDIO();

    // Initialize the Wifi chip
    int err = cyw43_arch_init();
    if ( err )
    {
        printf( "**ERROR: WiFi init failed: %d", err );
    }
}


static bool debugLed1State_;
void Bsp_turnOnDebug1()
{
    debugLed1State_ = true;
    cyw43_arch_gpio_put( CYW43_WL_GPIO_LED_PIN, debugLed1State_ );
}

void Bsp_turnOffDebug1()
{
    debugLed1State_ = false;
    cyw43_arch_gpio_put( CYW43_WL_GPIO_LED_PIN, debugLed1State_ );
}

void Bsp_toggleDebug1()
{
    debugLed1State_ = !debugLed1State_;
    cyw43_arch_gpio_put( CYW43_WL_GPIO_LED_PIN, debugLed1State_ );
}


////////////////////////////////////////////////////
/*
** Method stubs to satisfy the linker -->NOTE: C++ Streams are NOT supported by this BSP
*/
std::ios_base::Init::Init()
{
}

std::ios_base::Init::~Init()
{
}
