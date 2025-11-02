/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "InputOutput.h"

///
using namespace Kit::Io::Serial::ST::M32F4;



////////////////////////////////////
InputOutput::InputOutput( Kit::Container::RingBuffer<uint8_t>& txBuffer,
                          Kit::Container::RingBuffer<uint8_t>& rxBuffer
)
    :m_driver( txBuffer, rxBuffer )
{
}


InputOutput::~InputOutput( void )
{
    close();
}


void InputOutput::start( IRQn_Type           uartIrqNum,
                         UART_HandleTypeDef* uartHdlToUse ) noexcept
{
    m_driver.start( uartIrqNum, uartHdlToUse );
}


////////////////////////////////////
bool InputOutput::read( void* buffer, ByteCount_T numBytes, ByteCount_T& bytesRead )
{
    return m_driver.read( buffer, numBytes, bytesRead );
}

bool InputOutput::available()
{
    return m_driver.available();
}

size_t InputOutput::getRxErrorsCounts( bool clearCount ) noexcept
{
    return m_driver.getRXErrorsCounts( clearCount );
}


////////////////////////////////////
bool InputOutput::write( const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten )
{
    bytesWritten = maxBytes;
    return m_driver.write( buffer, (size_t) maxBytes );
}


void InputOutput::flush()
{
    // Not supported/has no meaning for a serial port
}

bool InputOutput::isEos()
{
    // Does not really have meaning for serial port (assuming the serial port is opened/active)
    return false;
}

void InputOutput::close()
{
    m_driver.stop();
}
