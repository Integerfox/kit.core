/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/ListItem.h"
#include "catch2/catch_test_macros.hpp"   
#include "Kit/Container/MapItem.h"    
#include "Kit/Container/SList.h"    
#include "Kit/Container/DList.h"    
#include "Kit/Container/Map.h"    
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include <string.h>
#include <stdio.h>

/// 
using namespace Kit::Container;
using namespace Kit::System;


/// Short hand for brute forcing string compares when not using std::string
#define STRING_EQ(a,b)  strcmp(a,b) == 0


////////////////////////////////////////////////////////////////////////////////

/// Use un-named namespace to make my class local-to-the-file in scope
namespace {


class MyItemMap : public MapItem,
    public KeyLiteralString
{
public:
    ///
    MyItemMap( const char* name )
        :KeyLiteralString( name )
    {
    }

public:
    ///  API from SortedItem
    const Key& getKey() const noexcept { return *this; }

};

void printMap( Map<MyItemMap>& map )
{
    printf( "Map (%p)\n", &map );
    MyItemMap* ptr = map.first();
    while ( ptr )
    {
        printf( "[%s]\n", ptr->getKeyValue() );
        ptr = map.next( *ptr );
    }

}

class ItemAutoAdd : public MapItem,
    public KeyInteger16_T
{
public:
    ///
    ItemAutoAdd( Map<ItemAutoAdd>& list, const char* name, int16_t keyValue=0 )
        :KeyInteger16_T( keyValue ),
        m_name( name )
    {
        list.insert( *this );
    }

    ///
    const char* m_name;

public:
    ///  API from SortedItem
    const Key& getKey() const noexcept { return *this; }
};



}; // end namespace

////////////////////////////////////////////////////////////////////////////////
static Map<ItemAutoAdd> emptymap_( "static constructor" );
static Map<ItemAutoAdd> staticmap_( "static constructor" );
static ItemAutoAdd      static1Item_A( staticmap_, "staticItemA (-2)", -2 );
static ItemAutoAdd      static1Item_B( staticmap_, "staticItemB (6)", 6 );
static ItemAutoAdd      static1Item_C( staticmap_, "staticItemV (-3)", -3 );




