#ifndef KIT_IO_IEOS_H_
#define KIT_IO_IEOS_H_
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

/** This abstract class defines a isEos() operation that is intended to be used
    Input and Output streams.  Since InputOutput streams are supported we end up
    with 2 isEos() methods when the InputOutput class inherits from Input and
    Output interfaces.  This causes basically a 'diamond' problem.  By making
    the isEof() it owns interface and a parent class - we can use the 'virtual
    mechanism' in C++ to ensure that for InputOutput classes there is one and
    only one isEos() method.
 */
class IEos
{
public:
    /** This method returns true if End-of-Stream was encountered on the stream.
     */
    virtual bool isEos() const = 0;


public:
    /// Lets the make the destructor virtual
    virtual ~IEos() noexcept = default;
};

}  // end namespaces
}
#endif  // end header latch
