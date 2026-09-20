#ifndef KIT_JOB_IJOB_H_
#define KIT_JOB_IJOB_H_
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
#include "Kit/Container/KeyedItem.h"
#include "Kit/Job/IContext.h"

/// KIT Trace Section identifier for a common trace output section
#ifndef OPTION_KIT_JOB_TRACE_SECTION
#define OPTION_KIT_JOB_TRACE_SECTION "Job"
#endif


///
namespace Kit {
///
namespace Job {

/** This abstract class defines the interface for a Job. An Job is essentially a
    'mini-application' that performs a series of steps and typically reports its
    output using the KIT_SYSTEM_TRACE_MSG() method.

    Multiple Job instances can be executing at the same time. Specifically, one
    or more Job instances execute in their IManager's threads.  Job self register
    with their IManager.  Multiple IManager instances can exist.

    The following are the 'thread safety' requirements for a IJob:
        o Data can safely be exchanged via Model Points
        o The application can safely pass initial settings, configuration,
          options when the IJob is started
 */
class IJob : public Kit::Container::KeyedItem, public Kit::Container::KeyLiteralString
{
public:
    /** This method returns the IJob name.  The IJob name must an printable
        ASCII string with NO whitespace.  The name is used to uniquely identify
        (within an IManager instance) the individual IJob
     */
    virtual const char* getName() const noexcept = 0;

    /** This method returns brief description/summary of what the IJob does.
        Typically this information will be displayed on the Debug console to
        aid the user on how to execute the IJob.  The content/verbosity of the
        text is IJob specific.
     */
    virtual const char* getDescription() const noexcept = 0;

    /** This method returns help (optional) for the IJob's arguments.
        Typically this information will be displayed on the Debug console to
        aid the user on how to execute the IJob.  The content/verbosity of the
        text is IJob specific.
     */
    virtual const char* getUsage() const noexcept = 0;


public:
    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Kit::Job::IJob namespace.  The Application should
        NEVER call this method.

        This method is used to perform any needed initialization on start-up of
        Application. This method is called when the IJob's Manager is opened.

        This method MUST be called in the thread that the IJob executes in.
     */
    virtual void initialize_() noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Kit::Job::IJob namespace.  The Application should
        NEVER call this method.

        This method is used to perform any needed shutdown when the application
        is shutdown.  This method is called when the IJob's Manager is closed.

        This method MUST be called in the thread that the IJob executes in.
     */
    virtual void shutdown_() noexcept = 0;


public:
    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Kit::Job::IJob namespace.  The Application should
        NEVER call this method, instead the Application must call the Manager
        to start a IJob.

        If the client has no 'optionalArgs' to pass to the IJob instance - it must
        provide an empty/blank null terminated string, i.e can NOT pass a nullptr.

        This method is used to start a IJob. If the IJob is unable to start then
        false is returned; else true is returned.  NOTE: The IJob is responsible
        for setting its 'running state' to true when the start_() method returns
        true.

        This method MUST be called in the thread that the IJob executes in.

        NOTE: The IJob is allowed to perform destructive parsing on 'optionalArgs',
              however it must honor the string len of 'optionalArgs'
     */
    virtual bool start_( IContext& context, char* optionalArgs ) noexcept = 0;

    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
        by other classes in the Kit::Job::IJob namespace.  The Application should
        NEVER call this method, instead the Application must call the Manager
        to stop a IJob.

        This method is used to stop a IJob. NOTE: The IJob is responsible for
        setting its 'running state' to false when the stop_() method returns.
     */
    virtual void stop_() noexcept = 0;

public:
    /** This method has PACKAGE Scope, i.e. it is intended to be ONLY accessible
         by other classes in the Kit::Job::IJob namespace.  The Application should
         NEVER call this method, instead the Application must call the Manager
         to stop a IJob.

         The method returns true if the IJob is currently running; else false.
     */
    virtual bool isRunning_() const noexcept = 0;

protected:
    /// Protected Constructor (needed because of the inheritance from KeyLiteralString)
    IJob( const char* name ) noexcept
        : Kit::Container::KeyLiteralString( name )
    {
    }

public:
    /// Virtual destructor
    virtual ~IJob() = default;
};


}  // end namespaces
}
#endif  // end header latch
