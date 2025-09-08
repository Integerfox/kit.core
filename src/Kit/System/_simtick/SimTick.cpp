/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/SimTick.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/api.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/Mutex.h"
#include "Kit/System/Tls.h"
#include "Kit/System/Thread.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/Assert.h"
#include "Kit/Container/SList.h"
#include <cstdint>
#include <new>

//------------------------------------------------------------------------------
namespace Kit {
namespace System {

static Tls*                           simTlsPtr_      = 0;
static uint64_t                       milliseconds_   = 0;
static uint64_t                       remainingTicks_ = 0;
static Semaphore                      tickSource_;
static Mutex                          myLock_;
static Kit::Container::SList<SimTick> waiters_;


/////////////////////////////////////////
unsigned SimTick::wakeUpWaiters( void ) noexcept
{
    unsigned waiters    = 0;
    SimTick* simInfoPtr = waiters_.get();
    while ( simInfoPtr )
    {
        waiters++;
        simInfoPtr->m_ackPending = true;
        simInfoPtr->m_waiter.signal();
        simInfoPtr = waiters_.get();
    }
    return waiters;
}

unsigned SimTick::getCurrentWaitersCount( void ) noexcept
{
    unsigned waiters    = 0;
    SimTick* simInfoPtr = waiters_.first();
    while ( simInfoPtr )
    {
        waiters++;
        simInfoPtr = waiters_.next( *simInfoPtr );
    }
    return waiters;
}

bool SimTick::advance( uint64_t numTicks ) noexcept
{
    bool result  = true;
    numTicks    += remainingTicks_;

    while ( numTicks >= OPTION_KIT_SYSTEM_SIM_TICK_MIN_TICKS_FOR_ADVANCE )
    {
        numTicks -= OPTION_KIT_SYSTEM_SIM_TICK_MIN_TICKS_FOR_ADVANCE;

        // Drain any 'left-over/initial' sim-thread-ack-signals
        while ( tickSource_.tryWait() )
        {
        }

        // START critical section
        myLock_.lock();

        // Increment my system time
        milliseconds_ += OPTION_KIT_SYSTEM_SIM_TICK_MIN_TICKS_FOR_ADVANCE;

        // Wake-up all of threads waiting on a simulate tick
        unsigned waiters = wakeUpWaiters();

        // END critical section
        myLock_.unlock();

        // Wait for all the 'sim threads' to complete their tick processing
        while ( waiters-- )
        {
            tickSource_.wait();
        }
    }

    remainingTicks_ = numTicks;
    return result;
}


void SimTick::applicationWait( void ) noexcept
{
    // Get my thread's SimInfo
    auto* simInfoPtr = static_cast<SimTick*>( simTlsPtr_->get() );

    // NEVER block if a non-simulated-tick thread
    if ( !simInfoPtr )
    {
        return;
    }

    // Notify the tick source that I have complete my processing for the system tick (this blocking wait aborts remaining thread iterations)
    if ( simInfoPtr->m_ackPending )
    {
        simInfoPtr->m_ackPending = false;
        tickSource_.signal();
    }
}


void SimTick::topLevelWait( void ) noexcept
{
    // Get my thread's SimInfo
    auto* simInfoPtr = static_cast<SimTick*>( simTlsPtr_->get() );

    // NEVER block if a non-simulated-tick thread
    if ( !simInfoPtr )
    {
        return;
    }

    // Test my thread's tick counter against the System tick count and queue my thread to wait for next tick if tick counts match
    if ( testAndQueue( simInfoPtr ) )
    {
        simInfoPtr->m_ackPending = false;
        tickSource_.signal();
        simInfoPtr->m_waiter.waitInRealTime();
    }
}


bool SimTick::testAndQueue( SimTick* simInfoPtr ) noexcept
{
    // SCOPE Critical section
    Mutex::ScopeLock criticalSection( myLock_ );

    // Wait while my thread's simulate time match the system simulate time
    if ( simInfoPtr->m_curTicks == milliseconds_ )
    {
        // Add this thread to the waiting list
        waiters_.put( *simInfoPtr );

        // Set flag/return code that I need to wait for the next simulated tick
        return true;
    }

    // Update my threads internal tick count
    simInfoPtr->m_curTicks = milliseconds_;
    return false;
}


/////////////////////////////////////////
SimTick::SimTick()
    : m_curTicks( 0 )
    , m_ackPending( false )
    , m_threadId( (KitSystemThreadID_T)0 )
{
}


bool SimTick::isWaitingOnNextTick( KitSystemThreadID_T threadID ) noexcept
{
    bool             waiting = false;
    Mutex::ScopeLock criticalSection( myLock_ );

    SimTick* simInfoPtr = waiters_.head();
    while ( simInfoPtr )
    {
        if ( simInfoPtr->m_threadId == threadID )
        {
            waiting = true;
            break;
        }

        simInfoPtr = waiters_.next( *simInfoPtr );
    }

    return waiting;
}


bool SimTick::usingSimTicks() noexcept
{
    // If I have not yet created the TLS key/index -->then NO threads are using simulated time
    if ( !simTlsPtr_ )
    {
        return false;
    }

    // If my thread's SimInfo is NULL -->then I am NOT using simulated time
    return simTlsPtr_->get() != nullptr;
}


uint64_t SimTick::current( void ) noexcept
{
    Mutex::ScopeLock criticalSection( myLock_ );
    return milliseconds_;
}


void SimTick::threadInit_( bool useSimTicks ) noexcept
{
    // Create the TLS key/index needed to store the per thread info (do a lazy create)
    if ( !simTlsPtr_ )
    {
        simTlsPtr_ = new ( std::nothrow ) Tls();
        KIT_SYSTEM_ASSERT( simTlsPtr_ != nullptr );
    }

    // Set my TLS SimInfo block to NULL since the thread has be requested to NOT USE simulated ticks
    if ( !useSimTicks )
    {
        simTlsPtr_->set( nullptr );
        return;
    }

    // Create an instance of SimTick for my per thread simInfo and store the newly created instance in TLS
    SimTick* simInfoPtr = new ( std::nothrow ) SimTick();
    if ( !simInfoPtr )
    {
        FatalError::logf( Shutdown::eOSAL, "SimTick::threadInit_().  Failed to alloc an instance SimTick for thread (%s)", Thread::myName() );
    }

    simInfoPtr->m_threadId = Thread::myId();
    myLock_.lock();
    simInfoPtr->m_curTicks = milliseconds_;
    myLock_.unlock();
    simTlsPtr_->set( simInfoPtr );
}


void SimTick::onThreadExit_( void ) noexcept
{
    // If I have not yet created the TLS key/index -->then NO threads are using simulated time
    if ( simTlsPtr_ )
    {

        // Get my thread's SimInfo
        auto* simInfoPtr = static_cast<SimTick*>( simTlsPtr_->get() );


        // NOTHING needed if a non-simulated-tick thread
        if ( !simInfoPtr )
        {
            return;
        }


        // Check if I was 'processing' a tick
        if ( simInfoPtr->m_ackPending )
        {
            // Acknowledge that the thread has completed it's processing for the current tick
            simInfoPtr->m_ackPending = false;
            tickSource_.signal();
        }

        // Free the SimTick object
        delete simInfoPtr;
    }
}


/////////////////////////////////////////////
void sleep( uint32_t milliseconds ) noexcept
{
    // If I have not yet created the TLS key/index -->then NO threads are using simulated time
    if ( simTlsPtr_ == nullptr )
    {
        sleepInRealTime( milliseconds );
        return;
    }
    
    // Get my thread's SimInfo
    auto* simInfoPtr = static_cast<SimTick*>( simTlsPtr_->get() );

    // ALWAYS use the 'real' sleep call when I am a non-simulated-tick thread
    if ( !simInfoPtr )
    {
        sleepInRealTime( milliseconds );
    }

    // Thread is using simulated time -->wait for my simulate ticks
    else
    {
        // Loop until enough simulated ticks have elapsed
        uint64_t msecEx = milliseconds;
        while ( msecEx )
        {
            // Get the current time
            myLock_.lock();
            uint64_t start = milliseconds_;
            myLock_.unlock();

            // Wait on the tick
            SimTick::topLevelWait();
            myLock_.lock();
            uint64_t current = milliseconds_;
            myLock_.unlock();

            // Subtract the elapsed time from my count down timer
            uint64_t delta = ElapsedTime::deltaMilliseconds( static_cast<uint32_t>( start ), static_cast<uint32_t>( current ) );
            msecEx         = msecEx > delta ? msecEx - delta : 0;
        }
    }
}


///////////////////////////////////////////////////////////////
uint32_t ElapsedTime::milliseconds( void ) noexcept
{
    return static_cast<uint32_t>( millisecondsEx() );
}

uint64_t ElapsedTime::millisecondsEx( void ) noexcept
{
    // If I have not yet created the TLS key/index -->then NO threads are using simulated time
    if ( simTlsPtr_ == nullptr )
    {
        return ElapsedTime::millisecondsInRealTimeEx();;
    }

    // Get my thread's SimInfo
    auto* simInfoPtr = static_cast<SimTick*>( simTlsPtr_->get() );

    // ALWAYS use the 'real' elapsed time when I am a non-simulated-tick thread
    if ( !simInfoPtr )
    {
        return ElapsedTime::millisecondsInRealTimeEx();
    }


    // Thread is using simulated time -->return the simulate time
    myLock_.lock();
    uint64_t msec = milliseconds_;
    myLock_.unlock();
    return msec;
}


}  // end namespace
}
//------------------------------------------------------------------------------