/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "c_assert.h"
#include "Kit/System/FatalError.h"

void Kit_System_assert_c_wrapper( bool true_expression, const char* file, unsigned line, const char* func )
{
    if ( !true_expression )
    {
        Kit::System::FatalError::logf( Kit::System::Shutdown::eASSERT,
                                       "ASSERT Failed at: file=%s, line=%d, func=%s\n",
                                       file,
                                       line,
                                       func );
    }
}
