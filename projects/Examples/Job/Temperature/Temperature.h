#ifndef EXAMPLES_JOB_TEMPERATURE_H_
#define EXAMPLES_JOB_TEMPERATURE_H_
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
#include "Kit/Job/JobBase.h"
#include "Kit/Container/OrderedList.h"
#include "Kit/System/Trace.h"
#include "Kit/System/Timer.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/Dm/Mp/Float.h"
#include "Kit/Text/StringTo.h"
#include "Kit/Text/Tokenizer/TextBlock.h"
#include "Kit/Bsp/Api.h"
#include <inttypes.h>

/// Default interval time (in milliseconds) for sampling temperature
#ifndef OPTION_EXAMPLE_JOB_TEMPERATURE_SAMPLE_INTERVAL_MS
#define OPTION_EXAMPLE_JOB_TEMPERATURE_SAMPLE_INTERVAL_MS 100
#endif

/// Default interval time (in milliseconds) for displaying temperature
#ifndef OPTION_EXAMPLE_JOB_TEMPERATURE_DISPLAY_INTERVAL_MS
#define OPTION_EXAMPLE_JOB_TEMPERATURE_DISPLAY_INTERVAL_MS ( 5 * 1000 )
#endif

/// Default temperature Units (false:=Celsius, true:=Fahrenheit)
#ifndef OPTION_EXAMPLE_JOB_TEMPERATURE_FAHRENHEIT
#define OPTION_EXAMPLE_JOB_TEMPERATURE_FAHRENHEIT true
#endif

///
namespace Examples {
///
namespace Job {
///
namespace Temperature {

/** This concrete class implements IJob interface that polls a model point for a
    temperature value and periodically writes the value to the trace output.
    The IJob also collects some metrics and has the option to display temperature
    in degrees Fahrenheit or Celsius.

    NOTE: Multiple of instance of the class can be created - as long as each
          instance has a unique 'name'.

    The class is intended to be example/template for an IJob - not that it does
    anything particular noteworthy.
 */
class Temperature : public Kit::Job::JobBase, public Kit::System::Timer
{
public:
    /// The default IJob Name
    static constexpr const char* DEFAULT_NAME = "temperature";

    /** Usage string (recommended that lines do not exceed 80 chars)
                  1         2         3         4         5         6         7         8
         12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* USAGE =
        "args: [<samplems> [<displayms> [F|C]]]\n"
        "  <samplems>   milliseconds between samples. Default is  100ms\n"
        "  <displayms>  milliseconds between outputs. Default is 5000ms\n"
        "  F|C          'F' use Fahrenheit, 'C' use Celsius. Default is 'F'";

    /** Description string (recommended that lines do not exceed 80 chars)
                  1         2         3         4         5         6         7         8
         12345678901234567890123456789012345678901234567890123456789012345678901234567890
     */
    static constexpr const char* DESCRIPTION =
        "Periodically Samples temperature and displays sample/metric values.";

public:
    /// Constructor
    Temperature( Kit::Container::OrderedList<Kit::Job::IJob>& jobList,
                 Kit::Dm::Mp::Float&                          srcTemperatureMp,
                 const char*                                  name = DEFAULT_NAME )
        : Kit::Job::JobBase( jobList, name, DESCRIPTION, USAGE )
        , m_mpTemperature( srcTemperatureMp )
    {
    }

protected:
    /// See Kit::Job::IJob
    void initialize_() noexcept override
    {
        // Nothing needed (for the example)
    }

    /// See Kit::Job::IJob
    bool start_( Kit::Job::IContext& context, char* args ) noexcept override
    {
        bool startResult = false;

        // Parse command line arguments
        if ( parse( args ) )
        {
            KIT_SYSTEM_TRACE_MSG( OPTION_KIT_JOB_TRACE_SECTION,
                                  "%s: Configuration: sampleMs=%" PRIu32 " ms, displayMs=%" PRIu32 " ms, units=%s",
                                  m_name,
                                  m_sampleMs,
                                  m_displayMs,
                                  m_fahrenheit ? "'F" : "'C" );

            // Housekeeping
            startResult    = true;
            m_maxTemp      = -FLT_MAX;
            m_minTemp      = FLT_MAX;
            m_sumTemp      = 0.0F;
            m_numSamples   = 0;
            m_invalidData  = false;
            m_timeMarkerMs = Kit::System::ElapsedTime::milliseconds();

            // Set the timing source for the SW timer
            Timer::setTimingSource( context.getEventQueue() );

            // Poll the first sample
            expired();
        }

        return setRunningState( startResult );
    }

    /// See Kit::Job::IJob
    void stop_() noexcept override
    {
        /// Ensure the timer is stopped
        Timer::stop();
        setStoppedState();
    }

