#ifndef KIT_DRIVER_DIO_TESTSUPPORT_MOCKOUTPUT_H_
#define KIT_DRIVER_DIO_TESTSUPPORT_MOCKOUTPUT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Mock Digital Output driver for unit testing.  Tracks pin state changes.
*/


#include "Kit/Driver/Dio/IOutput.h"
#include <stdint.h>


///
namespace Kit {
///
namespace Driver {
///
namespace Dio {
///
namespace TestSupport {


/** Mock digital output pin for unit testing.
    Tracks the current state and counts state transitions.
 */
class MockOutput : public IOutput
{
public:
    MockOutput() noexcept
        : m_state( false )
        , m_setHighCount( 0 )
        , m_setLowCount( 0 )
        , m_toggleCount( 0 )
    {
    }

public:
    void setHigh() noexcept override
    {
        m_state = true;
        m_setHighCount++;
    }

    void setLow() noexcept override
    {
        m_state = false;
        m_setLowCount++;
    }

    void toggle() noexcept override
    {
        m_state = !m_state;
        m_toggleCount++;
    }

    void set( bool newState ) noexcept override
    {
        m_state = newState;
        if ( newState )
        {
            m_setHighCount++;
        }
        else
        {
            m_setLowCount++;
        }
    }

    bool getState() const noexcept override
    {
        return m_state;
    }


public:
    /// Resets all state and counters
    void reset() noexcept
    {
        m_state        = false;
        m_setHighCount = 0;
        m_setLowCount  = 0;
        m_toggleCount  = 0;
    }

    /// Returns the number of setHigh() calls
    uint32_t getSetHighCount() const noexcept { return m_setHighCount; }

    /// Returns the number of setLow() calls
    uint32_t getSetLowCount() const noexcept { return m_setLowCount; }

    /// Returns the number of toggle() calls
    uint32_t getToggleCount() const noexcept { return m_toggleCount; }


protected:
    bool     m_state;
    uint32_t m_setHighCount;
    uint32_t m_setLowCount;
    uint32_t m_toggleCount;
};


}  // end namespaces
}
}
}
#endif  // end header latch
