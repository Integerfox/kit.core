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
#include "Kit/Container/Item.h"
#include "Kit/Container/DList.h"
#include <string.h>


///
using namespace Kit::Container;
// using namespace Kit::System;


/// Short hand for brute forcing string compares when not using std::string
#define STRING_EQ( a, b ) strcmp( a, b ) == 0


////////////////////////////////////////////////////////////////////////////////

/// Use un-named namespace to make my class local-to-the-file in scope
namespace {

class MyItem : public ExtendedItem
{
public:
    ///
    MyItem( const char* name )
        : m_name( name ){};
    ///
    const char* m_name;
};

class ItemAutoAdd : public ExtendedItem
{
public:
    ///
    ItemAutoAdd( const char* name, DList<ItemAutoAdd>& list )
        : m_name( name )
    {
        list.put( *this );
    }

    ///
    const char* m_name;
};

};  // end namespace

////////////////////////////////////////////////////////////////////////////////
static DList<ItemAutoAdd> emptylist_( "static constructor" );
static DList<ItemAutoAdd> staticlist_( "static constructor" );
static ItemAutoAdd        staticItem_( "staticItem", staticlist_ );


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "DLIST: Validate member functions", "[dlist]" )
{
    DList<MyItem> list;
    MyItem        apple( "apple" );
    MyItem        orange( "orange" );
    MyItem        cherry( "cherry" );
    MyItem        pear( "pear" );
    MyItem        plum( "plum" );
    MyItem*       ptr1;

    // Shutdown_TS::clearAndUseCounter();

    SECTION( "Validate that an 'item' can be only in one Container" )
    {
        DList<MyItem> foo;
        DList<MyItem> bar;

        MyItem item( "bob" );

        foo.put( item );
        bar.put( item );

        REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 1u );
    }

    SECTION( "Validate static Constructor" )
    {
        REQUIRE( emptylist_.head() == nullptr );
        REQUIRE( emptylist_.tail() == nullptr );

        REQUIRE( staticlist_.head() != 0 );
        REQUIRE( staticlist_.tail() != 0 );
        REQUIRE( STRING_EQ( staticlist_.get()->m_name, "staticItem" ) );
    }

    SECTION( "List Operations" )
    {
        DList<MyItem> foo;
        DList<MyItem> bar;
        MyItem        bob( "bob" );
        MyItem        yours( "yours" );
        MyItem        uncle( "uncle" );

        foo.put( bob );
        foo.put( yours );
        foo.put( uncle );
        REQUIRE( STRING_EQ( foo.head()->m_name, "bob" ) );
        REQUIRE( STRING_EQ( foo.tail()->m_name, "uncle" ) );
        REQUIRE( bar.head() == nullptr );
        REQUIRE( bar.tail() == nullptr );

        foo.move( bar );
        REQUIRE( STRING_EQ( bar.head()->m_name, "bob" ) );
        REQUIRE( STRING_EQ( bar.tail()->m_name, "uncle" ) );
        REQUIRE( foo.head() == nullptr );
        REQUIRE( foo.tail() == nullptr );

        bar.clearTheList();
        REQUIRE( bar.head() == nullptr );
        REQUIRE( bar.tail() == nullptr );

        REQUIRE( foo.next( bob ) == nullptr );
        REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 1u );
    }

    SECTION( "FIFO" )
    {
        REQUIRE( list.get() == nullptr );
        REQUIRE( list.head() == nullptr );
        REQUIRE( list.tail() == nullptr );

        list.put( apple );

        REQUIRE( list.head() != 0 );
        REQUIRE( STRING_EQ( list.head()->m_name, "apple" ) );
        REQUIRE( list.tail() != 0 );
        REQUIRE( STRING_EQ( list.tail()->m_name, "apple" ) );

        list.put( orange );

        REQUIRE( list.head() != 0 );
        REQUIRE( STRING_EQ( list.head()->m_name, "apple" ) );
        REQUIRE( list.tail() != 0 );
        REQUIRE( STRING_EQ( list.tail()->m_name, "orange" ) );

        list.put( cherry );

        REQUIRE( list.head() != 0 );
        REQUIRE( STRING_EQ( list.head()->m_name, "apple" ) );
        REQUIRE( list.tail() != 0 );
        REQUIRE( STRING_EQ( list.tail()->m_name, "cherry" ) );

        ptr1 = list.get();
        REQUIRE( ptr1 != 0 );
        REQUIRE( STRING_EQ( ptr1->m_name, "apple" ) );
        REQUIRE( list.head() != 0 );
        REQUIRE( STRING_EQ( list.head()->m_name, "orange" ) );
        REQUIRE( list.tail() != 0 );
        REQUIRE( STRING_EQ( list.tail()->m_name, "cherry" ) );

        ptr1 = list.get();
        REQUIRE( ptr1 != 0 );
        REQUIRE( STRING_EQ( ptr1->m_name, "orange" ) );
        REQUIRE( list.head() != 0 );
        REQUIRE( STRING_EQ( list.head()->m_name, "cherry" ) );
        REQUIRE( list.tail() != 0 );
        REQUIRE( STRING_EQ( list.tail()->m_name, "cherry" ) );

        ptr1 = list.get();
        REQUIRE( ptr1 != 0 );
        REQUIRE( STRING_EQ( ptr1->m_name, "cherry" ) );
        REQUIRE( list.head() == nullptr );
        REQUIRE( list.tail() == nullptr );

        REQUIRE( list.get() == nullptr );
        REQUIRE( list.head() == nullptr );
        REQUIRE( list.tail() == nullptr );
    }

    SECTION( "STACK" )
    {
        // Note: pop() == get(), top() == head(), push() == putFirst()
        list.push( apple );
        REQUIRE( list.top() != 0 );
        REQUIRE( STRING_EQ( list.head()->m_name, "apple" ) );
        REQUIRE( list.tail() != 0 );
        REQUIRE( STRING_EQ( list.tail()->m_name, "apple" ) );

        ptr1 = list.pop();
        REQUIRE( ptr1 != 0 );
        REQUIRE( STRING_EQ( ptr1->m_name, "apple" ) );
        REQUIRE( list.top() == nullptr );
        REQUIRE( list.tail() == nullptr );
        REQUIRE( list.pop() == nullptr );

        list.push( apple );
        list.push( orange );
        list.push( cherry );
        ptr1 = list.pop();
        list.push( plum );
        list.push( pear );
        REQUIRE( ptr1 != 0 );
        REQUIRE( STRING_EQ( ptr1->m_name, "cherry" ) );
        REQUIRE( STRING_EQ( list.top()->m_name, "pear" ) );
        REQUIRE( STRING_EQ( list.tail()->m_name, "apple" ) );

        REQUIRE( STRING_EQ( list.pop()->m_name, "pear" ) );
        REQUIRE( STRING_EQ( list.pop()->m_name, "plum" ) );
        REQUIRE( STRING_EQ( list.pop()->m_name, "orange" ) );
        REQUIRE( STRING_EQ( list.pop()->m_name, "apple" ) );
        REQUIRE( list.top() == nullptr );
        REQUIRE( list.tail() == nullptr );
        REQUIRE( list.pop() == nullptr );
    }

    SECTION( "Ordered List" )
    {
        // Note: getFirst() == get(), putLast() == put(), putFirst() == push(), first() == head(), last() == tail()
        list.putLast( orange );
        list.putFirst( apple );
        REQUIRE( STRING_EQ( list.first()->m_name, "apple" ) );
        REQUIRE( STRING_EQ( list.tail()->m_name, "orange" ) );

        ptr1 = list.getFirst();
        REQUIRE( ptr1 != 0 );
        REQUIRE( STRING_EQ( ptr1->m_name, "apple" ) );
        REQUIRE( list.head() != 0 );
        REQUIRE( STRING_EQ( list.head()->m_name, "orange" ) );
        REQUIRE( list.tail() != 0 );
        REQUIRE( STRING_EQ( list.tail()->m_name, "orange" ) );
        list.putFirst( *ptr1 );

        ptr1 = list.getLast();
        REQUIRE( ptr1 != 0 );
        REQUIRE( STRING_EQ( ptr1->m_name, "orange" ) );
        REQUIRE( list.head() != 0 );
        REQUIRE( STRING_EQ( list.head()->m_name, "apple" ) );
        REQUIRE( list.tail() != 0 );
        REQUIRE( STRING_EQ( list.tail()->m_name, "apple" ) );
        list.putLast( *ptr1 );

        list.insertBefore( apple, cherry );
        list.insertAfter( orange, plum );
        REQUIRE( STRING_EQ( list.first()->m_name, "cherry" ) );
        REQUIRE( STRING_EQ( list.tail()->m_name, "plum" ) );

        ptr1 = list.first();
        REQUIRE( STRING_EQ( ptr1->m_name, "cherry" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->m_name, "apple" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->m_name, "orange" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->m_name, "plum" ) );

        list.insertBefore( plum, pear );
        list.remove( orange );
        list.insertAfter( cherry, orange );
        ptr1 = list.first();
        REQUIRE( STRING_EQ( ptr1->m_name, "cherry" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->m_name, "orange" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->m_name, "apple" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->m_name, "pear" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->m_name, "plum" ) );

        REQUIRE( list.remove( plum ) == true );
        REQUIRE( list.remove( cherry ) == true );
        REQUIRE( list.remove( plum ) == false );
        ptr1 = list.first();
        REQUIRE( STRING_EQ( ptr1->m_name, "orange" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->m_name, "apple" ) );
        ptr1 = list.next( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->m_name, "pear" ) );

        REQUIRE( list.find( plum ) == false );
        REQUIRE( list.find( orange ) == true );
        REQUIRE( list.find( pear ) == true );
        REQUIRE( list.find( apple ) == true );
        REQUIRE( list.find( cherry ) == false );

        REQUIRE( list.remove( orange ) == true );
        REQUIRE( list.remove( apple ) == true );
        REQUIRE( list.remove( pear ) == true );

        //
        // START 'new' to DList tests...
        //
        list.put( cherry );
        list.put( apple );
        list.put( orange );
        list.put( plum );
        ptr1 = list.last();
        REQUIRE( STRING_EQ( ptr1->m_name, "plum" ) );
        ptr1 = list.previous( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->m_name, "orange" ) );
        ptr1 = list.previous( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->m_name, "apple" ) );
        ptr1 = list.previous( *ptr1 );
        REQUIRE( STRING_EQ( ptr1->m_name, "cherry" ) );
    }

    //
    // START 'new' to DList tests...
    //
    SECTION( "STACK: DList using ExtendedItem" )
    {
        DList<MyItem> list;

        // Note: pop() == get(), top() == head(), push() == putFirst()
        list.push( apple );
        REQUIRE( list.top() != 0 );
        REQUIRE( STRING_EQ( list.head()->m_name, "apple" ) );
        REQUIRE( list.tail() != 0 );
        REQUIRE( STRING_EQ( list.tail()->m_name, "apple" ) );

        ptr1 = list.pop();
        REQUIRE( ptr1 != 0 );
        REQUIRE( STRING_EQ( ptr1->m_name, "apple" ) );
        REQUIRE( list.top() == nullptr );
        REQUIRE( list.tail() == nullptr );
        REQUIRE( list.pop() == nullptr );

        list.push( apple );
        list.push( orange );
        list.push( cherry );
        ptr1 = list.pop();
        list.push( plum );
        list.push( pear );
        REQUIRE( ptr1 != 0 );
        REQUIRE( STRING_EQ( ptr1->m_name, "cherry" ) );
        REQUIRE( STRING_EQ( list.top()->m_name, "pear" ) );
        REQUIRE( STRING_EQ( list.tail()->m_name, "apple" ) );

        REQUIRE( STRING_EQ( list.pop()->m_name, "pear" ) );
        REQUIRE( STRING_EQ( list.pop()->m_name, "plum" ) );
        REQUIRE( STRING_EQ( list.pop()->m_name, "orange" ) );
        REQUIRE( STRING_EQ( list.pop()->m_name, "apple" ) );
        REQUIRE( list.top() == nullptr );
        REQUIRE( list.tail() == nullptr );
        REQUIRE( list.pop() == nullptr );
    }


    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
