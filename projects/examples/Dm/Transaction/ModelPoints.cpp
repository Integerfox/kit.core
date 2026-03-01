/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "ModelPoints.h"


// Helper macros to simplify the instantiation of the MP instances
#define ALLOC_INVALID( t, n )                t mp::n( mp::g_modelDatabase, ( #n ) )
#define ALLOC_INVALID_ALERT( t, n, en, pri ) t mp::n( mp::g_modelDatabase, ( #n ), en, (uint8_t)pri )

/// Static allocate the MP database
Kit::Dm::ModelDatabase mp::g_modelDatabase( "ignoreThisParameter_usedToCreateAUniqueConstructor" );

/*----------------------------------------------------------------------------*/
ALLOC_INVALID( Dm::Transaction::MpFoo, trigger );
ALLOC_INVALID( Kit::Dm::Mp::Bool, shutdownRequest );