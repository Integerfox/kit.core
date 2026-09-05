#ifndef ITC_ASYNCHRONOUS_EXAMPLE_H_
#define ITC_ASYNCHRONOUS_EXAMPLE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

///
namespace Itc {
///
namespace Asynchronous {

/** Entry point for the Itc Asynchronous example. Assumes that the KIT library
    has been initialized and the OSAL's scheduler is 'running'
 */
int runExample( unsigned numCycles = 1000 ) noexcept;

}  // end namespace(s)
}
#endif  // end header latch