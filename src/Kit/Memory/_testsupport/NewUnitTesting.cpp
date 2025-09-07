/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "NewUnitTesting.h"
#include "Kit/System/Shutdown.h"
#include "Kit/System/PrivateStartup.h"
#include <stdlib.h>
#include <stdio.h>
#include <new>

//------------------------------------------------------------------------------
namespace Kit {
namespace Memory {

static NewUnitTesting::Stats metrics_;
static bool                  disabled_;
static long                  newDeleteDelta_;
static bool                  ignoreDelta_;
static void                  clear_metrics( NewUnitTesting::Stats& stats ) noexcept;


//////////////////////////////////////////////////////////////////////////
namespace {  // anonymous namespace

class ExitHandler : public Kit::System::Shutdown::IHandler
{
protected:
    ///
    int notify( int exitCode ) noexcept override
    {
        const char* msg   = "new/delete call delta MATCHES expected value.";
        long        delta = (long)( metrics_.m_numNewCalls - metrics_.m_numDeleteCalls );
        if ( ignoreDelta_ )
        {
            msg = "new/delete call delta being IGNORED.";
        }
        else if ( delta != newDeleteDelta_ )
        {
            exitCode = Kit::System::Shutdown::eMEMORY;
            msg      = "ERROR: new/delete call delta does NOT match expected value.";
        }

        printf( "\n----------------------------------------------------------" );
        printf( "\nKit::Memory::NewUnitTesting Metrics:\n" );
        printf( "\n    Num new()              = %lu", metrics_.m_numNewCalls );
        printf( "\n    Num new() disabled     = %lu", metrics_.m_numNewCallsFailed );
        printf( "\n    Num delete()           = %lu", metrics_.m_numDeleteCalls );
        printf( "\n    Num delete(0)          = %lu", metrics_.m_numDeleteCalls_withNullPtr );
        printf( "\n    Bytes Allocated        = %lu", (unsigned long)metrics_.m_bytesAllocated );
        printf( "\n    Expected new/del delta = %ld", newDeleteDelta_ );
        printf( "\n    Actual new/del delta   = %ld", delta );
        printf( "\n\n%s", msg );
        printf( "\n----------------------------------------------------------\n" );

        return exitCode;
    }
};


class RegisterInitHandler : public Kit::System::IStartupHook
{
public:
    ///
    ExitHandler m_shutdown;


public:
    ///
    RegisterInitHandler() noexcept
        : IStartupHook( TEST_INFRA ) {}


protected:
    ///
    void notify( InitLevel init_level ) noexcept override
    {
        Kit::System::Shutdown::registerHandler( m_shutdown );
    }
};
}  // end anonymous namespace

///
static RegisterInitHandler autoRegister_systemInit_hook;


//////////////////////////////////////////////////////////////////////////
static void* my_new( size_t sz ) noexcept
{
    // Update metrics
    metrics_.m_bytesAllocated += sz;
    if ( disabled_ )
    {
        // Fail allocation if not 'enabled'
        metrics_.m_numNewCallsFailed++;
        return 0;
    }
    else
    {
        metrics_.m_numNewCalls++;
    }

    return malloc( sz );
}


static void my_delete( void* ptr ) noexcept
{
    // Update metrics
    metrics_.m_numDeleteCalls++;
    if ( !ptr )
    {
        metrics_.m_numDeleteCalls_withNullPtr++;
    }

    // C++ standard says it is okay to delete a null pointer (make sure we DON'T free a null pointer)
    if ( ptr )
    {
        free( ptr );
    }
}


////////////////////////////////////////////////////////////////////////////////
void NewUnitTesting::disable( void ) noexcept
{
    disabled_ = true;
}


void NewUnitTesting::enable( void ) noexcept
{
    disabled_ = false;
}


void NewUnitTesting::setNewDeleteDelta( long delta, bool ignoreDelta ) noexcept
{
    newDeleteDelta_ = delta;
    ignoreDelta_    = ignoreDelta;
}


void NewUnitTesting::getStats( Stats& stats, bool resetStats ) noexcept
{
    stats = metrics_;
    if ( resetStats )
    {
        clear_metrics( metrics_ );
    }
}

void NewUnitTesting::clearStats() noexcept
{
    clear_metrics( metrics_ );
}

void clear_metrics( NewUnitTesting::Stats& stats ) noexcept
{
    stats.m_numNewCalls                = 0;
    stats.m_numNewCallsFailed          = 0;
    stats.m_numDeleteCalls             = 0;
    stats.m_numDeleteCalls_withNullPtr = 0;
    stats.m_bytesAllocated             = 0;
}

}  // end namespaces
}

/////////////////////////////////////////////////
void* operator new( size_t sz, const std::nothrow_t& nothrow_value ) noexcept
{
    return Kit::Memory::my_new( sz );
}

void* operator new[]( size_t sz, const std::nothrow_t& nothrow_value ) noexcept
{
    return Kit::Memory::my_new( sz );
}

void operator delete( void* ptr ) noexcept
{
    Kit::Memory::my_delete( ptr );
}

void operator delete[]( void* ptr ) noexcept
{
    Kit::Memory::my_delete( ptr );
}


//------------------------------------------------------------------------------