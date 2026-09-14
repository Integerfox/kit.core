#ifndef KIT_JOB_IMANAGER_H_
#define KIT_JOB_IMANAGER_H_
/*-----------------------------------------------------------------------------
* COPYRIGHT_HEADER_TO_BE_FILLED_LATER
*----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Job/IJob.h"

///
namespace Kit {
///
namespace Job {


/** This abstract class defines the "manager" interface to for starting,
    stopping, etc. individual IJob instances.

    NOTE: ALL of the following method can ONLY be called from a different thread
          than where the Job instances execute!  This is because synchronous
          ITC is (assumed to be) used for the implementation.
 */
class IManager
{
public:
    /** This method is used to the specified IJob.  If the Job is currently executing,
        that Job will be stopped, then restarted. If the specified Job name is
        not found false is returned; else true is returned. 

        NOTE: If the client has no 'optionalArgs' to pass to the Job instance, 
              it still must be provide an empty/blank null terminated string, 
              i.e can NOT pass a nullptr. IN ADDITION, The 'optionalArgs' parameter
              must be a valid MODIFIABLE string.  This allows the Job to do
              destructive parse of the argument string.
     */
    virtual bool startJob( const char* jobName, char* optionalArgs ) noexcept = 0;

    /** This method is used to stop the specified running Job.  If the Job
        is currently not in the started state, then the method does nothing
        and false is returned; else true is returned.
     */
    virtual bool stopJob( const char* jobName ) noexcept = 0;

    /// This method is used to stop ALL currently started Job instances.
    virtual void stopAllJobs() noexcept = 0;

public:
    /** This method returns true if the specified Job is currently running; 
        else false is returned.
     */
    virtual bool isJobRunning( const char* jobName ) noexcept = 0;
    
    /** This method returns a list of all Registered Job instances.  If the number
        of registered Job instances exceeded the provided buffer space then false
        is returned; else true is returned.  The number of instances found is
        returned via 'numElemsFound'.

        NOTE: There is no order to the returned instances.
     */
    virtual bool getAvailableJobs( Kit::Job::IJob* dstList[], unsigned dstMaxElements, unsigned& numElemsFound ) noexcept = 0;

   /** This method returns a list of actively running Job instances.  If the number
       of actively running Job instances exceeded the provided buffer space false
       is returned; else true is returned.  The number of instances found is
       returned via 'numElemsFound'.

       NOTE: There is no order to the returned instances.
     */
    virtual bool getRunningJobs( Kit::Job::IJob* dstList[], unsigned dstMaxElements, unsigned& numElemsFound ) noexcept = 0;

    /** This method returns a pointer to the named Job instance.  If no such
        Job exists, a null pointer is returned;
     */
    virtual Kit::Job::IJob* lookUpJob( const char* jobName ) noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IManager() = default;
};

}       // end namespaces
}
#endif  // end header latch