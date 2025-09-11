#ifndef KIT_SYSTEM_SHUTDOWN_H_
#define KIT_SYSTEM_SHUTDOWN_H_
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
#include "Kit/Container/ListItem.h"


/** Starting value for KIT Library's exit codes.  It is strongly recommended that
    applications do NOT override this value.
 */
#ifndef OPTION_KIT_SYSTEM_SHUTDOWN_BEGIN_EXIT_CODES
#define OPTION_KIT_SYSTEM_SHUTDOWN_BEGIN_EXIT_CODES 0
#endif

/** Total number of reserved exit codes for the KIT Library.  The application
    can define its own exit codes starting at:
    OPTION_KIT_SYSTEM_SHUTDOWN_BEGIN_EXIT_CODES + KIT_SYSTEM_SHUTDOWN_NUM_RESERVED_EXIT_CODES
*/
constexpr int KIT_SYSTEM_SHUTDOWN_NUM_RESERVED_EXIT_CODES = 100;


namespace Kit {
///
namespace System {


/** This class defines methods for forcibly terminating the application.
    How gracefully (or not) the shutdown is dependent on the actual platform
    implementation. In addition, what 'shutdown' means is also platform
    dependent, e.g. on an embedded platform it may invoke a reboot and/or
    restart of the application.

    Since this is a forcibly shutdown there are no guaranties with respect to
    releasing resources.

    The interface does provide for callback method(s) to be called on shutdown.
    These methods are guaranteed to be called (or at least attempted to be
    called in the case of true code failure).
 */
class Shutdown
{
public:
    /** Reserved exit codes for the KIT Library */
    enum : int
    {
        eSUCCESS = OPTION_KIT_SYSTEM_SHUTDOWN_BEGIN_EXIT_CODES,  //!< Application terminated successfully
        eFAILURE,                                                //!< Application terminated due to a failure.  This is a generic failure
        eFATAL_ERROR,                                            //!< Application detected a fatal error and self terminated.  This is a generic failure
        eASSERT,                                                 //!< Application terminated due to an assertion failure
        eOSAL,                                                   //!< Application terminated due to an OSAL failure (e.g. error detected by a module in the Kit::System namespace)
        eDATA_MODEL,                                             //!< Application terminated due to a Data Model failure
        eCONTAINER,                                              //!< Application terminated due to a Container failure (e.g. crossed linked intrusive containers)
        eSTREAMIO,                                               //!< Application terminated due to a stream I/O failure.  This includes errors in File IO, Sockets, Serial ports, etc.
        eMEMORY,                                                 //!< Application terminated due to a memory failure (e.g. failed memory allocation)
        eDRIVER,                                                 //!< Application terminated due to a driver failure.  This is a generic failure
        eITC,                                                    //!< Application terminated due to an inter-thread communication failure.  This is a generic failure
        eFSM_EVENT_OVERFLOW,                                     //!< Application terminated due to a finite state machine event overflow condition (i.e. a FSM event was-dropped/not-processed)
        eWATCHDOG                                                //!< Application terminated due to a watchdog timeout (e.g. watchdog configuration errors, intentional WDOG trips)
    };

public:
    /** This call defines the callback interface that is used when the
        application is shutdown
     */
    class IHandler : public Kit::Container::ListItem
    {
    public:
        /** This method is called when the application is shutting down. The
            method is passed 'exit_code' which is the exit code provided by the
            application when called the Shutdown interface.  The return value
            from the handler will be used as the application shutdown exit code.
            To leave the exit code unaltered - return the passed in 'exit_code'.

            Notes:
                o The final exit code is the serialized 'sum' of the all of the
                  shutdown handlers.
                o The notify() method is called in the thread context that
                  initiated the shutdown.
         */
        virtual int notify( int exitCode ) noexcept = 0;

    public:
        /// Ensure the destructor is virtual
        virtual ~IHandler() {}
    };


public:
    /** This function will force a shutdown of the application with a
        'success' exit code.  What 'forced' means is dependent on the
        actual platform.  All registered callback methods will be called
        before exiting the application.
     */
    static int success() noexcept;

    /** This function will force a shutdown of the application with a 'failure'
        exit code.  The caller can optional specify an exit code. What 'forced'
        means is dependent on the actual platform.  All registered callback
        methods will be called before exiting the application. The returned
        value - assuming the method actually returns - will be the exit code
        returned by the last shutdown handler.

        Note: The recommended approach for exiting the application due to an
              error is to use the Cpl::System::FatalError interface so that
              the 'why' of the failure has the potential for being captured.
     */
    static int failure( int exitCode = eFAILURE ) noexcept;


public:
    /** This method is used to register a callback method that will be called
        when success() or failure() is called and before the application exits.

        A LIFO is used when registering handlers.  This means that the first
        handler registered, is the last handler run.

        During shutdown, the 'notify()' method of 'instanceToRegister'
        is called.
     */
    static void registerHandler( Shutdown::IHandler& instanceToRegister ) noexcept;


public:
    /** This COMPONENT Scoped method.  The application SHOULD not directly
        access this method (unless you are REALLY REALLY REALLY sure you
        know what you are doing).

        Helper method used to notify registered shutdown handlers
     */
    static int notifyShutdownHandlers( int exitCode ) noexcept;
};


};  // end namespaces
};
#endif  // end header latch