    /// See Kit::Job::IJob
    void shutdown_() noexcept override
    {
        /// Ensure everything gets stopped
        stop_();
    }

protected:
    /// Helper method to parse the 'command line' options
    bool parse( char* argString ) noexcept
    {
        Kit::Text::Tokenizer::TextBlock args( argString );

        // Default the arguments
        m_sampleMs   = OPTION_EXAMPLE_JOB_TEMPERATURE_SAMPLE_INTERVAL_MS;
        m_displayMs  = OPTION_EXAMPLE_JOB_TEMPERATURE_DISPLAY_INTERVAL_MS;
        m_fahrenheit = OPTION_EXAMPLE_JOB_TEMPERATURE_FAHRENHEIT;

        // No arguments
        if ( args.numParameters() == 0 )
        {
            return true;
        }

        // Sample time
        if ( !Kit::Text::StringTo::unsignedInt( m_sampleMs, args.getParameter( 0 ) ) )
        {
            return false;
        }

        // ONLY sample time is provided
        if ( args.numParameters() == 1 )
        {
            return true;
        }

        // Display time
        else
        {
            if ( !Kit::Text::StringTo::unsignedInt( m_displayMs, args.getParameter( 1 ) ) )
            {
                return false;
            }
        }

        /// ONLY sample time & display time provided
        if ( args.numParameters() == 2 )
        {
            return true;
        }
        else
        {
            // Fahrenheit/Celsius
            if ( args.numParameters() == 3 )
            {
                const char* arg3 = args.getParameter( 2 );
                if ( *arg3 == 'F' || *arg3 == 'f' )
                {
                    m_fahrenheit = true;
                    return true;
                }
                else if ( *arg3 == 'C' || *arg3 == 'c' )
                {
                    m_fahrenheit = false;
                    return true;
                }
            }
        }

        // If I get here the parsing failed
        return false;
    }

    /// Timer expired callback
    void expired( void ) noexcept override
    {
        // Toggle the LED to provide visual feedback on the HW
        Bsp_toggle_debug1();

        // Sample the current temperature
        float t;
        if ( m_mpTemperature.read( t ) )
        {
            m_invalidData = false;

            // Metrics
            if ( t < m_minTemp )
            {
                m_minTemp = t;
            }
            if ( t > m_maxTemp )
            {
                m_maxTemp = t;
            }

            // Running totals
            m_numSamples++;
            m_sumTemp += t;

            uint32_t now = Kit::System::ElapsedTime::milliseconds();
            if ( Kit::System::ElapsedTime::expiredMilliseconds( m_timeMarkerMs, m_displayMs, now ) || m_numSamples == 1 )
            {
                m_timeMarkerMs = now;

                float avgTemp     = m_sumTemp / m_numSamples;
                float displayTemp = t;
                float maxTemp     = m_maxTemp;
                float minTemp     = m_minTemp;
                char  units       = 'C';
                if ( m_fahrenheit )
                {
                    avgTemp     = ( avgTemp * ( 9.0F / 5.0F ) ) + 32.0F;
                    displayTemp = ( displayTemp * ( 9.0F / 5.0F ) ) + 32.0F;
                    maxTemp     = ( maxTemp * ( 9.0F / 5.0F ) ) + 32.0F;
                    minTemp     = ( minTemp * ( 9.0F / 5.0F ) ) + 32.0F;
                    units       = 'F';
                }

                KIT_SYSTEM_TRACE_MSG( OPTION_KIT_JOB_TRACE_SECTION,
                                      "%s: %g '%c, avg: %g '%c, min: %g '%c, max: %g '%c",
                                      m_name,
                                      displayTemp,
                                      units,
                                      avgTemp,
                                      units,
                                      minTemp,
                                      units,
                                      maxTemp,
                                      units );
            }
        }

        // Only print an error message on the transition
        else if ( !m_invalidData )
        {
            m_invalidData = true;
            KIT_SYSTEM_TRACE_MSG( OPTION_KIT_JOB_TRACE_SECTION, "%s: <data invalid>", m_name );
        }

        // Restart my timer
        Timer::start( m_sampleMs );
    }

protected:
    /// MP for temperature
    Kit::Dm::Mp::Float& m_mpTemperature;

    /// Maximum Temp. sampled
    float m_maxTemp;

    /// Minimum Temp. sampled
    float m_minTemp;

    /// Cumulative sum Temp
    float m_sumTemp;

    /// Number of samples
    uint32_t m_numSamples;

    /// Sample time in msecs
    uint32_t m_sampleMs;

    /// Display time in msecs
    uint32_t m_displayMs;

    /// Time marker of last display action
    uint32_t m_timeMarkerMs;

    /// When true the values are displayed in Fahrenheit
    bool m_fahrenheit;

    /// Flag that tracks when I have invalid data
    bool m_invalidData;
};

}  // end namespaces
}
}
#endif  // end header latch
