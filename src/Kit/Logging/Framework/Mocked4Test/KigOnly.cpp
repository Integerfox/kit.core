/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "KitOnly.h"
#include "Kit/Logging/Framework/Logger.h"

//------------------------------------------------------------------------------
namespace Kit {
namespace Logging {
namespace Framework {

// Access to Whitebox variable
extern uint32_t g_vlogfCallCount;
extern bool     g_queueFull;
extern uint16_t g_overflowCount;

namespace Mocked4Test {

////////////////////////////////////////////////
KitOnly::KitOnly() noexcept
{
    Framework::initialize( *this /*, m_logFifo, */ );
}

const char* KitOnly::classificationIdToString( uint8_t classificationId ) noexcept
{
    return Kit::Type::betterEnumToString<Kit::Logging::Pkg::ClassificationId, uint8_t>(
        classificationId,
        OPTION_KIT_LOGGING_FRAMEWORK_UNKNOWN_CLASSIFICATION_ID_TEXT );
}

/// See Kit::Logging::Framework::IApplication
IPackage& KitOnly::getPackage( uint8_t packageId ) noexcept
{
    // Only supports the KIT Package
    return m_kitPackage;
}

////////////////////////////////////////////////
/// See Kit::Logging::Framework::Mocked4Test::WhiteBox
bool KitOnly::isLogQueueFull() const noexcept
{
    return g_queueFull;
}

/// See Kit::Logging::Framework::Mocked4Test::WhiteBox
uint32_t KitOnly::getOverflowedLogEntryCount() const noexcept
{
    return g_overflowCount;
}

} // end namespace
}
}
}
//------------------------------------------------------------------------------