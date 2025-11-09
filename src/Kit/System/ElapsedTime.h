#ifndef KIT_SYSTEM_ELAPSED_TIME_H_
#define KIT_SYSTEM_ELAPSED_TIME_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include <stdint.h>


///
namespace Kit {
///
namespace System {

/** This class defines the interface for accessing the elapsed time since power
    up and/or reset of the platform.  Elapsed time has millisecond precision.
    Elapsed time is kept in a free running counter - which will roll
    over after its data type has been exceeded 

    There are two set of methods for accessing elapsed time:
    1. The default 'API' that uses uint32_t to represent elapsed time.  The
       32bit counter rolls over after 49.7 days.
    2. Extended 'API' that uses a 64bit data type to represent elapsed time.
       The 64bit counter rolls over after 584 million years.

    NOTE: The two APIs share the same underlying timing source, i.e. the returned
          values are 'in-sync'
 */
class ElapsedTime
{
public:
    /** This method returns the elapsed time, in milliseconds, since the system
        was powered on and/or reset.  The maximum value is 49.7 days before
        the internal counter rolls over.
     */
    static uint32_t milliseconds() noexcept;

    /** This method returns the delta time, in milliseconds, between the
        specified 'startTime' and 'endTime'.  'endTime' is defaulted to NOW
        (i.e. a call to milliseconds(). The calculation properly handles the
        scenario of when the has been 'roll over' between the two times.
     */
    static uint32_t deltaMilliseconds( uint32_t startTime, uint32_t endTime = milliseconds() ) noexcept
    {
        return endTime - startTime;
    }

    /** This method returns true if the specified amount of time has elapsed
        since the 'timeMarker'.  The calculation properly handles the scenario
        of when the has been 'roll over' between the 'timeMarker' and NOW.
     */
    static bool expiredMilliseconds( uint32_t timeMarker,
                                            uint32_t duration,
                                            uint32_t currentTime = milliseconds() ) noexcept
    {
        return deltaMilliseconds( timeMarker, currentTime ) >= duration;
    }

public:
    /** This method returns the elapsed time, in seconds, since the system
        was powered on and/or reset.  The maximum value is 49.7 days before
        the internal counter rolls over.
     */
    static uint32_t seconds() noexcept
    {
        return milliseconds() / 1000L;
    }

    /** This method returns the delta time, in seconds, between the
        specified 'startTime' and 'endTime'.  'endTime' is defaulted to NOW (i.e.
        a call to seconds(). The calculation properly handles the scenario of
        when the has been 'roll over' between the two times.
     */
    static uint32_t deltaSeconds( uint32_t startTime, uint32_t endTime = seconds() ) noexcept
    {
        return endTime - startTime;
    }

    /** This method returns true if the specified amount of time has elapsed
        since the 'timeMarker'.  The calculation properly handles the scenario
        of when the has been 'roll over' between the 'timeMarker' and NOW.
     */
     static bool expiredSeconds( uint32_t timeMarker,
                                       uint32_t duration,
                                       uint32_t currentTime = seconds() ) noexcept
    {
        return deltaSeconds( timeMarker, currentTime ) >= duration;
    }


public:
    /** This method is the same as milliseconds(), EXCEPT that is ALWAYS
        guaranteed to return elapsed time in 'real time'.  See the
        Cpl::System::SimTick for more details about real time vs. simulated
        time.  It is recommended the application NOT CALL this method because
        then that code can NOT be simulated using the SimTick interface.
     */
    static uint32_t millisecondsInRealTime() noexcept;

    /** This method is the same as seconds(), EXCEPT that is ALWAYS guaranteed
        to return elapsed time in 'real time'.  See the Kit::System::SimTick for
        more details about real time vs. simulated time.  It is recommended
        the application NOT CALL this method because then that code can NOT
        be simulated using the SimTick interface.
     */
    static uint32_t secondsInRealTime() noexcept
    {
        return millisecondsInRealTime() / 1000L;
    }


public:
    /** This method returns the elapsed time, in milliseconds, since the system
        was powered on and/or reset.   The maximum value is 585 million years
        before the internal counter rolls over.
     */
    static uint64_t millisecondsEx() noexcept;

    /** This method returns the delta time, in milliseconds, between the
        specified 'startTime' and 'endTime'.  'endTime' is defaulted to NOW
        (i.e. a call to milliseconds(). The calculation properly handles the
        scenario of when the has been 'roll over' between the two times.
     */
     static uint64_t deltaMillisecondsEx( uint64_t startTime, uint64_t endTime = millisecondsEx() ) noexcept
    {
        return endTime - startTime;
    }

    /** This method returns true if the specified amount of time has elapsed
        since the 'timeMarker'.  The calculation properly handles the scenario
        of when the has been 'roll over' between the 'timeMarker' and NOW.
     */
     static bool expiredMillisecondsEx( uint64_t timeMarker,
                                              uint64_t duration,
                                              uint64_t currentTime = millisecondsEx() ) noexcept
    {
        return deltaMillisecondsEx( timeMarker, currentTime ) >= duration;
    }

public:
    /** This method returns the elapsed time, in seconds, since the system
        was powered on and/or reset.  The maximum value is 585 million years
        before the internal counter rolls over.
     */
    static uint64_t secondsEx() noexcept
    {
        return millisecondsEx() / 1000L;
    }

    /** This method returns the delta time, in seconds, between the
        specified 'startTime' and 'endTime'.  'endTime' is defaulted to NOW (i.e.
        a call to seconds(). The calculation properly handles the scenario of
        when the has been 'roll over' between the two times.
     */
     static uint64_t deltaSecondsEx( uint64_t startTime, uint64_t endTime = secondsEx() ) noexcept
    {
        return endTime - startTime;
    }

    /** This method returns true if the specified amount of time has elapsed
        since the 'timeMarker'.  The calculation properly handles the scenario
        of when the has been 'roll over' between the 'timeMarker' and NOW.
     */
     static bool expiredSecondsEx( uint64_t timeMarker,
                                         uint64_t duration,
                                         uint64_t currentTime = secondsEx() ) noexcept
    {
        return deltaSecondsEx( timeMarker, currentTime ) >= duration;
    }


public:
    /** This method is the same as milliseconds(), EXCEPT that is ALWAYS
        guaranteed to return elapsed time in 'real time'.  See the
        Kit::System::SimTick for more details about real time vs. simulated
        time.  It is recommended the application NOT CALL this method because
        then that code can NOT be simulated using the SimTick interface.
     */
    static uint64_t millisecondsInRealTimeEx() noexcept;

    /** This method is the same as seconds(), EXCEPT that is ALWAYS guaranteed
        to return elapsed time in 'real time'.  See the Kit::System::SimTick for
        more details about real time vs. simulated time.  It is recommended the
        application NOT CALL this method because then that code can NOT be
        simulated using the SimTick interface.
     */
    static uint64_t secondsInRealTimeEx() noexcept
    {
        return millisecondsInRealTimeEx() / 1000L;
    }

};


}       // end namespaces
}
#endif  // end header latch
