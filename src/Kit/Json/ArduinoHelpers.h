#ifndef KIT_JSON_ARDUINO_HELPERS_H_
#define KIT_JSON_ARDUINO_HELPERS_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    Contains a collection of 'helper' function (mostly for debugging) when
    using JsonArduino tool.
*/

#include "Kit/Json/Arduino.h"


///
namespace Kit {
///
namespace Json {

/** This method dumps the contents of the JSON object/array to the trace output.
    
    Note: This method uses the heap to temporarily allocate memory
 */
void dumpToTrace( const char* traceSection, const JsonVariant& srcObjToDump ) noexcept;



}       // end namespaces
}
#endif  // end header latch
