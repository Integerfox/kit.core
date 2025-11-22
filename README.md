# Coming soon.... 

## KIT C++ Library
The KIT Library is a comprehensive suite of C++ middleware specifically designed for developing microcontroller 
applications that “work” with any microcontroller and operating systems.  Or said another way, KIT is platform
agnostic.  Because the library is purpose built to be platform independent – the following features are “free” 
out-of-the-box:
* Automated unit tests that execute as terminal applications on Windows and Linux, i.e. CI/CD ready.  In practice
  over 80% of your application can be unit test this way.
* Functional simulator for your application on Day 0.  Since +80% of your application can be unit tested off
  target – building a function simulator is minimal effort (i.e. it is more of planning effort that a coding effort).
  * What is Day 0?  You can stand up the functional simulator before you have built, purchased, or even defined your
    hardware platform.  This means you can write and test production code as soon as the project starts. **No more
    gating firmware development by HW availability**.
* Moving to a new Microcontroller, Microprocessor, or new PCBA is a low-risk activity because none of the application’s business
  logic is dependent on hardware specific entities.  
  * This means that only the lowest drivers are impacted when transitioning to a new hardware platform.  For example,
    if you started the project with a function simulator your application already runs on least two platforms – so
    the mechanics of supporting yet another platform has already been defined/laid out.

## Sounds great, but what's the catch?
The catch is that at some point there has to be hardware specific code.  The KIT library has N hardware platforms 
that are used for its development and testing.  The list of hardware platforms and supported drivers, operating systems, 
file systems, etc. can be found here (**TBD**).  That said – the library probably does not have the hardware support for 
your specific project – which means you will have to develop the platform specific drivers or port your particular 
operating system, file system, etc. to work with the library.   This work is not on the critical path if you are using 
a functional simulator, nor is it rocket science; but developing, testing, and debugging a reliable driver is not 
always easy – especially when interrupts are involved – so don’t underestimate this effort. We are continually adding 
new hardware platforms and drivers as resources permit.

## KIT Feature list
*	**No dynamic memory allocation** used/required after start-up of an application.
* OSAL Layer (with several RTOS ports and supports bare-metal applications)
  * Mutexes
  * Semaphores
  * Event Flags
  * Sleep
  * Software Timers 
  * Elapsed Time
  * Periodic Scheduler
  * Watchdog
  * Fatal Error Handler
  * Thread Local Storage
* Command-Console/CLI
*	Streams (UART, Sockets, etc.) and File abstractions 
  * File Systems:
    * [Littlefs](https://github.com/littlefs-project/littlefs)
    * Windows
    * Linux
* Inter-Thread-Communications (ITC) via message passing.
*	[Limitless containers](https://patternsinthemachine.net/2022/09/limitless-containers-without-dynamic-memory-allocation-in-c-c/) with no dynamic memory allocation
*	[Data Model](https://patternsinthemachine.net/2022/12/data-model-introduction-to-the-data-model-architecture/) framework
*	JSON Parsing and formatting
* Text utilities
  *	Standardize set of C string library functions
  * String class with no dynamic memory allocation
  * String parsers, formatters, and tokenizers
  * Binary to text Encoding/Deconding
  * Text to/from binary utilities
*	Checksum and CRC functions
*	Logging and Tracing functions
*	Persistent Storage framework
*	Cryptographic abstractions
* Drivers
  * Stream based, blocking UART drivers
  * SPI
  * I2C
  * GPIO
  * Analog IO
  * EEPROM
  * Flash Block driver

*Note*: The above list is only a partial list
 
## Fine Print
The KIT library is planned to be a new and improved, extended version of the [CPL C++ class libary](https://github.com/johnttaylor/colony.core)
repository.  This means that while the KIT library is new (i.e. coming soon) it is built on top of an existing and proven code base.

