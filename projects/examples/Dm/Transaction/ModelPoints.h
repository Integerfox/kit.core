#ifndef MP_TRANSACTION_MODEL_POINTS_H_
#define MP_TRANSACTION_MODEL_POINTS_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file defines all of the Model Points for Example application.

    All of the model points are placed into the 'mp' namespace.  The model
    point names map 1-to-1 with the instance names.

    The Application/Client is RESPONSIBLE for ensuring input values are with
    the defined range for the model point instance.
 */

 #include "Kit/Dm/ModelDatabase.h"
#include "Kit/Dm/Mp/Bool.h"
 #include "Dm/Transaction/MpFoo.h"
///
namespace mp {

/*---------------------------------------------------------------------------*/
/** This model point is used to trigger a client/server transaction.

    \b Units: struct

    \b Range: invalid             --> "idle"
              transition to valid -->Notifies the Server (of the request)
              updated while valid -->Notifies the Client (of the response)
              

    \b Notes:
        The application is responsible for ensuring that there is only at most
        one transaction 'in-flight'at any given time.
*/
extern Dm::Transaction::MpFoo trigger;

/** This model point is used to terminate the application

    \b Units: bool

    \b Range: invalid --> "running"
              true    --> "terminate" the application
              false   --> ignored/not-used
              
    \b Notes:
*/
extern Kit::Dm::Mp::Bool shutdownRequest;


/*---------------------------------------------------------------------------*/
/// The Application's Model Point Database
extern Kit::Dm::ModelDatabase   g_modelDatabase;

}  // end namespaces
#endif  // end header latch
