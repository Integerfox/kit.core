#ifndef KIT_TYPE_TRAVERSER_H_
#define KIT_TYPE_TRAVERSER_H_
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
namespace Type {

/** This class defines a set of status(s) that are used with the traverser design
    pattern.  The traverser pattern is an adaptation of a internal iterator.
 */
class Traverser
{
public:
    /** Return codes (for the callback method) that determine if the traversal
        should continue.
     */
    enum Status_T {
        eABORT    = 0, //!< Indicates the client wants to cancel/terminate the traversal
        eCONTINUE = 1  //!< Indicates the client wants to continue the traversal
    };

public:
    /** This constructor does NOTHING.  It is work-around/hack to get Doxygen
        to properly document the enum above when there is ONLY a enum in the
        class.
     */
    Traverser() = default;
};

}       // end namespaces
}
#endif  // end header latch

