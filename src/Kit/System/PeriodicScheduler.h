#ifndef KIT_SYSTEM_PERIODIC_SCHEDULER_H_
#define KIT_SYSTEM_PERIODIC_SCHEDULER_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/System/ElapsedTime.h"

/** Helper macro that is used to mark the end of an 'Interval Array'
 */
#define KIT_SYSTEM_PERIODIC_SCHEDULER_END_INTERVALS \
    {                                               \
        nullptr, 0, nullptr                         \
    }


namespace Kit {
///
namespace System {

/** This concrete class is a 'policy' object that is used to add polled based,
    cooperative monotonic scheduling to a Runnable object.  Monotonic in this
    content means that if a 'interval' method is scheduled to execute at a
    periodic rate of 200Hz, then the method will be called on every 5ms boundary
    of the system time.

    The scheduler makes its best attempt at being monotonic and deterministic,
    BUT because the scheduling is polled and cooperative (i.e. its is the
    application's responsibility to not-overrun/over-allocate the processing
    done during each interval) the actual the timing cannot be guaranteed.
    That said, the scheduler will detect and report when the interval timing
    slips.

    The usage of this class is to implement (or extended) a Cpl::System::Runnable
    object, i.e. invoked inside the 'forever' loop of the Runnable object's appRun()
    method.  The scheduler can be use within a 'bare' forever loop and it can be
    used with existing event based Runnable objects to add periodic scheduling.
    See Cpl::Dm::PeriodicScheduler for example extending an event based Runnable
    object to include periodic scheduling.

    NOTE: A uint64_t is used for the millisecond time keeping because it
          provides a larger range and can accommodate long-running applications
          (i.e. a uint32_t rolls over every 49.7 days; a uint64_t rolls over
          every 584 million years).

    NOTE: A "traditional C function pointer" paradigm is used to provide
          application specialization.  This is by design on the premise that
          it simplifies the Application "main" in that 'start-up child classes'
          are not required to be created for each interval and/or for the
          scheduler hooks.
 */
class PeriodicScheduler
{
public:
    /** Definition for an interval method.  This method is called when the
        period time has expired for the interval.

        Where:
            currentTick     - is current system when the interval method is called
            currentInterval - is the deterministic interval boundary that is being
                              logically executed.
            context         - Application defined argument/value passed to the
                              interval method.

        Example:
            Given a interval method is scheduled to execute at 10Hz and the
            current system time in the Runnable object's forever loop is 10.212
            seconds when the interval method is called THEN:

                currentTick:=      10212 ms
                currentInterval:=  10200 ms
     */
    typedef void ( *IntervalCallbackFunc_T )( uint64_t currentTick,
                                              uint64_t currentInterval,
                                              void*    context );

    /** Defines an interval.  The application should treat this data struct as
        an opaque structure.
     */
    struct Interval_T
    {
        IntervalCallbackFunc_T callbackFunc;  //!< Callback function pointer
        void*                  context;       //!< Optional Context for the callback.  The callback function is required to 'understand' the actual type of the context pointer being passed to it.
        uint64_t               duration;      //!< The amount time in the Interval's period.
        uint64_t               timeMarker;    //!< Internal Use ONLY: Marks the last time the interval executed

        /// Constructor
        Interval_T( IntervalCallbackFunc_T callbackFunc,
                    uint64_t               periodTime,
                    void*                  context = nullptr )
            : callbackFunc( callbackFunc )
            , context( context )
            , duration( periodTime )
            , timeMarker( 0 )
        {
        }
    };


    /** Defines the method that is used to report to the Application when an
        Interval does not execute 'on time'

        Where:
            intervalThatSlipped  - Reference to the interval that did not execute on time
            currentTick          - The system time when the scheduler executed the interval
            missedInterval       - The interval boundary that was missed
            intervalContext      - Application defined argument/value that was
                                   passed to the interval method.

     */
    typedef void ( *ReportSlippageFunc_T )( Interval_T& intervalThatSlipped,
                                            uint64_t    currentTick,
                                            uint64_t    missedInterval );

    /** Defines the function that returns current system.  This method has two
        purposes:
            1) It simplifies unit testing because it breaks the dependency
               on 'real time
            2) Makes unit testing easier
      */
    typedef uint64_t ( *NowFunc_T )();

    /** Defines the optional functions that are used to provide hooks during
        startup/shutdown of the thread/loop[ to perform application specific
        processing.
      */
    typedef void ( *Hook_T )( uint64_t currentTick );

    /** Defines an optional method that is called every time the Runnable's
        object executes its event/scheduling loop.  NOTE: The method is not
        used by the PeriodicScheduler object directly - if used - it is the
        responsibility of the Runnable object (that is calling the scheduler) to
        ensure it is called at the appropriate times.
     */
    typedef void ( *IdleFunc_T )( uint64_t currentTick, bool atLeastOneIntervalExecuted );


public:
    /** Constructor. The application provides a variable length array of interval
        definitions that will be scheduled.  The last entry in the
        array must contain a 'null' Interval_T definition (i.e. all fields
        set to zero).  The Scheduler assumes that each Interval_T definition
        has a unique period time.

        The individual 'intervals' MUST be initialized (either statically or
        by calling initializeInterval()) before creating the scheduler.

        Notes:
            - When extending a event based (i.e. inherits from EventLoop) runnable
              object with scheduling, the Application should define all of the
              scheduled Intervals to have period times that are multiples of
              the EventLoop's 'timeOutPeriodInMsec' constructor value.
     */
    PeriodicScheduler( Interval_T           intervals[],
                       Hook_T               beginThreadProcessing = nullptr,
                       Hook_T               endThreadProcessing   = nullptr,
                       ReportSlippageFunc_T slippageFunc          = nullptr,
                       NowFunc_T            nowFunc               = ElapsedTime::millisecondsEx ) noexcept;

    /// Virtual destructor
    virtual ~PeriodicScheduler() = default;


public:
    /** This method is used to invoke the scheduler.  When called - zero or more
        Interval definitions will be executed.  The method returns true if at
        least one Interval was executed.

        If a scheduled Interval does not execute 'on time', then the reportSlippage()
        method will called.  It is the Application's to decide (what if anything)
        is done when there is slippage in the scheduling. The slippage is reported
        AFTER the Interval's IntervalCallbackFunc_T is called.
     */
    virtual bool executeScheduler() noexcept;

    /** This method is expected to be called ONCE when the 'thread' is started and
        prior to the thread entering its 'forever' loop
     */
    virtual void beginLoop() noexcept;

    /** This method is expected to be called ONCE when the 'thread' has exited
        its 'forever' loop (but before the thread has actually terminated)
     */
    virtual void endLoop() noexcept;

protected:
    /** Helper method to Round DOWN to the nearest 'interval' boundary.
        A side effect the rounding-down is the FIRST execution of an interval
        will NOT be accurate (i.e. will be something less than 'periodMs').
     */
    void setTimeMarker( Interval_T& interval, uint64_t currentTick ) noexcept;

protected:
    /// List of Intervals Pointers
    Interval_T* m_intervals;

    /// Report slippage method
    ReportSlippageFunc_T m_reportSlippage;

    /// Current system callback
    NowFunc_T m_nowFunc;

    /// Application hook during thread start-up
    Hook_T m_beginThreadFunc;

    /// Application hook during thread shutdown
    Hook_T m_endThreadFunc;

    /// Flag to managing the 'first' execution
    bool m_firstExecution;
};

}  // end namespaces
}
#endif  // end header latch
