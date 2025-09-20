#ifndef Cpl_Itc_CloseSync_h_
#define Cpl_Itc_CloseSync_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Cpl/Itc/OpenSync.h"
#include "Cpl/Itc/CloseRequest.h"
#include "Cpl/Itc/CloseApi.h"

///
namespace Cpl {
///
namespace Itc {

/** This partially concrete class implements the synchronous ITC close
    call for the CloseApi.  The subclass is still required to implement the ITC
    request() method.

    NOTE: This class is derived from OpenSync -->if you support a close(),
          call, you better have had an open() call.
 */
class CloseSync : public OpenSync,
    public CloseApi,
    public CloseRequest
{
public:
    /// Constructor
    CloseSync( PostApi& myMbox ) noexcept;


public:
    /// See CloseSync
    bool close( void* args = nullptr );
};




};      // end namespaces
};
#endif  // end header latch