////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Map" )
{
    Map<MyItemMap> map;
    MyItemMap      apple( "apple" );
    MyItemMap      orange( "orange" );
    MyItemMap      cherry( "cherry" );
    MyItemMap      pear( "pear" );
    MyItemMap      plum( "plum" );
    MyItemMap*     ptr1;
        
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "Validate that an 'item' can be only in one Container" )
    {
        Map<MyItemMap> foo;
        Map<MyItemMap> bar;

        MyItemMap item( "bob" );

        foo.insert( item );
        bar.insert( item );

        REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 1u );
    }

    SECTION( "Validate static Constructor" )
    {
        REQUIRE( emptymap_.first() == nullptr );
        REQUIRE( emptymap_.last() == nullptr );

        ItemAutoAdd* ptr2 = staticmap_.find( static1Item_A );
        REQUIRE( ptr2 != nullptr );
        REQUIRE( ptr2->getKeyValue() == -2 );

        ptr2 = staticmap_.first();
        REQUIRE( ptr2 != nullptr );
        REQUIRE( ptr2->getKeyValue() == -3 );
        ptr2 = staticmap_.next( *ptr2 );
        REQUIRE( ptr2 != nullptr );
        REQUIRE( ptr2->getKeyValue() == -2 );
        ptr2 = staticmap_.next( *ptr2 );
        REQUIRE( ptr2 != nullptr );
        REQUIRE( ptr2->getKeyValue() == 6 );
        ptr2 = staticmap_.next( *ptr2 );
        REQUIRE( ptr2 == 0 );

        // Look-up by key
        KeyInteger16_T key6  = 6;
        KeyInteger16_T key0  = 0;
        KeyInteger16_T key_2 = -2;

        ptr2 = staticmap_.find( key6 );
        REQUIRE( ptr2 != nullptr );
        REQUIRE( ptr2->getKeyValue() == 6 );
        ptr2 = staticmap_.find( key0 );
        REQUIRE( ptr2 == 0 );
        ptr2 = staticmap_.find( key_2 );
        REQUIRE( ptr2 != nullptr );
        REQUIRE( ptr2->getKeyValue() == -2 );


        // Sneak in test for bogus keys
        REQUIRE( staticmap_.find( cherry ) == nullptr );
    }

    SECTION( "Basic" )
    {
        REQUIRE( map.getFirst() == nullptr );
        REQUIRE( map.first() == nullptr );
        REQUIRE( map.last() == nullptr );

        map.insert( apple );

        REQUIRE( map.first() != nullptr );
        REQUIRE( STRING_EQ( map.first()->m_stringKeyPtr, "apple" ) );
        REQUIRE( map.last() != nullptr );
        REQUIRE( STRING_EQ( map.last()->m_stringKeyPtr, "apple" ) );

        map.insert( orange );

        REQUIRE( map.first() != nullptr );
        REQUIRE( STRING_EQ( map.first()->m_stringKeyPtr, "apple" ) );
        REQUIRE( map.last() != nullptr );
        REQUIRE( STRING_EQ( map.last()->m_stringKeyPtr, "orange" ) );

        map.insert( cherry );

        REQUIRE( map.first() != nullptr );
        REQUIRE( STRING_EQ( map.first()->m_stringKeyPtr, "apple" ) );
        REQUIRE( map.last() != nullptr );
        REQUIRE( STRING_EQ( map.last()->m_stringKeyPtr, "orange" ) );

        ptr1 = map.getLast();
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );
        REQUIRE( map.first() != nullptr );
        REQUIRE( STRING_EQ( map.first()->m_stringKeyPtr, "apple" ) );
        REQUIRE( map.last() != nullptr );
        REQUIRE( STRING_EQ( map.last()->m_stringKeyPtr, "cherry" ) );

        ptr1 = map.getFirst();
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "apple" ) );
        REQUIRE( map.first() != nullptr );
        REQUIRE( STRING_EQ( map.first()->m_stringKeyPtr, "cherry" ) );
        REQUIRE( map.last() != nullptr );
        REQUIRE( STRING_EQ( map.last()->m_stringKeyPtr, "cherry" ) );

        ptr1 = map.getFirst();
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "cherry" ) );
        REQUIRE( map.first() == nullptr );
        REQUIRE( map.last() == nullptr );

        REQUIRE( map.first() == nullptr );
        REQUIRE( map.last() == nullptr );
        REQUIRE( map.getFirst() == nullptr );
        REQUIRE( map.getLast() == nullptr );
    }

    SECTION( "More Basic ops" )
    {
        MyItemMap pear1( "pear1" );

        REQUIRE( map.insert( orange ) == true );
        REQUIRE( map.insert( pear1 ) == true );
        REQUIRE( map.insert( pear ) == true );
        REQUIRE( map.insert( cherry ) == true );
        REQUIRE( map.insert( apple ) == true );
        REQUIRE( map.insert( orange ) == false );

        printMap( map );

        KeyLiteralString key1( "Hello" );
        MyItemMap* ptr1 = map.find( key1 );
        REQUIRE( ptr1 == nullptr );

        KeyLiteralString key2( "orange" );
        ptr1 = map.find( key2 );
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );

        ptr1 = map.last();
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "pear1" ) );
        ptr1 = map.previous( *ptr1 );
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "pear" ) );
        ptr1 = map.previous( *ptr1 );
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );
        ptr1 = map.previous( *ptr1 );
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "cherry" ) );
        ptr1 = map.previous( *ptr1 );
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "apple" ) );
        REQUIRE( map.previous( *ptr1 ) == nullptr );

        ptr1 = map.next( *ptr1 );
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "cherry" ) );
        ptr1 = map.next( *ptr1 );
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );
        ptr1 = map.next( *ptr1 );
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "pear" ) );
        ptr1 = map.next( *ptr1 );
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "pear1" ) );
        REQUIRE( map.next( *ptr1 ) == nullptr );

        bool result = map.removeItem( orange );
        REQUIRE( result == true );
        REQUIRE( map.removeItem( orange ) == false );
        REQUIRE( map.remove( key2 ) == nullptr );
        map.insert( orange );
        ptr1 = map.remove( key2 );
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );

        Map<MyItemMap> dst;
        REQUIRE( dst.first() == nullptr );
        REQUIRE( map.first() != nullptr );
        map.move( dst );
        REQUIRE( dst.first() != nullptr );
        REQUIRE( map.first() == nullptr );
        ptr1 = dst.last();
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "pear1" ) );
        dst.clearTheMap();
        REQUIRE( dst.first() == nullptr );
    }



    // 
    // Verify MapItem works in a DList
    //
    SECTION( "DLIST: FIFO with MapItem" )
    {
        DList<MyItemMap> dlist;

        REQUIRE( dlist.get() == nullptr );
        REQUIRE( dlist.head() == nullptr );
        REQUIRE( dlist.tail() == nullptr );

        dlist.put( apple );

        REQUIRE( dlist.head() != nullptr );
        REQUIRE( STRING_EQ( dlist.head()->m_stringKeyPtr, "apple" ) );
        REQUIRE( dlist.tail() != nullptr );
        REQUIRE( STRING_EQ( dlist.tail()->m_stringKeyPtr, "apple" ) );

        dlist.put( orange );

        REQUIRE( dlist.head() != nullptr );
        REQUIRE( STRING_EQ( dlist.head()->m_stringKeyPtr, "apple" ) );
        REQUIRE( dlist.tail() != nullptr );
        REQUIRE( STRING_EQ( dlist.tail()->m_stringKeyPtr, "orange" ) );

        dlist.put( cherry );

        REQUIRE( dlist.head() != nullptr );
        REQUIRE( STRING_EQ( dlist.head()->m_stringKeyPtr, "apple" ) );
        REQUIRE( dlist.tail() != nullptr );
        REQUIRE( STRING_EQ( dlist.tail()->m_stringKeyPtr, "cherry" ) );

        ptr1 = dlist.get();
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "apple" ) );
        REQUIRE( dlist.head() != nullptr );
        REQUIRE( STRING_EQ( dlist.head()->m_stringKeyPtr, "orange" ) );
        REQUIRE( dlist.tail() != nullptr );
        REQUIRE( STRING_EQ( dlist.tail()->m_stringKeyPtr, "cherry" ) );

        ptr1 = dlist.get();
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );
        REQUIRE( dlist.head() != nullptr );
        REQUIRE( STRING_EQ( dlist.head()->m_stringKeyPtr, "cherry" ) );
        REQUIRE( dlist.tail() != nullptr );
        REQUIRE( STRING_EQ( dlist.tail()->m_stringKeyPtr, "cherry" ) );

        ptr1 = dlist.get();
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "cherry" ) );
        REQUIRE( dlist.head() == nullptr );
        REQUIRE( dlist.tail() == nullptr );

        REQUIRE( dlist.get() == nullptr );
        REQUIRE( dlist.head() == nullptr );
        REQUIRE( dlist.tail() == nullptr );
    }

    // 
    // Verify MapItem works in a DList
    //
    SECTION( "SLIST: FIFO with MapItem" )
    {
        SList<MyItemMap> slist;

        REQUIRE( slist.get() == nullptr );
        REQUIRE( slist.head() == nullptr );
        REQUIRE( slist.tail() == nullptr );

        slist.put( apple );

        REQUIRE( slist.head() != nullptr );
        REQUIRE( STRING_EQ( slist.head()->m_stringKeyPtr, "apple" ) );
        REQUIRE( slist.tail() != nullptr );
        REQUIRE( STRING_EQ( slist.tail()->m_stringKeyPtr, "apple" ) );

        slist.put( orange );

        REQUIRE( slist.head() != nullptr );
        REQUIRE( STRING_EQ( slist.head()->m_stringKeyPtr, "apple" ) );
        REQUIRE( slist.tail() != nullptr );
        REQUIRE( STRING_EQ( slist.tail()->m_stringKeyPtr, "orange" ) );

        slist.put( cherry );

        REQUIRE( slist.head() != nullptr );
        REQUIRE( STRING_EQ( slist.head()->m_stringKeyPtr, "apple" ) );
        REQUIRE( slist.tail() != nullptr );
        REQUIRE( STRING_EQ( slist.tail()->m_stringKeyPtr, "cherry" ) );

        ptr1 = slist.get();
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "apple" ) );
        REQUIRE( slist.head() != nullptr );
        REQUIRE( STRING_EQ( slist.head()->m_stringKeyPtr, "orange" ) );
        REQUIRE( slist.tail() != nullptr );
        REQUIRE( STRING_EQ( slist.tail()->m_stringKeyPtr, "cherry" ) );

        ptr1 = slist.get();
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "orange" ) );
        REQUIRE( slist.head() != nullptr );
        REQUIRE( STRING_EQ( slist.head()->m_stringKeyPtr, "cherry" ) );
        REQUIRE( slist.tail() != nullptr );
        REQUIRE( STRING_EQ( slist.tail()->m_stringKeyPtr, "cherry" ) );

        ptr1 = slist.get();
        REQUIRE( ptr1 != nullptr );
        REQUIRE( STRING_EQ( ptr1->m_stringKeyPtr, "cherry" ) );
        REQUIRE( slist.head() == nullptr );
        REQUIRE( slist.tail() == nullptr );

        REQUIRE( slist.get() == nullptr );
        REQUIRE( slist.head() == nullptr );
        REQUIRE( slist.tail() == nullptr );
    }


    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
