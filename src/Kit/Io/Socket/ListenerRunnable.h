#ifndef KIT_IO_SOCKET_LISTENER_H_
#define KIT_IO_SOCKET_LISTENER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "Kit/Io/Socket/IListener.h"
#include "Kit/Io/Types.h"
#include "Kit/System/IRunnable.h"
#include "Kit/System/Mutex.h"


/** This value is number of retries that is performed when attempting
    to bind to the listening port.
 */
#ifndef USE_KIT_IO_SOCKET_LISTENER_BIND_RETRIES
#define USE_KIT_IO_SOCKET_LISTENER_BIND_RETRIES 5
#endif

/** This value is time, in milliseconds between retries during the binding
    process.
 */
#ifndef USE_KIT_IO_SOCKET_LISTENER_BIND_RETRY_WAIT_MS
#define USE_KIT_IO_SOCKET_LISTENER_BIND_RETRY_WAIT_MS ( 10 * 1000 )  // 10 seconds
#endif

/** Delay time, in milliseconds, to wait for the Object's thread to exit
    after terminate() is called in the class's destructor.
 */
#ifndef USE_KIT_IO_SOCKET_LISTENER_TERMINATE_WAIT_MS
#define USE_KIT_IO_SOCKET_LISTENER_TERMINATE_WAIT_MS 100
#endif


///
namespace Kit {
///
namespace Io {
///
namespace Socket {


/** This concrete class provides an implementation of a simple listener using
    IPv4 addressing.  The listener requires a dedicated thread to run in (because
    it blocks waiting for incoming connection requests).

    The threading semantics of the client is defined/implemented by client
    instances passed to the startListening() method.  See the IListenerClient
    class for more details.

    NOTE: The application is responsible for creating the thread for the
          listener to run in.

          NOTE: Because the class uses blocking semantics waiting for incoming
          connection requests, how effective the terminate() call is to
          abort the blocking call is platform/implementation specific.  Or said
          another way, there is no guarantee that terminate() will stop the
          listener and that its thread self terminates.
 */
class ListenerRunnable : public IListener, public Kit::System::IRunnable
{
protected:
    /// Constructor
    ListenerRunnable() noexcept;

public:
    /// See Kit::Io::Socket::IListener
    void startListening( IListenerClient& client, int portNumToListenOn ) noexcept override;

    /// See Kit::Io::Socket::IListener
    void terminate() noexcept override;

protected:
    /// See Kit::System::Runnable
    void entry() noexcept override;

    /// See Kit::System::Runnable
    void pleaseStop() noexcept override;

protected:
    /// Begin listening -->to be implemented by platform specific child class
    virtual void listen() noexcept;

    /// Stop the listener -->to be implemented by platform specific child class
    virtual void stopListener() noexcept;

protected:
    /// Client
    IListenerClient* m_clientPtr;

    /// Mutex to guard start up sequence
    Kit::System::Mutex m_lock;

    /// Listening socket handle
    KitIoSocketHandle_T m_listeningSocket;

    /// Port Number to listen on
    int m_port;

    /// Indicates if the listener was instructed to start
    bool m_startCalled;
};


}  // end namespaces
}
}
#endif  // end header latch
