# Platform Support {#platform_support}

\brief Integrated Platforms. The KIT library is fundamentally hardware/platform
independent.  That said, here is the list of platforms that the KIT library
provides direct integration and/or is tested with.

Kit provides comprehensive support across multiple platforms, from embedded microcontrollers to desktop environments.

[TOC]

@note **⚠️ UNDER CONSTRUCTION** - This documentation is currently being updated.

## IDE

The KIT library does not require or enforce any specific IDE, i.e. you can use
your IDE or editor of choice.  That said there are optional scripts/functionality
that **is** IDE specific.  For example:

- Helper scripts to launch the Segger's Ozone debugger
- Support for VS Code's Clang intellisense and auto-formatting.

If you contribute the KIT library - it is recommended that you use [VS Code](https://code.visualstudio.com/download).

## Host/Build Platforms

The KIT library supports the following Host platforms and compiler toolchains
for building and running its automated unit tests on.

| Host | Runtime Target | Compiler | Debugger |
|---------|---------|---------|---------|
| Linux | Posix | Native GCC | Native GDB |
| Linux | ARM7 Cortex R/M | GCC arm-none-eabi | [JLink/Segger Ozone](https://www.segger.com/debug-trace-embedded-systems/) |
| Windows | Windows | Clang-MSVC | Visual Studio IDE |
| Windows | Windows | Visual Studio | Visual Studio IDE |
| Windows | ARM7 Cortex R/M | GCC arm-none-eabi | [JLink/Segger Ozone](https://www.segger.com/debug-trace-embedded-systems/) |

## OSAL Implementations

The KIT library's OSAL layer has been ported to run on the following platforms/OS

| Operating System | Description/Notes |
|---------|---------|
| Bare Metal | No underlying Operating System and the application is a 'super loop' |
| POSIX | Any POSIX complaint operating system.  Note: It has only been validate on Linux (and some limited testing with MacOS) |
| Windows | Windows 10 or higher operating system |
| FreeRTOS | [Real-Time Operating System](https://www.freertos.org/) (RTOS) for Microcontrollers. Support for multi-core (SMP) is planned by **not** yet implemented |

@ref porting_guide "Porting Guide" - Step-by-step instructions for porting KIT's OSAL.

## Runtime Platforms

The following table is list of currently supported Runtime Platforms.  *Supported*
in this context means there are unit tests and/or example projects in the
repository.

| Platform | Description/Notes |
|---------|---------|
| POSIX | Multi-thread application. Typical use case is for off-target unit testing, but can also be used for embedded Linux development. |
| Windows | Multi-thread application. Typical use case is for off-target unit testing. Can be used for Windows Console applications. |
| [ST NUCLEO-F413ZH](https://www.st.com/en/evaluation-tools/nucleo-f413zh.html) Evaluation board | The F413ZH is a ARM Cortex-M4 microcontroller. Includes a basic BSP and drivers support for the ST's STM32F4 family of microcontrollers. |

Planned/comming platforms.

| Platform | Description/Notes |
|---------|---------|
| [Adafruit Grand Central M4 Express](https://www.adafruit.com/grandcentral)| The board contains a Microchip ATSAMD51 microcontroller. The included BSP supports and is integrated with the Arduino framework. |
| [PICO/PICOW](https://www.raspberrypi.com/products/raspberry-pi-pico/) |  Raspberry PI RP2040 with and without WIFI. Also includes port of the KIT OSAL to directly run on the MCU's dual core where each core runs a single thread. |
| [PICO2/PICO2W](https://www.raspberrypi.com/products/raspberry-pi-pico-2/) |  Raspberry PI RP2350 with and without WIFI. Also includes port of the KIT OSAL to directly run on the MCU's dual core where each core runs a single thread. TBD if the RISC cores are supported.|

## See Also

- @ref examples "Code Examples" - Platform-specific examples
- @ref getting_started "Getting Started Guide"
- [API Reference](namespaces.html)
