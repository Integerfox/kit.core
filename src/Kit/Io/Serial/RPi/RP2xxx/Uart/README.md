# Kit::Io::Serial::RPi::RP2xxx::Uart
@brief namespace description for Kit::Io::Serial::RPi::RP2xxx::Uart
@namespace Kit::Io::Serial::RPi::RP2xxx::Uart @brief


The Uart namespace implements the Kit::Io streams utilizing the Raspberry PI 
Pico C/C++ SDK's UART interfaces.  The implement provide a more robust stream 
interface than using Kit::Io::Serial::RPi::RP2xxx::Stdio because it is interrupted 
driven and provides software TX/RX FIFOs.
