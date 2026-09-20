#ifndef KIT_JOB_MANAGER_H_
#define KIT_JOB_MANAGER_H_
/*-----------------------------------------------------------------------------
 * COPYRIGHT_HEADER_TO_BE_FILLED_LATER
 *----------------------------------------------------------------------------*/
/** @file */

#include "kit_config.h"
#include "Kit/Itc/IOpenRequest.h"
#include "Kit/Job/IManager.h"
#include "Kit/Job/IContext.h"
#include "Kit/Job/IManagerRequest.h"
#include "Kit/Itc/OpenCloseSync.h"
#include "Kit/EventQueue/IQueue.h"
#include "Kit/Container/OrderedList.h"
#include "Kit/Text/FString.h"


/** Size, in bytes, of the work buffers that the Manager makes available to the
    Job instances.  NOTE: The size of these buffers do NOT include the null
    terminator for the IString buffers/instances.
 */
#ifndef OPTION_KIT_JOB_MANAGER_WORK_BUFFER_SIZE
#define OPTION_KIT_JOB_MANAGER_WORK_BUFFER_SIZE 256
#endif

///
namespace Kit {
///
namespace Job {


/** This concrete class implements the IManager and IContext interfaces.  The
    'public' facing interfaces are all thread safe.  HOWEVER, all clients of
    the IManager MUST execute in a different thread than the IManager itself.

    TL;DR: The above threading restriction is because the IManager interface
           is implemented using synchronous inter-thread communication (ITC).
           If there need for client to co-exist in the same thread as the IManager,
           it must use asynchronous ITC.
 */
class Manager : public IManager, public IContext, public IManagerRequest, public Kit::Itc::OpenCloseSync
{
public:
    /// Constructor
    Manager( Kit::EventQueue::IQueue&           myEventQueue,
             Kit::Container::OrderedList<IJob>& listOfJobs )
        : Kit::Itc::OpenCloseSync( myEventQueue )
        , m_jobs( listOfJobs )
        , m_opened( false )
    {
    }

public:
    /// Starts/initializes the Manager
    void request( Kit::Itc::IOpenRequest::OpenMsg& msg ) noexcept override;

    /// Stops/shutdowns the Manager
    void request( Kit::Itc::ICloseRequest::CloseMsg& msg ) noexcept override;

public:
    /// See Kit::Job::IManager
    bool startJob( const char* jobName, char* optionalArgs ) noexcept override;

    /// See Kit::Job::IManager
    bool stopJob( const char* jobName ) noexcept override;

    /// See Kit::Job::IManager
    bool stopAllJobs() noexcept override;

    /// See Kit::Job::IManager
    bool isJobRunning( const char* jobName ) noexcept override;

    /// See Kit::Job::IManager
    bool getAvailableJobs( Kit::Job::IJob* dstList[], unsigned dstMaxElements, unsigned& numElemsFound ) noexcept override;

    /// See Kit::Job::IManager
    bool getRunningJobs( Kit::Job::IJob* dstList[], unsigned dstMaxElements, unsigned& numElemsFound ) noexcept override;

    /// See Kit::Job::IManager
    Kit::Job::IJob* lookUpJob( const char* jobName ) noexcept override;

public:
    /// See Kit::Job::IContext
    Kit::EventQueue::IQueue& getEventQueue() noexcept override;

    /// See Kit::Job::IContext
    Kit::Text::IString& getWorkBuffer0() noexcept override;

    /// See Kit::Job::IContext
    Kit::Text::IString& getWorkBuffer1() noexcept override;

public:
    /// See Kit::Job::IManagerRequest
    void request( IManagerRequest::StartJobMsg& msg ) noexcept override;

    /// See Kit::Job::IManagerRequest
    void request( IManagerRequest::StopJobMsg& msg ) noexcept override;

    /// See Kit::Job::IManagerRequest
    void request( IManagerRequest::StopAllJobsMsg& msg ) noexcept override;

    /// See Kit::Job::IManagerRequest
    void request( IManagerRequest::JobRunningMsg& msg ) noexcept override;
    /// See Kit::Job::IManagerRequest
    void request( IManagerRequest::GetAvailableJobsMsg& msg ) noexcept override;

    /// See Kit::Job::IManagerRequest
    void request( IManagerRequest::GetRunningJobsMsg& msg ) noexcept override;

    /// See Kit::Job::IManagerRequest
    void request( IManagerRequest::LookupJobMsg& msg ) noexcept override;

protected:
    /// List of IJobs
    Kit::Container::OrderedList<IJob>& m_jobs;

    /// Work buffer 0
    Kit::Text::FString<OPTION_KIT_JOB_MANAGER_WORK_BUFFER_SIZE> m_workBuffer0;

    /// Work buffer 1
    Kit::Text::FString<OPTION_KIT_JOB_MANAGER_WORK_BUFFER_SIZE> m_workBuffer1;

    /// My open/close state
    bool m_opened;
};

}  // end namespaces
}
#endif  // end header latch