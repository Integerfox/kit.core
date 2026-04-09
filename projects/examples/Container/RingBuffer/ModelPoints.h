#ifndef CONTAINER_RINGBUFFER_MODELPOINTS_H_
#define CONTAINER_RINGBUFFER_MODELPOINTS_H_
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
#include "Kit/Dm/Mp/Uint32.h"

///
namespace mp {

/*---------------------------------------------------------------------------*/
/** This model point contains the current count of the number of elements
    in the Flash Codes Ring Buffer. The MP is used to generate change notifications
    when the number of elements in the buffer changes.

    \b Units: count of elements

    \b Range: invalid  -->No items have ever been added to the Ring Buffer
              valid    -->number of current elements in the Ring Buffer. 

    \b Notes:
        n/a
*/
extern Kit::Dm::Mp::Uint32 flashCodeQueueElementCount;



/*---------------------------------------------------------------------------*/
/// The Application's Model Point Database
extern Kit::Dm::ModelDatabase g_modelDatabase;

}  // end namespaces
#endif  // end header latch
