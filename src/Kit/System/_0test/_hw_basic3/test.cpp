/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Bsp/Api.h"
#include "Kit/System/Api.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/Thread.h"
#include "Kit/System/Mutex.h"
#include "Kit/System/FatalError.h"
#include "Kit/System/ElapsedTime.h"
#include "Kit/System/Tls.h"
#include "Kit/System/Trace.h"
#include "Kit/System/GlobalLock.h"
#include <cstdint>


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

static float globalLockValue_;
static bool  testGlobalLock_( float op1, float op2, float expected )
{
    GlobalLock::begin();
    bool result      = true;
    globalLockValue_ = ( op1 * 2.0 ) / ( op2 + op1 );
    if ( globalLockValue_ < ( expected - 0.0001 ) || globalLockValue_ > ( expected + 0.0001 ) )
    {
        result = false;
    }

    GlobalLock::end();
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
    float   m_op1;
    float   m_op2;
    float   m_expected;

public:
    ///
    MyRunnable( Tls& tlsKey, uint8_t ledNum, float op1, float op2 )
        : m_ledNum( ledNum )
        , m_tlsKey( tlsKey )
        , m_tlsCounter( ledNum )
        , m_op1( op1 )
        , m_op2( op2 )
        , m_expected( ( op1 * 2.0 ) / ( op2 + op1 ) )
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
            KIT_SYSTEM_TRACE_MSG( SECT_, "Top of MyRunnable::entry(%zu)", m_tlsCounter );
            m_tlsKey.set( (void*)m_tlsCounter );
            testMutex();
            testGlobalLock();
            toggleLED();
            Thread::wait();
            testMutex();
            testGlobalLock();
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

    void testMutex()
    {
        if ( modifyGlobal_( m_op1, m_op2, m_expected ) == false )
        {
            FatalError::logf( Shutdown::eFAILURE, "Mutex failed. Thread=%s", Thread::myName() );
        }
    }

    void testGlobalLock()
    {
        if ( testGlobalLock_( m_op1, m_op2, m_expected ) == false )
        {
            FatalError::logf( Shutdown::eFAILURE, "Global Lock failed. Thread=%s", Thread::myName() );
        }
    }
};

class MyRunnable2 : public IRunnable
{
public:
    Thread& m_ledThread;
    Tls&    m_tlsKey;
    size_t  m_tlsCounter;

    uint16_t m_onTime_ms;
    uint16_t m_offTime_ms;
    uint64_t m_time64;
    uint32_t m_time32;
    float    m_op1;
    float    m_op2;
    float    m_expected;

public:
    MyRunnable2( Thread& ledThread, Tls& tlsKey, uint16_t onTime_ms, uint16_t offTime_ms, float op1, float op2 )
        : m_ledThread( ledThread )
        , m_tlsKey( tlsKey )
        , m_tlsCounter( 1000 )
        , m_onTime_ms( onTime_ms )
        , m_offTime_ms( offTime_ms )
        , m_op1( op1 )
        , m_op2( op2 )
        , m_expected( ( op1 * 2.0 ) / ( op2 + op1 ) )
    {
    }

public:
    void entry() noexcept override
    {
        if ( m_tlsKey.get() != nullptr )
        {
            FatalError::logf( Shutdown::eFAILURE, "(%s) Bad initial TLS value (%p)", Thread::myName(), m_tlsKey.get() );
        }
        uint32_t sleepTime = m_onTime_ms + m_offTime_ms;
        m_time64           = ElapsedTime::millisecondsEx();
        m_time32           = ElapsedTime::milliseconds();

        for ( ;; )
        {
            KIT_SYSTEM_TRACE_MSG( SECT_, "Top of MyRunnable2::entry(%zu)", m_tlsCounter );
            m_tlsKey.set( (void*)m_tlsCounter );
            
            testMutex();
            testGlobalLock();
            sleep( m_onTime_ms );
            m_ledThread.signal();
            testMutex();
            testGlobalLock();
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

            if ( m_tlsKey.get() != (void*)m_tlsCounter )
            {
                FatalError::logf( Shutdown::eFAILURE, "(%s) Bad TLS value (%p) - should be (%p)", Thread::myName(), m_tlsKey.get(), (void*)m_tlsCounter );
            }
            m_tlsCounter++;
        }
    }

    void testMutex()
    {
        if ( modifyGlobal_( m_op1, m_op2, m_expected ) == false )
        {
            FatalError::logf( Shutdown::eFAILURE, "Mutex failed. Thread=%s", Thread::myName() );
        }
    }

    void testGlobalLock()
    {
        if ( testGlobalLock_( m_op1, m_op2, m_expected ) == false )
        {
            FatalError::logf( Shutdown::eFAILURE, "Global Lock failed. Thread=%s", Thread::myName() );
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
    MyRunnable*  appleLed     = new ( std::nothrow ) MyRunnable( *keyPtr, 1, 3.14159, 3 );
    Thread*      appledLedPtr = Thread::create( *appleLed, "AppleLED" );
    MyRunnable2* appleTimer   = new ( std::nothrow ) MyRunnable2( *appledLedPtr, *keyPtr, 250, 250, 2.71828, 7 );
    Thread::create( *appleTimer, "AppleTimer" );

    // Start the scheduler
    KIT_SYSTEM_TRACE_MSG( SECT_, ( "Starting scheduler..." ) );
    enableScheduling();
}
