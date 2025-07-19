# Call for contributors
I am reaching out for contributors/volunteers to the KIT project.  If you are interested
please email me at john.t.taylor@gmail.com.  

## I am looking for help with:
* Support/port additional hardware “reference” platforms.  I have limited number of boards (ST NUCLEO-F413ZH, ST NUCLEO-F767ZI, Adafruit Grand Central (SAMD51), Raspberry PI PICO and PICO2).
  * For a new hardware platform - at a minimum this requires developing a stream based blocking UART driver and porting the RTOS the KIT library’s OSAL (I have porting guide for this).  
    * Optional support would be things like I2C and SPI drivers for the new board.
  * Any board/MCU/RTOS/no-RTOS is acceptable as long as the board is currently available for purchase.
* Feature development in general
* Feedback/input on improvements to the original CPL library 
* Feedback for features/functionality you would like to see in the KIT library

## Work items for the initial "release" of KIT
Note: The list is tenative and is in no particular order
* Provide example projects (probably can “re-cycle” existing unit test is many cases) for each “feature” (e.g. containers, data model, checksums, ITC, Mapp, TShell, etc.)
* Remove the top level tests/ directory tree place build directories under the src/ tree inside the individual _0test/ directories.
* Investigate different mechanisms for setting up the compiler environments (i.e. the env.bat and env.sh scripts)
* NQBP – more support for VSCode
  * e.g. Debug on Windoze using MingW (launch and attach)
* Drivers: Refactor all driver to use pure virtual interfaces (in practice this is the most flexible and functional the same as using the c-function-pointers approach for writing decoupled drivers)
  * Refactor to have common start()/stop() interface for driver APIs such that a concrete class can implemented multiple driver interfaces – but still only implement a single set of start()/stop() methods (aka. Virtual inheritance to resolve the diamond inheritance)
* Refactoring Logging to allow application/sub-system defined identifiers to be created as needed.  The current paradigm does not scale.
* Revisit how Catch2 is built. At minimum “clean-up” that there is an xsrc/catch2/ and src/Catch/ directories
* OSAL – watchdog infrastructure – to monitor each event thread – where each thread can have a different timebase.

## Other posible work items
* OSAL: Additional RTOSes
* Additional File Systems
* Integrate lvgl (or other Graphic libraries)
* Networking: Integrate lwIP for Cpl::Io::Socket interface
* Networking: TLS support – what does this mean for the Cpl::Io::Socket interfaces?
* Networking: Add abstraction/support for UDP
* Networking: Revisit “socket” API (can they be simpler?)
* Networking: Abstractions for other protocol stacks (e.g. BLE, ZigBee, etc.)
* Drivers: Revisit and expand the Crypto drivers
* Drivers: New/Add 
  * block Flash driver to support the Driver::NV::Api interface without using a file system
  * analog drivers
  * others??
* Drivers: Expand Wifi support
* Integrate State Machine tools (e.g. StateSmith)
* Improvements to the CI/release processes...
