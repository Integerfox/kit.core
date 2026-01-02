# Kit::Io::Serial::RPi::RP2xxx::Stdio
@brief namespace description for Kit::Io::Serial::RPi::RP2xxx::Stdio
@namespace Kit::Io::Serial::RPi::RP2xxx::Stdio @brief

The Stdio namespace implements the Kit::Io streams utilizing the Raspberry PI 
Pico C/C++ SDK's stdio interfaces.  The SDK's stdio interface supports using a 
physical UART on the MCU, or a virtual UART over USB (i.e. as a CDC USB device).
Which physical media is used is a compile/build time decision.

The Pico C/C++ SDK does NOT DEFINE any semantics with respect to buffering
of input/output data over stdio.  For example, when using the UART for
stdin - there is no background ISR that is caching incoming data bytes
until a read() method is called.  However, the USB implementation of stdin
does do some internal caching (how much caching is unclear).  
      
**BUYER BEWARE** (or use the Kit::Io::Serial::RPi::RP2xxx::Uart implementation instead)

*NOTE: The implement does NOT support the blocking semantics of the Kit::Io
      streams.  The design decision was for the implementation to be compatible/
      useful on bare-metal systems, i.e. no threads or KIT OSAL required*