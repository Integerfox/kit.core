/*-----------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "catch2/catch_test_macros.hpp"   
#include "Kit/Container/Item.h"    
#include "Kit/Container/SList.h"    
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include <string.h>


/// 
using namespace Kit::Container;
using namespace Kit::System;

#if 0
/// Short hand for brute forcing string compares when not using std::string
#define STRING_EQ(a,b)  strcmp(a,b) == 0


////////////////////////////////////////////////////////////////////////////////

/// Use un-named namespace to make my class local-to-the-file in scope
namespace {

class MyItem : public ListItem
{
public:
    ///
    MyItem( const char* name ) : m_name( name ) {};
    ///
    const char* m_name;
};

class ItemAutoAdd : public ListItem
{
public:
    ///
    ItemAutoAdd( const char* name, SList<ItemAutoAdd>& list )
        :m_name( name )
    {
        list.put( *this );
    }

    ///
    const char* m_name;
};

}; // end namespace


////////////////////////////////////////////////////////////////////////////////
static SList<ItemAutoAdd> emptylist_( "static constructor" );
static SList<ItemAutoAdd> staticlist_( "static constructor" );
static ItemAutoAdd        staticItem_( "staticItem", staticlist_ );

#endif
#include "MyFsm.h"
////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Context" )
{
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "one" )
    {
        Foo::Bar::MyFsm fsm;
        fsm.start();
        REQUIRE( fsm.stateId == Foo::Bar::MyFsmStateId::STATE1 );
        fsm.generateEvent(Foo::Bar::MyFsmEventId::EVENT1);
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
