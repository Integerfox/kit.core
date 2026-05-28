/*-----------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/Container/KeyedItem.h"
#include "Kit/Container/Key.h"
#include "Kit/Container/OrderedList.h"
#include <string.h>


///
using namespace Kit::Container;
using namespace Kit::System;


/// Short hand for brute forcing string compares when not using std::string
#define STRING_EQ( a, b ) strcmp( a, b ) == 0


////////////////////////////////////////////////////////////////////////////////

/// Use un-named namespace to make my class local-to-the-file in scope
namespace {

class MyItem : public KeyedItem, public KeyLiteralString
{
public:
    ///
    MyItem( const char* name, unsigned value )
        : KeyLiteralString( name )
        , m_value( value )
    {
    }

    const Key& getKey() const noexcept override { return *this; }
    const char* getName() const noexcept { return m_stringKeyPtr; }

    ///
    const char* m_name;
    unsigned    m_value;
};

class ItemAutoAdd : public KeyedItem, public KeyLiteralString
{
public:
    ///
    ItemAutoAdd( const char* name, unsigned value, OrderedList<ItemAutoAdd>& list )
        : KeyLiteralString( name )
        , m_value( value )
    {
        list.insert( *this );
    }

    const Key& getKey() const noexcept override { return *this; }
    const char* getName() const noexcept { return m_stringKeyPtr; }

    ///
    const char* m_name;
    unsigned    m_value;
};

};  // end namespace

////////////////////////////////////////////////////////////////////////////////
static OrderedList<ItemAutoAdd> emptylist_( "static constructor" );
static OrderedList<ItemAutoAdd> staticlist_( "static constructor" );
static ItemAutoAdd              staticItem_( "staticItem", 0, staticlist_ );


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "OrderedList" )
{
    OrderedList<MyItem> list;
    MyItem              apple( "apple", 0 );
    MyItem              orange( "orange", 1 );
    MyItem              cherry( "cherry", 2 );
    MyItem              pear( "pear", 3 );
    MyItem              plum( "plum", 4 );
    MyItem*             ptr1;

    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "Validate that an 'item' can be only in one Container" )
    {
        OrderedList<MyItem> foo;
        OrderedList<MyItem> bar;

        MyItem item( "bob", 0 );

        foo.insert( item );
        bar.insert( item );

        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 1u );
    }

    SECTION( "Validate static Constructor" )
    {
        REQUIRE( emptylist_.first() == nullptr );
        REQUIRE( emptylist_.last() == nullptr );

        REQUIRE( staticlist_.first() != nullptr );
        REQUIRE( staticlist_.last() != nullptr );
        REQUIRE( STRING_EQ( staticlist_.first()->getName(), "staticItem" ) );
        REQUIRE( STRING_EQ( staticlist_.last()->getName(), "staticItem" ) );
    }

    SECTION( "List Operations" )
    {
        OrderedList<MyItem> foo;
        OrderedList<MyItem> bar;
        MyItem              bob( "bob", 0 );
        MyItem              yours( "yours", 1 );
        MyItem              uncle( "uncle", 2 );

        foo.insert( bob );
        foo.insert( yours );
        foo.insert( uncle );
        REQUIRE( STRING_EQ( foo.first()->getName(), "bob" ) );
        REQUIRE( STRING_EQ( foo.last()->getName(), "yours" ) );
        REQUIRE( bar.first() == nullptr );
        REQUIRE( bar.last() == nullptr );

        foo.move( bar );
        REQUIRE( STRING_EQ( bar.first()->getName(), "bob" ) );
        REQUIRE( STRING_EQ( bar.last()->getName(), "yours" ) );
        REQUIRE( foo.first() == nullptr );
        REQUIRE( foo.last() == nullptr );

        bar.clearTheList();
        REQUIRE( bar.first() == nullptr );
        REQUIRE( bar.last() == nullptr );

        REQUIRE( foo.next( bob ) == nullptr );
        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 1u );
    }

    SECTION( "FIFO" )
    {
        REQUIRE( list.getFirst() == nullptr );
        REQUIRE( list.first() == nullptr );
        REQUIRE( list.last() == nullptr );

        list.insert( apple );

        REQUIRE( list.first() != nullptr );
        REQUIRE( STRING_EQ( list.first()->getName(), "apple" ) );
        REQUIRE( list.last() != nullptr );
        REQUIRE( STRING_EQ( list.last()->getName(), "apple" ) );

        list.insert( orange );

        REQUIRE( list.first() != nullptr );
        REQUIRE( STRING_EQ( list.first()->getName(), "apple" ) );
        REQUIRE( list.last() != nullptr );
        REQUIRE( STRING_EQ( list.last()->getName(), "orange" ) );

        list.insert( cherry );

        REQUIRE( list.first() != nullptr );
        REQUIRE( STRING_EQ( list.first()->getName(), "apple" ) );
        REQUIRE( list.last() != nullptr );
        REQUIRE( STRING_EQ( list.last()->getName(), "orange" ) );

        ptr1 = list.getFirst();
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->getName(), "apple" ) );
        REQUIRE( list.first() != nullptr );
        REQUIRE( STRING_EQ( list.first()->getName(), "cherry" ) );
        REQUIRE( list.last() != nullptr );
        REQUIRE( STRING_EQ( list.last()->getName(), "orange" ) );

        ptr1 = list.getFirst();
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->getName(), "cherry" ) );
        REQUIRE( list.first() != nullptr );
        REQUIRE( STRING_EQ( list.first()->getName(), "orange" ) );
        REQUIRE( STRING_EQ( list.last()->getName(), "orange" ) );

        ptr1 = list.getFirst();
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->getName(), "orange" ) );
        REQUIRE( list.first() == nullptr );
        REQUIRE( list.last() == nullptr );

        REQUIRE( list.getFirst() == nullptr );
        REQUIRE( list.first() == nullptr );
        REQUIRE( list.last() == nullptr );
    }


    SECTION( "Order" )
    {
        list.insert( cherry );
        list.insert( pear );
        REQUIRE( STRING_EQ( list.first()->getName(), "cherry" ) );
        REQUIRE( STRING_EQ( list.last()->getName(), "pear" ) );

        list.insert( plum );
        list.insert( apple );
        REQUIRE( STRING_EQ( list.first()->getName(), "apple" ) );
        REQUIRE( STRING_EQ( list.last()->getName(), "plum" ) );

        ptr1 = list.first();
        REQUIRE( STRING_EQ( ptr1->getName(), "apple" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->getName(), "cherry" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->getName(), "pear" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->getName(), "plum" ) );

        list.insert( orange );
        list.remove( plum );
        ptr1 = list.first();
        REQUIRE( STRING_EQ( ptr1->getName(), "apple" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->getName(), "cherry" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->getName(), "orange" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->getName(), "pear" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( ptr1 == nullptr );

        REQUIRE( list.remove( plum ) == false );
        REQUIRE( list.remove( cherry ) == true );
        REQUIRE( list.remove( apple ) == true );
        ptr1 = list.first();
        REQUIRE( STRING_EQ( ptr1->getName(), "orange" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->getName(), "pear" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( ptr1 == nullptr );

        REQUIRE( list.find( plum ) == false );
        REQUIRE( list.find( orange ) == true );
        REQUIRE( list.find( pear ) == true );
        REQUIRE( list.find( apple ) == false );
        REQUIRE( list.find( cherry ) == false );

        REQUIRE( list.remove( orange ) == true );
        REQUIRE( list.remove( apple ) == false );
        REQUIRE( list.remove( pear ) == true );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
