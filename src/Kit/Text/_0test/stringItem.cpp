/*------------------------------------------------------------------------------
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
#include "Kit/Container/MapItem.h"
#include "Kit/Container/SList.h"
#include "Kit/Container/DList.h"
#include "Kit/Container/Map.h"
#include "Kit/Text/FStringItem.h"


///
using namespace Kit::Text;
using namespace Kit::System;
using namespace Kit::Container;

/// Short hand for brute forcing string compares when not using std::string
#define STRING_EQ( a, b ) strcmp( a, b ) == 0


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "StringItem" )
{

    ShutdownUnitTesting::clearAndUseCounter();

    Map<StringItem> map;
    FStringItem<15> apple( "apple" );
    FStringItem<15> orange( "orange" );
    FStringItem<15> cherry( "cherry" );
    FStringItem<15> pear( "pear" );
    FStringItem<15> plum( "plum" );
    StringItem*     ptr1;


    SECTION( "Validate that an 'item' can be only in one Container" )
    {
        Map<StringItem> foo;
        Map<StringItem> bar;

        FStringItem<4> item( "bob" );

        foo.insert( item );
        bar.insert( item );

        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 1u );
    }


    SECTION( "StringItem methods" )
    {
        IString& myStringRef = apple.get();
        REQUIRE( myStringRef == "apple" );

        const char* ptr = plum.getString();
        REQUIRE( STRING_EQ( ptr, "plum" ) );

        ptr = (const char*)orange;
        REQUIRE( STRING_EQ( ptr, "orange" ) );

        IString& myStringRef2 = (IString&)pear;
        REQUIRE( myStringRef2 == "pear" );

        REQUIRE( STRING_EQ( cherry(), "cherry" ) );
    }


    SECTION( "MAP" )
    {
        map.insert( orange );
        map.insert( plum );
        map.insert( pear );
        map.insert( cherry );
        map.insert( apple );

        KeyLiteralString key1( "Hello" );
        ptr1 = map.find( key1 );
        REQUIRE( ptr1 == 0 );

        KeyLiteralString key2( "orange" );
        ptr1 = map.find( key2 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "orange" );

        ptr1 = map.last();
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "plum" );
        ptr1 = map.previous( *ptr1 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "pear" );
        ptr1 = map.previous( *ptr1 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "orange" );
        ptr1 = map.previous( *ptr1 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "cherry" );
        ptr1 = map.previous( *ptr1 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "apple" );
        REQUIRE( map.previous( *ptr1 ) == 0 );

        ptr1 = map.next( *ptr1 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "cherry" );
        ptr1 = map.next( *ptr1 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "orange" );
        ptr1 = map.next( *ptr1 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "pear" );
        ptr1 = map.next( *ptr1 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "plum" );
        REQUIRE( map.next( *ptr1 ) == 0 );

        bool result = map.removeItem( orange );
        REQUIRE( result == true );
        REQUIRE( map.removeItem( orange ) == false );
        REQUIRE( map.remove( key2 ) == 0 );
        map.insert( orange );
        ptr1 = map.remove( key2 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "orange" );

        Map<StringItem> dst;
        REQUIRE( dst.first() == 0 );
        REQUIRE( map.first() != 0 );
        map.move( dst );
        REQUIRE( dst.first() != 0 );
        REQUIRE( map.first() == 0 );
        ptr1 = dst.last();
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "plum" );
        dst.clearTheMap();
        REQUIRE( dst.first() == 0 );
    }

    // Verify MapItem works in a DList
    //
    SECTION( "DLIST" )
    {
        DList<StringItem> dlist;

        REQUIRE( dlist.get() == 0 );
        REQUIRE( dlist.head() == 0 );
        REQUIRE( dlist.tail() == 0 );

        dlist.put( apple );

        REQUIRE( dlist.head() != 0 );
        REQUIRE( dlist.head()->get() == "apple" );
        REQUIRE( dlist.tail() != 0 );
        REQUIRE( dlist.tail()->get() == "apple" );

        dlist.put( orange );

        REQUIRE( dlist.head() != 0 );
        REQUIRE( dlist.head()->get() == "apple" );
        REQUIRE( dlist.tail() != 0 );
        REQUIRE( dlist.tail()->get() == "orange" );

        dlist.put( cherry );

        REQUIRE( dlist.head() != 0 );
        REQUIRE( dlist.head()->get() == "apple" );
        REQUIRE( dlist.tail() != 0 );
        REQUIRE( dlist.tail()->get() == "cherry" );

        ptr1 = dlist.get();
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "apple" );
        REQUIRE( dlist.head() != 0 );
        REQUIRE( dlist.head()->get() == "orange" );
        REQUIRE( dlist.tail() != 0 );
        REQUIRE( dlist.tail()->get() == "cherry" );

        ptr1 = dlist.get();
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "orange" );
        REQUIRE( dlist.head() != 0 );
        REQUIRE( dlist.head()->get() == "cherry" );
        REQUIRE( dlist.tail() != 0 );
        REQUIRE( dlist.tail()->get() == "cherry" );

        ptr1 = dlist.get();
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "cherry" );
        REQUIRE( dlist.head() == 0 );
        REQUIRE( dlist.tail() == 0 );

        REQUIRE( dlist.get() == 0 );
        REQUIRE( dlist.head() == 0 );
        REQUIRE( dlist.tail() == 0 );
    }

    //
    // Verify MapItem works in a SList
    //
    SECTION( "SLIST" )
    {
        SList<StringItem> slist;

        REQUIRE( slist.get() == 0 );
        REQUIRE( slist.head() == 0 );
        REQUIRE( slist.tail() == 0 );

        slist.put( apple );

        REQUIRE( slist.head() != 0 );
        REQUIRE( slist.head()->get() == "apple" );
        REQUIRE( slist.tail() != 0 );
        REQUIRE( slist.tail()->get() == "apple" );

        slist.put( orange );

        REQUIRE( slist.head() != 0 );
        REQUIRE( slist.head()->get() == "apple" );
        REQUIRE( slist.tail() != 0 );
        REQUIRE( slist.tail()->get() == "orange" );

        slist.put( cherry );

        REQUIRE( slist.head() != 0 );
        REQUIRE( slist.head()->get() == "apple" );
        REQUIRE( slist.tail() != 0 );
        REQUIRE( slist.tail()->get() == "cherry" );

        ptr1 = slist.get();
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "apple" );
        REQUIRE( slist.head() != 0 );
        REQUIRE( slist.head()->get() == "orange" );
        REQUIRE( slist.tail() != 0 );
        REQUIRE( slist.tail()->get() == "cherry" );

        ptr1 = slist.get();
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "orange" );
        REQUIRE( slist.head() != 0 );
        REQUIRE( slist.head()->get() == "cherry" );
        REQUIRE( slist.tail() != 0 );
        REQUIRE( slist.tail()->get() == "cherry" );

        ptr1 = slist.get();
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "cherry" );
        REQUIRE( slist.head() == 0 );
        REQUIRE( slist.tail() == 0 );

        REQUIRE( slist.get() == 0 );
        REQUIRE( slist.head() == 0 );
        REQUIRE( slist.tail() == 0 );
    }


#if 0
    SECTION( "MAP: DString" )
    {
        DStringItem pear1( "pear1" );
        DStringItem apple( "apple" );
        DStringItem orange( "orange" );
        DStringItem cherry( "cherry" );
        DStringItem pear( "pear" );


        REQUIRE( map.insert( orange ) == true );
        REQUIRE( map.insert( pear1 ) == true );
        REQUIRE( map.insert( pear ) == true );
        REQUIRE( map.insert( cherry ) == true );
        REQUIRE( map.insert( apple ) == true );
        REQUIRE( map.insert( orange ) == false );

        KeyLiteralString key1( "Hello" );
        ptr1 = map.find( key1 );
        REQUIRE( ptr1 == 0 );

        KeyLiteralString key2( "orange" );
        ptr1 = map.find( key2 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "orange" );

        ptr1 = map.last();
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "pear1" );
        ptr1 = map.previous( *ptr1 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "pear" );
        ptr1 = map.previous( *ptr1 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "orange" );
        ptr1 = map.previous( *ptr1 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "cherry" );
        ptr1 = map.previous( *ptr1 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "apple" );
        REQUIRE( map.previous( *ptr1 ) == 0 );

        ptr1 = map.next( *ptr1 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "cherry" );
        ptr1 = map.next( *ptr1 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "orange" );
        ptr1 = map.next( *ptr1 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "pear" );
        ptr1 = map.next( *ptr1 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "pear1" );
        REQUIRE( map.next( *ptr1 ) == 0 );

        bool result = map.removeItem( orange );
        REQUIRE( result == true );
        REQUIRE( map.removeItem( orange ) == false );
        REQUIRE( map.remove( key2 ) == 0 );
        map.insert( orange );
        ptr1 = map.remove( key2 );
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "orange" );

        Map<StringItem> dst;
        REQUIRE( dst.first() == 0 );
        REQUIRE( map.first() != 0 );
        map.move( dst );
        REQUIRE( dst.first() != 0 );
        REQUIRE( map.first() == 0 );
        ptr1 = dst.last();
        REQUIRE( ptr1 != 0 );
        REQUIRE( ptr1->get() == "pear1" );
        dst.clearTheMap();
        REQUIRE( dst.first() == 0 );
    }
#endif

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
