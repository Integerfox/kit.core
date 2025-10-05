/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include <winsock2.h>
#include "Kit/System/PrivateStartup.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/FatalError.h"

/////////////////////////////////////////////////////////////////////////////
// Use the KIT startup and shutdown hooks to initialize the Winsock library

// Anonymous namespace
namespace {

////
class ExitHandler : public Kit::System::Shutdown::IHandler
{
protected:
    ///
    int notify( int exit_code ) noexcept override
    {
        WSACleanup();
        return exit_code;
    }
};


////
class RegisterInitHandler : public Kit::System::IStartupHook
{
protected:
    ///
    ExitHandler m_shutdown;

public:
    ///
    RegisterInitHandler()
        : IStartupHook( MIDDLE_WARE )
    {
    }


protected:
    ///
    void notify( InitLevel init_level )
    {
        WSADATA wsaData;

        if ( WSAStartup( 0x202, &wsaData ) == SOCKET_ERROR )
        {
            int err = WSAGetLastError();
            WSACleanup();
            Kit::System::FatalError::logf( Kit::System::Shutdown::eSTREAMIO, "WSAStartup failed with error: %d", err );
        }

        // Register my shutdown handler
        Kit::System::Shutdown::registerHandler( m_shutdown );
    }
};


}  // end namespace

///
static RegisterInitHandler autoRegister_systemInit_hook;