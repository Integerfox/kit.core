/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Container/SList.h"
#include "PrivateStartup.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace System {

///
static Kit::Container::SList<IStartupHook> levelTest_( "invoke_special_static_constructor" );
static Kit::Container::SList<IStartupHook> levelSystem_( "invoke_special_static_constructor" );
static Kit::Container::SList<IStartupHook> levelMiddleWare_( "invoke_special_static_constructor" );
static Kit::Container::SList<IStartupHook> levelApplication_( "invoke_special_static_constructor" );


////////////////////////////////////////////////////////////////////////////////
IStartupHook::IStartupHook( InitLevel myInitLevel ) noexcept
{
    registerHook( *this, myInitLevel );
}


void IStartupHook::registerHook( IStartupHook& callbackInstance, InitLevel initOrder ) noexcept
{
    // NOTE: The assumption is that sub-system register at the time of when
    //       constructor for static object execute - which is before main()
    //       is enter so there should be only one thread.
    if ( initOrder == IStartupHook::TEST_INFRA )
    {
        levelTest_.put( callbackInstance );
    }
    else if ( initOrder == IStartupHook::SYSTEM )
    {
        levelSystem_.put( callbackInstance );
    }
    else if ( initOrder == IStartupHook::MIDDLE_WARE )
    {
        levelMiddleWare_.put( callbackInstance );
    }
    else
    {
        levelApplication_.put( callbackInstance );
    }
}

void IStartupHook::notifyStartupClients( void ) noexcept
{
    // NOTE: The assumption is that no thread protection is required since
    //       there should only be one caller to the initialize() method.

    // Do TEST_INFRA level first
    IStartupHook* ptr = levelTest_.get();
    while ( ptr != 0 )
    {
        ptr->notify( TEST_INFRA );
        ptr = levelTest_.get();
    }

    // Do SYSTEM level next
    ptr = levelSystem_.get();
    while ( ptr != 0 )
    {
        ptr->notify( SYSTEM );
        ptr = levelSystem_.get();
    }

    // Do MIDDLE_WARE level next
    ptr = levelMiddleWare_.get();
    while ( ptr != 0 )
    {
        ptr->notify( MIDDLE_WARE );
        ptr = levelMiddleWare_.get();
    }

    // Do APPLICATION level last
    ptr = levelApplication_.get();
    while ( ptr != 0 )
    {
        ptr->notify( APPLICATION );
        ptr = levelApplication_.get();
    }
}

} // end namespaces
}
//------------------------------------------------------------------------------