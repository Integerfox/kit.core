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
#include "Kit/Container/SList.h"

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
    JobBase( Kit::Container::SList<IJob>& jobList,
             const char*                  name,
             const char*                  description = "",
             const char*                  usage       = "" ) noexcept
        : IJob( name )
        , m_name( name )
        , m_description( description )
        , m_usage( usage )
        , m_started( false )
    {
        jobList.put( *this );  // Self register with the Job Manager
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
    /// Job name
    const char* m_name;

    /// Job description
    const char* m_description;

    /// Job usage information
    const char* m_usage;

    /// Used to track the job's started/stopped state
    bool m_started;
};


}  // end namespaces
}
#endif  // end header latch
