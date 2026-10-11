/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/EventQueue/Server.h"
#include "Kit/Itc/SelfPostMessage.h"
#include "catch2/catch_test_macros.hpp"

namespace {

class TestEventQueue : public Kit::EventQueue::Server
{
public:
    void dispatchOne() noexcept
    {
        processMessages();
    }
};

class ContinuationProbe
{
public:
    explicit ContinuationProbe( Kit::EventQueue::IQueue& queue ) noexcept
        : m_resume( queue, *this, &ContinuationProbe::onResume )
        , callbacks( 0 )
        , rescheduleOnResume( false )
    {
    }

    void onResume() noexcept
    {
        ++callbacks;
        if ( rescheduleOnResume )
        {
            rescheduleOnResume = false;
            m_resume.schedule();
        }
    }

    Kit::Itc::SelfPostMessage<ContinuationProbe> m_resume;
    unsigned                                   callbacks;
    bool                                       rescheduleOnResume;
};

}  // end anonymous namespace

TEST_CASE( "SelfPostMessage" )
{
    SECTION( "coalesces repeated schedules" )
    {
        TestEventQueue  queue;
        ContinuationProbe probe( queue );

        probe.m_resume.schedule();
        probe.m_resume.schedule();
        REQUIRE( probe.m_resume.isPending() );

        queue.dispatchOne();
        REQUIRE( probe.callbacks == 1 );
        REQUIRE_FALSE( probe.m_resume.isPending() );
    }

    SECTION( "cancels a queued callback" )
    {
        TestEventQueue  queue;
        ContinuationProbe probe( queue );

        probe.m_resume.schedule();
        probe.m_resume.cancel();
        REQUIRE( probe.m_resume.isPending() );

        queue.dispatchOne();
        REQUIRE( probe.callbacks == 0 );
        REQUIRE_FALSE( probe.m_resume.isPending() );
    }

    SECTION( "schedule rearms a cancelled queued callback" )
    {
        TestEventQueue  queue;
        ContinuationProbe probe( queue );

        probe.m_resume.schedule();
        probe.m_resume.cancel();
        probe.m_resume.schedule();

        queue.dispatchOne();
        REQUIRE( probe.callbacks == 1 );
        REQUIRE_FALSE( probe.m_resume.isPending() );
    }

    SECTION( "callback can schedule a continuation" )
    {
        TestEventQueue  queue;
        ContinuationProbe probe( queue );
        probe.rescheduleOnResume = true;

        probe.m_resume.schedule();
        queue.dispatchOne();
        REQUIRE( probe.callbacks == 1 );
        REQUIRE( probe.m_resume.isPending() );

        queue.dispatchOne();
        REQUIRE( probe.callbacks == 2 );
        REQUIRE_FALSE( probe.m_resume.isPending() );
    }
}