#ifndef EXAMPLES_JOB_TEMPERATURE_MODEL_POINTS_H_
#define EXAMPLES_JOB_TEMPERATURE_MODEL_POINTS_H_
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
#include "Kit/Dm/Mp/Float.h"

///
namespace mp {

/*---------------------------------------------------------------------------*/
/** This model point contains a "published" (from a hypothetical) driver
    layer that is sampling an analog temperature sensor.

    \b Units: Degrees Celsius

    \b Range: -40.0 to 125.0

    \b Notes:
*/
extern Kit::Dm::Mp::Float tempSensor1;

/** This model point contains a "published" (from a hypothetical) driver
    layer that is sampling an analog temperature sensor.

    \b Units: Degrees Celsius

    \b Range: -40.0 to 125.0

    \b Notes:
*/
extern Kit::Dm::Mp::Float tempSensor2;

/*---------------------------------------------------------------------------*/
/// The Application's Model Point Database
extern Kit::Dm::ModelDatabase g_modelDatabase;

}  // end namespaces
#endif  // end header latch
