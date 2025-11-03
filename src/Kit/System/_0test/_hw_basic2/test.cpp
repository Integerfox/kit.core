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
#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Mutex.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/Tls.h"
#include "Kit/System/Trace.h"
#include <cstdint>

#ifndef OPTION_0TEST_HW_BASIC2_STACK_SIZE
#define OPTION_0TEST_HW_BASIC2_STACK_SIZE ( 512 * 3 )
#endif

#define SECT_ "_0test"

///
using namespace Kit::System;

Mutex myLock_;
float testGlobal_;

static bool modifyGlobal_( float op1, float op2, float expected )
{
    Mutex::ScopeLock guard( myLock_ );

    bool result = true;
    testGlobal_ = ( op1 * 2.0 ) / ( op2 + op1 );
    if ( testGlobal_ < ( expected - 0.0001 ) || testGlobal_ > ( expected + 0.0001 ) )
    {
        result = false;
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////
namespace {


class MyRunnable : public IRunnable
{
public:
    uint8_t m_ledNum;
    Tls&    m_tlsKey;
    size_t  m_tlsCounter;

public:
    ///
    MyRunnable( Tls& tlsKey, uint8_t ledNum )
        : m_ledNum( ledNum ), m_tlsKey( tlsKey ), m_tlsCounter( ledNum )
    {
    }

public:
    ///
    void entry() noexcept override
    {
        if ( m_tlsKey.get() != nullptr )
        {
            FatalError::logf( Shutdown::eFAILURE, "(%s) Bad initial TLS value (%p)", Thread::myName(), m_tlsKey.get() );
        }

        for ( ;; )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "Top of MyRunnable::entry()" );
            m_tlsKey.set( (void*)m_tlsCounter );
            toggleLED();
            Thread::wait();
            toggleLED();
            Thread::wait();
            if ( m_tlsKey.get() != (void*)m_tlsCounter )
            {
                FatalError::logf( Shutdown::eFAILURE, "(%s) Bad TLS value (%p) - should be (%p)", Thread::myName(), m_tlsKey.get(), (void*)m_tlsCounter );
            }
            m_tlsCounter++;
        }
    }

    void toggleLED()
    {
        if ( m_ledNum == 1 )
        {
            Bsp_toggle_debug1();
        }
        else
        {
            Bsp_toggle_debug2();
        }
    }
};

class MyRunnable2 : public IRunnable
{
public:
    Thread&  m_ledThread;
    uint16_t m_onTime_ms;
    uint16_t m_offTime_ms;
    uint64_t m_time64;
    uint32_t m_time32;

public:
    MyRunnable2( Thread& ledThread, uint16_t onTime_ms, uint16_t offTime_ms )
        : m_ledThread( ledThread ), m_onTime_ms( onTime_ms ), m_offTime_ms( offTime_ms )
    {
    }

public:
    void entry() noexcept override
    {
        uint32_t sleepTime = m_onTime_ms + m_offTime_ms;
        m_time64           = ElapsedTime::millisecondsEx();
        m_time32           = ElapsedTime::milliseconds();

        for ( ;; )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "Top of MyRunnable2::entry()" );

            sleep( m_onTime_ms );
            m_ledThread.signal();
            sleep( m_offTime_ms );
            m_ledThread.signal();


            uint64_t time64  = ElapsedTime::millisecondsEx();
            uint32_t time32  = ElapsedTime::milliseconds();
            uint32_t delta32 = ElapsedTime::deltaMilliseconds( m_time32, time32 );
            uint64_t delta64 = ElapsedTime::deltaMillisecondsEx( m_time64, time64 );

            if ( delta64 < sleepTime )
            {
                FatalError::logf( Shutdown::eFAILURE, "Elapsed 64bit delta is wrong" );
            }

            if ( delta32 < sleepTime )
            {
                FatalError::logf( Shutdown::eFAILURE, "Elapsed 32bit delta is wrong" );
            }

            if ( delta64 < delta32 - 1 || delta64 > delta32 + 1 )
            {
                FatalError::logf( Shutdown::eFAILURE, "64bit time is not insync with 32bit time" );
            }

            m_time64 = time64;
            m_time32 = time32;
        }
    }
};


