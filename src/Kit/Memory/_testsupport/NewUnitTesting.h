#ifndef KIT_MEMORY_TEST_SUPPORT_NEW_UNIT_TESTING_H_
#define KIT_MEMORY_TEST_SUPPORT_NEW_UNIT_TESTING_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include <stddef.h>


///
namespace Kit {
///
namespace Memory {


/** This class provides methods to access the unit testing support for overriding
    the global new/delete operators.  The main purpose is to be able to trigger
    memory allocation failures.  It also provides (very) limited memory leak
    detection.

    Note: About memory leak detection - it DOESN'T really work with the
          Catch2 framework.  This is because Catch2 does a lot of dynamic
          memory allocation while the test is running - which means that
          Catch2's new/delete are all mixed in with UUT's new/deletes :(.

    NOTE: THIS INTERFACE IS **NOT** THREAD SAFE!!

 */
class NewUnitTesting
{
public:
    /** This method will cause the next call(s) to 'new' to return a
        null pointer.
     */
    static void disable() noexcept;

    /** This method 'undos' the disable() call, i.e. the next call(s) to
        new will succeed (assuming there is actual heap memory available).
        This is the default/startup state of the Test heap.
     */
    static void enable() noexcept;

    /** This method is used to set the 'expected' delta between the number
        of calls to new and delete.  The default is a delta of zero, i.e.
        a delete call for every new call.
     */
    static void setNewDeleteDelta( long delta, bool ignoreDelta = false ) noexcept;


public:
    /// New/Delete metrics
    struct Stats
    {
        ///
        unsigned long m_numNewCalls;
        ///
        unsigned long m_numNewCallsFailed;  // Failed do to being 'disabled'
        ///
        unsigned long m_numDeleteCalls;
        ///
        unsigned long m_numDeleteCalls_withNullPtr;
        ///
        size_t m_bytesAllocated;
    };


    /** This method returns and optionally clears the metrics collected
        with respect to Test heap. The caller is responsible for providing
        the memory for the returned metrics.
     */
    static void getStats( Stats& stats, bool resetStats = true ) noexcept;

    /** This method unconditionally clears the stats
     */
    static void clearStats() noexcept;
};


}  // end namespaces
}
#endif  // end header latch
