#ifndef KIT_LOGGING_FRAMEWORK_0TEST_STATEFULAPP_H_
#define KIT_LOGGING_FRAMEWORK_0TEST_STATEFULAPP_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/Framework/IApplication.h"
#include <stdint.h>


/** Test-only IApplication that opts into the 'application state' feature.  The
    KIT library itself has no concept of application state - so the mocked
    KitOnly application intentionally does NOT implement it.  All of the
    non-state IApplication behavior is delegated to the supplied instance.
 */
class StatefulApp4Test : public Kit::Logging::Framework::IApplication
{
public:
    /// Example Application defined state bit
    static constexpr KitLoggingApplicationState_T STATE_TAINTED = 0x01;

    /// Marker text rendered when STATE_TAINTED is set
    static constexpr const char* STATE_TAINTED_TEXT = "*";

public:
    /// Constructor
    StatefulApp4Test( Kit::Logging::Framework::IApplication& delegate ) noexcept
        : m_delegate( delegate )
        , m_state( 0 )
    {
    }

public:
    /// See Kit::Logging::Framework::IApplication
    bool isClassificationIdValid( uint8_t classificationId ) noexcept override
    {
        return m_delegate.isClassificationIdValid( classificationId );
    }

    /// See Kit::Logging::Framework::IApplication
    const char* classificationIdToString( uint8_t classificationId ) noexcept override
    {
        return m_delegate.classificationIdToString( classificationId );
    }

    /// See Kit::Logging::Framework::IApplication
    Kit::Logging::Framework::IPackage* getPackage( uint8_t packageId ) noexcept override
    {
        return m_delegate.getPackage( packageId );
    }

    /// See Kit::Logging::Framework::IApplication
    KitLoggingApplicationState_T captureApplicationState() noexcept override
    {
        return m_state;
    }

    /// See Kit::Logging::Framework::IApplication
    const char* applicationStateToString( KitLoggingApplicationState_T state ) noexcept override
    {
        return ( state & STATE_TAINTED ) ? STATE_TAINTED_TEXT : "";
    }

public:
    /// Sets the state that is reported by captureApplicationState()
    void setApplicationState( KitLoggingApplicationState_T newState ) noexcept { m_state = newState; }

protected:
    /// Provides the non-state portions of the IApplication interface
    Kit::Logging::Framework::IApplication& m_delegate;

    /// Current application state
    KitLoggingApplicationState_T m_state;
};

#endif  // end header latch
