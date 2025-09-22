#ifndef KIT_ITC_IOPEN_H_
#define KIT_ITC_IOPEN_H_
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
namespace Itc {

/** This abstract class defines an open interface.  This open interface
    provides the ability to run an "initialize/open" routine for a
    ITC server object IN its execution thread (i.e. the thread that the
    ITC server object executes its ITC requests in).  The open call
    is a SYNCHRONOUS ITC call.  This has the following effects/requirements:

    1) The caller MUST be in a different thread than the thread of the object
       being  opened.

    2) The thread of the caller will block until the ITC server object
       has completed its initialization (i.e. called returnToSender() on
       the Open Request message).
 */
class IOpen
{
public:
    /** Synchronously opens/initializes the called object. Returns true
        if the open operation was successful
     */
    virtual bool open( void* args = nullptr ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IOpen() = default;
};


}      // end namespaces
}
#endif  // end header latch
