/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/Api.h"
#include "Kit/System/Trace.h"
#include "Kit/Driver/Flash/_testsupport/MockFlash.h"
#include "Kit/Driver/Flash/_0test/test.h"

using namespace Kit::Driver::Flash;

// 128KB flash, 4KB sectors, 256B write pages -- exercises all tests including multi-sector erase
static constexpr size_t MOCK_FLASH_SIZE = 128 * 1024;

int main()
{
    Kit::System::initialize();
    KIT_SYSTEM_TRACE_ENABLE();
    KIT_SYSTEM_TRACE_ENABLE_SECTION( "_0test" );
    KIT_SYSTEM_TRACE_SET_INFO_LEVEL( Kit::System::TraceLevel::eINFO );

    MockFlash<MOCK_FLASH_SIZE> flash;
    flash.start();
    return runtests( flash ) ? 0 : 1;
}
