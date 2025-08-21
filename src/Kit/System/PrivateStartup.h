#ifndef KIT_SYSTEM_PRIVATE_STARTUP_H_
#define KIT_SYSTEM_PRIVATE_STARTUP_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/ListItem.h"


/// 
namespace Kit {
/// 
namespace System {


/** This class defines a start-up handler/hook that allows a KIT Library
    sub-system to register to be called when the Api::initialize()
    method is called.

    The intended usage of this interface is that a sub-system creates a
    sub-class of this class and then creates a static instance of its sub-class.
    During the init process, the instance's 'notifyInit' method will be called.

    NOTE: This class/interface SHOULD ONLY be used by KIT Library interfaces,
          the application should NEVER register a startup hook!
 */
class StartupHook : public Kit::Container::ListItem
{
public:
    /** This enum defines 'init_levels'.  The init levels are processed
        in order (lowest value to highest), i.e. callbacks for lowest
        numbered init level are called first and the callback for highest
        numbered init level are called last.  Within a init level there is
        no guaranteed order to the callbacks.
     */
    enum InitLevel_e
    {
        TEST_INFRA=0,    /// Initialized first
        SYSTEM,
        MIDDLE_WARE,
        APPLICATION
    }; /// Initialized last


public:
    /** This method is called as part of the KIT Library initialize()
        process. The 'initLevel' informs the client what initialize
        level context the notifyInit() method is being called.
     */
    virtual void notify( InitLevel_e initLevel ) = 0;


protected:
    /** Base Class constructor -->performs the callback/init-level
        registration
     */
    StartupHook( InitLevel_e myInitLevel );

public:
    /// Ensure the destructor is virtual
    virtual ~StartupHook() {}


public:
    /** This is used to register instance of this class.  This method is
        NOT implemented by the 'client sub-system' - it is a singleton that
        is implemented Kit::System::Startup classes.

        NOTE: This is the ONE KIT Library method that can be called BEFORE
              Kit::System::Api::initialize() is called.
     */
    static void registerHook( StartupHook& callbackInstance, InitLevel_e initOrder );

    /** This method is intended to be USED ONLY by the Kit::System::Api::init()
        method to trigger all of the registered init callbacks.
     */
    static void notifyStartupClients( void );
};


}       // end namespaces
}
#endif  // end header latch

