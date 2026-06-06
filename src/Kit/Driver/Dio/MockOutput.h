#ifndef KIT_DRIVER_DIO_MOCKOUTPUT_H_
#define KIT_DRIVER_DIO_MOCKOUTPUT_H_
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


/** Mock digital output pin for unit testing.
    Tracks the current state and counts state transitions.
 */
class MockOutput : public IOutput
{
public:
    MockOutput( bool initialState = false ) noexcept
        : m_asserted( initialState )
        , m_assertCount( 0 )
        , m_deassertCount( 0 )
        , m_toggleCount( 0 )
    {
    }

public:
    /// See Kit::Driver::IStart
    bool start( void* startArgs = nullptr ) noexcept override { return true; }

    /// See Kit::Driver::IStop
    void stop() noexcept override {}


public:
    void assertPin() noexcept override
    {
        m_asserted = true;
        m_assertCount++;
    }

    void deassertPin() noexcept override
    {
        m_asserted = false;
        m_deassertCount++;
    }

    void toggle() noexcept override
    {
        m_asserted = !m_asserted;
        m_toggleCount++;
    }

    void set( bool asserted ) noexcept override
    {
        m_asserted = asserted;
        if ( asserted )
        {
            m_assertCount++;
        }
        else
        {
            m_deassertCount++;
        }
    }

    bool isAsserted() const noexcept override
    {
        return m_asserted;
    }


public:
    /// Resets all state and counters
    void reset() noexcept
    {
        m_asserted      = false;
        m_assertCount   = 0;
        m_deassertCount = 0;
        m_toggleCount   = 0;
    }


public:
    bool     m_asserted;       //!< Current logical state (true=asserted)
    uint32_t m_assertCount;    //!< Number of assert() calls
    uint32_t m_deassertCount;  //!< Number of deassert() calls
    uint32_t m_toggleCount;    //!< Number of toggle() calls
};


}  // end namespaces
}
}
#endif  // end header latch
