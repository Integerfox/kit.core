/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "catch2/catch_test_macros.hpp"
#include "Kit/Container/RingBuffer.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include <string.h>


///
using namespace Kit::Container;
///
using namespace Kit::System;

#define NUM_ELEMENTS 5


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "RingBuffer" )
{
    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "Operations" )
    {
    RingBuffer<int8_t, NUM_ELEMENTS + 1> uut;

        int8_t item     = 0;
        int8_t peekItem = 0;

        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.peekHead( peekItem ) == false );
        REQUIRE( uut.peekTail( peekItem ) == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.remove( item ) == false );
        item = 10;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 10 );

        item = 20;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 20 );
        item = 30;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 30 );
        item = 40;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 40 );
        item = 50;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == true );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 50 );
        item = 60;
        REQUIRE( uut.add( item ) == false );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == true );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 50 );


        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 20 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 50 );
        REQUIRE( item == 10 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 30 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 50 );
        REQUIRE( item == 20 );
        item = 60;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 30 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 60 );
        item = 70;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == true );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 30 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 70 );


        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 40 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 70 );
        REQUIRE( item == 30 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 50 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 70 );
        REQUIRE( item == 40 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 60 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 70 );
        REQUIRE( item == 50 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 70 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 70 );
        REQUIRE( item == 60 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) == false);
        REQUIRE( uut.peekTail( peekItem ) == false);


        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.peekHead( peekItem ) == false);
        REQUIRE( uut.peekTail( peekItem ) == false);
        REQUIRE( uut.getMaxItems() == 5 );

        item = 10;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 10 );
        item = 20;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 20 );

        uut.clearTheBuffer();
        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.peekHead( peekItem ) == false);
        REQUIRE( uut.peekTail( peekItem ) == false);
        REQUIRE( uut.getMaxItems() == 5 );
    }

    SECTION( "Operations" )
    {
    RingBuffer<uint64_t, NUM_ELEMENTS + 1> uut;

        uint64_t item     = 0;
        uint64_t peekItem = 0;

        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.peekHead( peekItem ) == false );
        REQUIRE( uut.peekTail( peekItem ) == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.remove( item ) == false );
        item = 10;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 10 );

        item = 20;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 20 );
        item = 30;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 30 );
        item = 40;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 40 );
        item = 50;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == true );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 50 );
        item = 60;
        REQUIRE( uut.add( item ) == false );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == true );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 50 );


        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 20 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 50 );
        REQUIRE( item == 10 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 30 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 50 );
        REQUIRE( item == 20 );
        item = 60;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 30 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 60 );
        item = 70;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == true );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 30 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 70 );


        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 40 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 70 );
        REQUIRE( item == 30 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 50 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 70 );
        REQUIRE( item == 40 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 60 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 70 );
        REQUIRE( item == 50 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 70 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 70 );
        REQUIRE( item == 60 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) == false);
        REQUIRE( uut.peekTail( peekItem ) == false);


        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.peekHead( peekItem ) == false);
        REQUIRE( uut.peekTail( peekItem ) == false);
        REQUIRE( uut.getMaxItems() == 5 );

        item = 10;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 10 );
        item = 20;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 20 );

        uut.clearTheBuffer();
        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.peekHead( peekItem ) == false);
        REQUIRE( uut.peekTail( peekItem ) == false);
        REQUIRE( uut.getMaxItems() == 5 );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}