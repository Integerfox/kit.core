#ifndef KIT_JOB_JOBBASE_H_
#define KIT_JOB_JOBBASE_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Job/IJob.h"
#include "Kit/Container/OrderedList.h"

///
namespace Kit {
///
namespace Job {


/** This partial concrete class provide common infrastructure that is common
    to all IJob instances.
 */
class JobBase : public IJob
{
protected:
    /// Constructor
    JobBase( Kit::Container::OrderedList<IJob>& jobList,
             const char*                        name,
             const char*                        description = "",
             const char*                        usage       = "" ) noexcept
        : IJob( name )
        , m_name( name )
        , m_description( description )
        , m_usage( usage )
        , m_running( false )
    {
        jobList.insert( *this );  // Self register with the Job Manager
    }

public:
    /// See Kit::Container::KeyedItem
    const Kit::Container::Key& getKey() const noexcept override { return *this; }

public:
    /// See Kit::Job::IJob
    const char* getName() const noexcept override { return m_name; }

    /// See Kit::Job::IJob
    const char* getDescription() const noexcept override { return m_description; }

    /// See Kit::Job::IJob
    const char* getUsage() const noexcept override { return m_usage; }

protected:
    /// See Kit::Job::IJob
    bool isRunning_() const noexcept override { return m_running; }

protected:
    /** Helper method.  The concrete child class should call this method ON EXIT
        of it start_(...) method.  It marks the Job as running/idle based on the
        provide 'startResult' parameter to indicate success or failure.
    */
    inline bool setRunningState( bool startResult ) noexcept
    {
        m_running = startResult;
        return m_running;
    }

    /** Helper method.  The concrete child class should call this method ON EXIT
        of it stop_() method.  It marks the Job as stopped/idle.  The 'stopResult'
        parameter indicates success or failure of the 'stopping action', i.e.
        set to 'true' if the stop action was successful, else 'false'.

        NOTE: This method should CAN be called by the Job at any time to
              indicate it has stopped.
    */
    inline void setStoppedState() noexcept
    {
        m_running = false;
    }

protected:
    /// Job name
    const char* m_name;

    /// Job description
    const char* m_description;

    /// Job usage information
    const char* m_usage;

    /// Used to track the job's running/idle state
    bool m_running;
};


}  // end namespaces
}
#endif  // end header latch
