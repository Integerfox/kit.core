/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/Assert.h"
#include "KitOnly.h"
#include "Kit/Logging/Framework/Logger.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Logging {
namespace Framework {

// Access to Whitebox variable
extern uint32_t g_vlogfCallCount;
extern bool     g_queueOverflowed;
extern uint16_t g_overflowCount;
extern void     resetLoggerState() noexcept;

namespace Mocked4Test {

////////////////////////////////////////////////
KitOnly::KitOnly() noexcept
{
    Framework::initialize( *this, m_logFifo );
}

const char* KitOnly::classificationIdToString( uint8_t classificationId ) noexcept
{
    return Kit::Type::betterEnumToString<Kit::Logging::Pkg::ClassificationId, uint8_t>(
        classificationId,
        OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_CLASSIFICATION_ID_TEXT );
}

IPackage& KitOnly::getPackage( uint8_t packageId ) noexcept
{
    // Only supports the KIT Package
    return m_kitPackage;
}

////////////////////////////////////////////////
bool KitOnly::isLogQueOverflowed() const noexcept
{
    return g_queueOverflowed;
}

uint32_t KitOnly::getOverflowedLogEntryCount() const noexcept
{
    return g_overflowCount;
}

void KitOnly::reset() noexcept
{
    Framework::resetLoggerState();
    m_logFifo.clearTheBuffer();
}

uint32_t KitOnly::getVLogfCallCount() const noexcept
{
    return g_vlogfCallCount;
}


}  // end namespace
}
}
}
//------------------------------------------------------------------------------