class MyRunnable3 : public IRunnable
{
public:
    float m_op1;
    float m_op2;
    float m_expected;

public:
    ///
    MyRunnable3( float op1, float op2 )
        : m_op1( op1 ), m_op2( op2 ), m_expected( ( op1 * 2.0 ) / ( op2 + op1 ) )
    {
    }


public:
    ///
    void entry() noexcept override
    {
        for ( ;; )
        {
            uint32_t now = ElapsedTime::milliseconds();
            KIT_SYSTEM_TRACE_MSG( SECT_, ( "Top of MyRunnable3::entry()" ) );

            if ( modifyGlobal_( m_op1, m_op2, m_expected ) == false )
            {
                FatalError::logf( Shutdown::eFAILURE, "Mutex failed. Thread=%s", Thread::myName() );
            }

            uint32_t delta = ElapsedTime::deltaMilliseconds( now, ElapsedTime::milliseconds() );
            sleep( 1000 - delta );
        }
    }
};

};  // end namespace


////////////////////////////////////////////////////////////////////////////////

// NOTE: I create the Runnable objects on the Heap - because depending on the
//       platform - FreeRTOS will corrupt the raw 'main stack' when it starts
//       the first thread.

void runtests( void )
{
    // Create my TLS key (can't be done statically)
    Tls* keyPtr = new ( std::nothrow ) Tls();


    // Create some threads....
    MyRunnable*  appleLed     = new ( std::nothrow ) MyRunnable( *keyPtr, 1 );
    Thread*      appledLedPtr = Thread::create( *appleLed, "AppleLED", KIT_SYSTEM_THREAD_PRIORITY_NORMAL, OPTION_0TEST_HW_BASIC2_STACK_SIZE );
    MyRunnable2* appleTimer   = new ( std::nothrow ) MyRunnable2( *appledLedPtr, 1000, 1000 );
    Thread::create( *appleTimer, "AppleTimer" );

    MyRunnable*  orangeLed    = new ( std::nothrow ) MyRunnable( *keyPtr, 2 );
    Thread*      orangeLedPtr = Thread::create( *orangeLed, "OrangeLED", KIT_SYSTEM_THREAD_PRIORITY_NORMAL, OPTION_0TEST_HW_BASIC2_STACK_SIZE );
    MyRunnable2* orangeTimer  = new ( std::nothrow ) MyRunnable2( *orangeLedPtr, 1500, 250 );
    Thread::create( *orangeTimer, "OrangeTimer", KIT_SYSTEM_THREAD_PRIORITY_NORMAL, OPTION_0TEST_HW_BASIC2_STACK_SIZE );

    MyRunnable3* t1 = new ( std::nothrow ) MyRunnable3( 3.14159, 3 );
    Thread::create( *t1, "T1", KIT_SYSTEM_THREAD_PRIORITY_NORMAL, OPTION_0TEST_HW_BASIC2_STACK_SIZE );
    MyRunnable3* t2 = new ( std::nothrow ) MyRunnable3( 2.71828, 7 );
    Thread::create( *t2, "T2", KIT_SYSTEM_THREAD_PRIORITY_NORMAL, OPTION_0TEST_HW_BASIC2_STACK_SIZE );
    MyRunnable3* t3 = new ( std::nothrow ) MyRunnable3( 64.0, 128.0 );
    Thread::create( *t3, "T3", KIT_SYSTEM_THREAD_PRIORITY_NORMAL, OPTION_0TEST_HW_BASIC2_STACK_SIZE );

    // Start the scheduler
    KIT_SYSTEM_TRACE_MSG( SECT_, ( "Starting scheduler..." ) );
    enableScheduling();
}
