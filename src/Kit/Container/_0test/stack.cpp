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
#include "Kit/Container/Stack.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include <string.h>


/// 
using namespace Kit::Container;
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////

int memoryStack_[5];


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "Stack" )
{
    Stack<int> stack( sizeof( memoryStack_ ) / sizeof( memoryStack_[0] ), memoryStack_ );

    ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "Operations" )
    {
        int  val;

        REQUIRE( stack.isEmpty() == true );
        REQUIRE( stack.isFull() == false );
        REQUIRE( stack.peekTop( val ) == false );
        REQUIRE( stack.getNumItems() == 0 );
        REQUIRE( stack.getMaxItems() == 5 );

        REQUIRE( stack.push( 10 ) == true );
        REQUIRE( stack.isEmpty() == false );
        REQUIRE( stack.isFull() == false );
        REQUIRE( stack.getNumItems() == 1 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val )  );
        REQUIRE( val == 10 );
        REQUIRE( stack.peekTop( val )  );
        REQUIRE( val == 10 );

        REQUIRE( stack.push( 20 ) == true );
        REQUIRE( stack.isEmpty() == false );
        REQUIRE( stack.isFull() == false );
        REQUIRE( stack.getNumItems() == 2 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val ) );
        REQUIRE( val == 20 );

        REQUIRE( stack.push( 30 ) == true );
        REQUIRE( stack.isEmpty() == false );
        REQUIRE( stack.isFull() == false );
        REQUIRE( stack.getNumItems() == 3 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val )  );
        REQUIRE( val == 30 );

        REQUIRE( stack.push( 40 ) == true );
        REQUIRE( stack.isEmpty() == false );
        REQUIRE( stack.isFull() == false );
        REQUIRE( stack.getNumItems() == 4 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val )  );
        REQUIRE( val == 40 );

        REQUIRE( stack.push( 50 ) == true );
        REQUIRE( stack.isEmpty() == false );
        REQUIRE( stack.isFull() == true );
        REQUIRE( stack.getNumItems() == 5 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val ));
        REQUIRE( val == 50 );

        REQUIRE( stack.push( 60 ) == false );
        REQUIRE( stack.isEmpty() == false );
        REQUIRE( stack.isFull() == true );
        REQUIRE( stack.getNumItems() == 5 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val )  );
        REQUIRE( val == 50 );


        REQUIRE( stack.pop( val ) );
        REQUIRE( val == 50 );
        REQUIRE( stack.isEmpty() == false );
        REQUIRE( stack.isFull() == false );
        REQUIRE( stack.getNumItems() == 4 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val )  );
        REQUIRE( val == 40 );

        REQUIRE( stack.pop( val ) );
        REQUIRE( val == 40 );
        REQUIRE( stack.isEmpty() == false );
        REQUIRE( stack.isFull() == false );
        REQUIRE( stack.getNumItems() == 3 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val )  );
        REQUIRE( val == 30 );

        REQUIRE( stack.push( 60 ) == true );
        REQUIRE( stack.isEmpty() == false );
        REQUIRE( stack.isFull() == false );
        REQUIRE( stack.getNumItems() == 4 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val )  );
        REQUIRE( val == 60 );

        REQUIRE( stack.push( 70 ) == true );
        REQUIRE( stack.isEmpty() == false );
        REQUIRE( stack.isFull() == true );
        REQUIRE( stack.getNumItems() == 5 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val ) );
        REQUIRE( val == 70 );


        REQUIRE( stack.pop( val ) );
        REQUIRE( val == 70 );
        REQUIRE( stack.isEmpty() == false );
        REQUIRE( stack.isFull() == false );
        REQUIRE( stack.getNumItems() == 4 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val ) );
        REQUIRE( val == 60 );

        REQUIRE( stack.pop( val ) );
        REQUIRE( val == 60 );
        REQUIRE( stack.isEmpty() == false );
        REQUIRE( stack.isFull() == false );
        REQUIRE( stack.getNumItems() == 3 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val ) );
        REQUIRE( val == 30 );

        REQUIRE( stack.pop( val ) );
        REQUIRE( val == 30 );
        REQUIRE( stack.isEmpty() == false );
        REQUIRE( stack.isFull() == false );
        REQUIRE( stack.getNumItems() == 2 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val ) );
        REQUIRE( val == 20 );

        REQUIRE( stack.pop( val ) );
        REQUIRE( val == 20 );
        REQUIRE( stack.isEmpty() == false );
        REQUIRE( stack.isFull() == false );
        REQUIRE( stack.getNumItems() == 1 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val ) );
        REQUIRE( val == 10 );

        REQUIRE( stack.pop( val ) );
        REQUIRE( val == 10 );
        REQUIRE( stack.isEmpty() == true );
        REQUIRE( stack.isFull() == false );
        REQUIRE( stack.getNumItems() == 0 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val ) == false );


        REQUIRE( stack.pop( val ) == false);
        REQUIRE( stack.isEmpty() == true );
        REQUIRE( stack.isFull() == false );
        REQUIRE( stack.peekTop( val ) == false );

        REQUIRE( stack.push( 10 ) == true );
        REQUIRE( stack.isEmpty() == false );
        REQUIRE( stack.isFull() == false );
        REQUIRE( stack.getNumItems() == 1 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val ) );
        REQUIRE( val == 10 );

        REQUIRE( stack.push( 20 ) == true );
        REQUIRE( stack.isEmpty() == false );
        REQUIRE( stack.isFull() == false );
        REQUIRE( stack.getNumItems() == 2 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val ) );
        REQUIRE( val == 20 );

        stack.clearTheStack();
        REQUIRE( stack.isEmpty() == true );
        REQUIRE( stack.isFull() == false );
        REQUIRE( stack.getNumItems() == 0 );
        REQUIRE( stack.getMaxItems() == 5 );
        REQUIRE( stack.peekTop( val ) == false );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}