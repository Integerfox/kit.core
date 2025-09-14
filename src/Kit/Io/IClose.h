#ifndef KIT_IO_ICLOSE_H_
#define KIT_IO_ICLOSE_H_
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
namespace Kit {
///
namespace Io {

/** This abstract class defines a close operation that is intended to be used
    Input and Output streams/files.  Since InputOutput streams/files are
    supported we end up with 2 close() method when the InputOutput class
    inherits from Input and Output interfaces.  This causes basically a
    'diamond' problem.  By making the close() it owns interface and a parent
    class - we can use the 'virtual mechanism' in C++ to ensure that for
    InputOutput classes there is one and only one close() method.
 */
class IClose
{
public:
    /** This method will close the stream.  The result of closing the stream is
        dependent on the actual concrete stream/platform.
     */
    virtual void close() noexcept = 0;


public:
    /// Lets the make the destructor virtual
    virtual ~IClose() noexcept= default;

};

}       // end namespaces
}
#endif  // end header latch
