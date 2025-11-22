#ifndef KIT_STATESMITH_CONTEXTMYFSM_H_
#define KIT_STATESMITH_CONTEXTMYFSM_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "MyFsm.h"  // Note: Since this example code - not following KIT's coding standard for using paths in #include statements.
#include "IWidget.h"

///
namespace Foo {
///
namespace Bar {

/** This class implements the 'Context' for the MyFSM State Diagram

    This interface is NOT thread safe.
 */
class ContextMyFsm : public MyFsm, public IWidget
{
public:
    /// Constructor
    ContextMyFsm() = default;

public:
    void start() noexcept override 
    {
        startFsm();
    }
    void tick() noexcept override 
    {
        dispatchEvent( EventId::DO );
    }
    void notifyAbcChanged() noexcept override 
    {
        dispatchEvent( EventId::EVENT1 );
    }
    void notifyXyzStarted() noexcept override 
    {
        dispatchEvent( EventId::EVENT2 );
    }
    void notifyXyzCompleted() noexcept override 
    {
        dispatchEvent( EventId::EVENT3 );
    }

protected:
    /// Action Method
    void ActionA() noexcept override { printf( "ActionA\n" ); }

    /// Action Method
    void ActionB() noexcept override { printf( "ActionB\n" ); }

    /// Action Method
    void ActionC() noexcept override { printf( "ActionC\n" ); }

    /// Action Method
    void ActionD() noexcept override { printf( "ActionD\n" ); }

    /// Action Method
    void ActionE() noexcept override { printf( "ActionE\n" ); }

    /// Action Method
    void ActionF() noexcept override { printf( "ActionF\n" ); }

    /// Action Method
    void ActionG() noexcept override { printf( "ActionG\n" ); }

    /// Action Method
    void ActionH() noexcept override { printf( "ActionH\n" ); }


protected:
    /// Guard Method
    bool guard1() noexcept override
    {
        printf( "guard1==true\n" );
        return true;
    }
};


}  // end namespaces
}
#endif  // end header latch
