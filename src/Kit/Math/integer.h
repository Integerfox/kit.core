#ifndef Cpl_Math_integer_h_
#define Cpl_Math_integer_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file

    This file contains a collection of methods comparing, manipulating, etc.
    floating binary integers

*/

#include <stdlib.h>
#include <stdint.h>


///
namespace Kit {
///
namespace Math {


/** This method treats the binary array a single 'large integer' and increment
    the array/large-integer by N.  N is limited to [0,255].

    If the 'littleEndian' flag is set to the true, then the increment starts 
    with the first byte in the array; else the increment starts with the last 
    byte in the array.
 */
void incrementLargeInteger( uint8_t* integerToBeIncrement, int integerSizeInBytes, uint8_t incValue=1, bool littleEndian=true ) noexcept;


}       // end namespaces
}
#endif  // end header latch
