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
#include "Kit/Container/RingBufferMPAllocate.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "Kit/Dm/ModelDatabase.h"
#include <cstdint>


///
using namespace Kit::Container;
///
using namespace Kit::System;

#define NUM_ELEMENTS 5

namespace {
class MyRingBuffer : public RingBufferMPAllocate<uint64_t, NUM_ELEMENTS + 1>
{
public:
    // Constructor
    MyRingBuffer( Kit::Dm::Mp::Uint32& mpElementCount )
        : RingBufferMPAllocate<uint64_t, NUM_ELEMENTS + 1>( mpElementCount )
    {
    }

    uint64_t* getMemoryPtr()
    {
        return m_rawMemory;
    }
};

}  // end anonymous namespace

static Kit::Dm::ModelDatabase modelDatabase_( "static constructor" );
static Kit::Dm::Mp::Uint32    mpElemCount_( modelDatabase_, "elemCount" );

////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "RingBufferMP" )
{
    ShutdownUnitTesting::clearAndUseCounter();
    uint32_t elemCount = 0;
    mpElemCount_.setInvalid();

    SECTION( "Block Operations#4" )
    {
        MyRingBuffer uut( mpElemCount_ );

        // Buffer empty
        uint64_t items[NUM_ELEMENTS] = { 10, 20, 30, 40, 50 };
        uint64_t peekItem            = 0;
        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.peekHead( peekItem ) == false );
        REQUIRE( uut.peekTail( peekItem ) == false );
        REQUIRE( mpElemCount_.read( elemCount ) == false );

        // Get the write index less than the read index
        REQUIRE( uut.add( items[0] ) == true );
        REQUIRE( mpElemCount_.read( elemCount ) == true );
        REQUIRE( elemCount == 1 );
        REQUIRE( uut.add( items[1] ) == true );
        REQUIRE( mpElemCount_.read( elemCount ) == true );
        REQUIRE( elemCount == 2 );
        REQUIRE( uut.add( items[2] ) == true );
        REQUIRE( mpElemCount_.read( elemCount ) == true );
        REQUIRE( elemCount == 3 );
        REQUIRE( uut.add( items[3] ) == true );
        REQUIRE( mpElemCount_.read( elemCount ) == true );
        REQUIRE( elemCount == 4 );
        REQUIRE( uut.add( items[4] ) == true );
        REQUIRE( mpElemCount_.read( elemCount ) == true );
        REQUIRE( elemCount == 5 );
        REQUIRE( uut.add( 111 ) == false );
        REQUIRE( mpElemCount_.read( elemCount ) == true );
        REQUIRE( elemCount == 5 );
        REQUIRE( uut.getNumItems() == 5 );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == true );

        // Block add: buffer full
        unsigned  numFlatItems = 11;
        uint64_t* dstPtr       = uut.peekNextAddItems( numFlatItems );
        REQUIRE( dstPtr == nullptr );
        REQUIRE( numFlatItems == 0 );
    }

    SECTION( "Block Operations#3" )
    {
        MyRingBuffer uut( mpElemCount_ );

        // Buffer empty
        uint64_t items[NUM_ELEMENTS] = { 10, 20, 30, 40, 50 };
        uint64_t item                = 0;
        uint64_t peekItem            = 0;
        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.peekHead( peekItem ) == false );
        REQUIRE( uut.peekTail( peekItem ) == false );

        // Get the write index less than the read index
        REQUIRE( uut.add( items[0] ) == true );
        REQUIRE( uut.add( items[1] ) == true );
        REQUIRE( uut.add( items[2] ) == true );
        REQUIRE( uut.add( items[3] ) == true );
        REQUIRE( uut.add( items[4] ) == true );
        REQUIRE( uut.add( 111 ) == false );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( item == 10 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( item == 20 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( item == 30 );
        REQUIRE( uut.getNumItems() == 2 );
        REQUIRE( uut.add( 111 ) == true );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 40 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 111 );
        REQUIRE( uut.getNumItems() == 3 );
        REQUIRE( mpElemCount_.read( elemCount ) == true );
        REQUIRE( elemCount == 3 );

        // Block Remove: Write idx:= 0, Read idx:= 3
        uint64_t  newItems[NUM_ELEMENTS] = { 0 };
        unsigned  numFlatItems           = 11;
        unsigned  NUM_ITEMS_XFER         = 3;
        uint64_t* srcPtr                 = uut.peekNextRemoveItems( numFlatItems );
        REQUIRE( srcPtr != nullptr );
        REQUIRE( numFlatItems == NUM_ITEMS_XFER );
        memcpy( newItems, srcPtr, NUM_ITEMS_XFER * sizeof( uint64_t ) );
        uut.removeElements( NUM_ITEMS_XFER );
        REQUIRE( mpElemCount_.read( elemCount ) == true );
        REQUIRE( elemCount == 0 );
        REQUIRE( uut.peekHead( peekItem ) == false );
        REQUIRE( uut.peekTail( peekItem ) == false );
        uint64_t expectedItems[NUM_ELEMENTS] = { 40, 50, 111 };
        REQUIRE( memcmp( newItems, expectedItems, sizeof( expectedItems ) ) == 0 );
    }

    SECTION( "Block Operations#3a" )
    {
        MyRingBuffer uut( mpElemCount_ );

        // Buffer empty
        uint64_t items[NUM_ELEMENTS] = { 10, 20, 30, 40, 50 };
        uint64_t item                = 0;
        uint64_t peekItem            = 0;
        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.peekHead( peekItem ) == false );
        REQUIRE( uut.peekTail( peekItem ) == false );
        REQUIRE( mpElemCount_.read( elemCount ) == false );

        // Get the write index less than the read index
        uint16_t seqNum     = 0;
        uint16_t readSeqNum = 0;
        REQUIRE( uut.add( items[0], seqNum ) == true );
        REQUIRE( seqNum > readSeqNum );
        REQUIRE( mpElemCount_.read( elemCount, &readSeqNum ) == true );
        REQUIRE( seqNum == readSeqNum );
        REQUIRE( elemCount == 1 );
        REQUIRE( uut.add( items[1], seqNum ) == true );
        REQUIRE( seqNum > readSeqNum );
        REQUIRE( mpElemCount_.read( elemCount, &readSeqNum ) == true );
        REQUIRE( seqNum == readSeqNum );
        REQUIRE( elemCount == 2 );
        REQUIRE( uut.add( items[2], seqNum ) == true );
        REQUIRE( seqNum > readSeqNum );
        REQUIRE( mpElemCount_.read( elemCount, &readSeqNum ) == true );
        REQUIRE( seqNum == readSeqNum );
        REQUIRE( elemCount == 3 );
        REQUIRE( uut.add( items[3], seqNum ) == true );
        REQUIRE( seqNum > readSeqNum );
        REQUIRE( mpElemCount_.read( elemCount, &readSeqNum ) == true );
        REQUIRE( seqNum == readSeqNum );
        REQUIRE( elemCount == 4 );
        REQUIRE( uut.add( items[4], seqNum ) == true );
        REQUIRE( seqNum > readSeqNum );
        REQUIRE( mpElemCount_.read( elemCount, &readSeqNum ) == true );
        REQUIRE( seqNum == readSeqNum );
        REQUIRE( elemCount == 5 );
        REQUIRE( uut.add( 111, seqNum ) == false );
        REQUIRE( seqNum == readSeqNum );
        REQUIRE( mpElemCount_.read( elemCount, &readSeqNum ) == true );
        REQUIRE( seqNum == readSeqNum );
        REQUIRE( elemCount == 5 );

        REQUIRE( uut.remove( item, seqNum ) == true );
        REQUIRE( item == 10 );
        REQUIRE( seqNum > readSeqNum );
        REQUIRE( mpElemCount_.read( elemCount, &readSeqNum ) == true );
        REQUIRE( seqNum == readSeqNum );
        REQUIRE( elemCount == 4 );
        REQUIRE( uut.remove( item, seqNum ) == true );
        REQUIRE( item == 20 );
        REQUIRE( seqNum > readSeqNum );
        REQUIRE( mpElemCount_.read( elemCount, &readSeqNum ) == true );
        REQUIRE( seqNum == readSeqNum );
        REQUIRE( elemCount == 3 );
        REQUIRE( uut.remove( item, seqNum ) == true );
        REQUIRE( item == 30 );
        REQUIRE( seqNum > readSeqNum );
        REQUIRE( mpElemCount_.read( elemCount, &readSeqNum ) == true );
        REQUIRE( seqNum == readSeqNum );
        REQUIRE( elemCount == 2 );
        REQUIRE( uut.getNumItems() == 2 );
        REQUIRE( uut.add( 111, seqNum ) == true );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 40 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 111 );
        REQUIRE( uut.getNumItems() == 3 );
        REQUIRE( mpElemCount_.read( elemCount ) == true );
        REQUIRE( elemCount == 3 );

        // Block Remove: Write idx:= 0, Read idx:= 3
        uint64_t  newItems[NUM_ELEMENTS] = { 0 };
        unsigned  numFlatItems           = 11;
        unsigned  NUM_ITEMS_XFER         = 3;
        uint64_t* srcPtr                 = uut.peekNextRemoveItems( numFlatItems );
        REQUIRE( srcPtr != nullptr );
        REQUIRE( numFlatItems == NUM_ITEMS_XFER );
        memcpy( newItems, srcPtr, NUM_ITEMS_XFER * sizeof( uint64_t ) );
        uut.removeElements( NUM_ITEMS_XFER, seqNum );
        REQUIRE( seqNum > readSeqNum );
        REQUIRE( mpElemCount_.read( elemCount, &readSeqNum ) == true );
        REQUIRE( seqNum == readSeqNum );
        REQUIRE( elemCount == 0 );
        REQUIRE( uut.peekHead( peekItem ) == false );
        REQUIRE( uut.peekTail( peekItem ) == false );
        uint64_t expectedItems[NUM_ELEMENTS] = { 40, 50, 111 };
        REQUIRE( memcmp( newItems, expectedItems, sizeof( expectedItems ) ) == 0 );
    }
    SECTION( "Block Operations#2" )
    {
        MyRingBuffer uut( mpElemCount_ );
        
        // Buffer empty
        uint64_t items[NUM_ELEMENTS] = { 10, 20, 30, 40, 50 };
        uint64_t item                = 0;
        uint64_t peekItem            = 0;
        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.peekHead( peekItem ) == false );
        REQUIRE( uut.peekTail( peekItem ) == false );

        // Get the write index less than the read index
        REQUIRE( uut.add( items[0] ) == true );
        REQUIRE( uut.add( items[1] ) == true );
        REQUIRE( uut.add( items[2] ) == true );
        REQUIRE( uut.add( items[3] ) == true );
        REQUIRE( uut.add( items[4] ) == true );
        REQUIRE( uut.add( 111 ) == false );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( item == 10 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( item == 20 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( item == 30 );
        REQUIRE( uut.getNumItems() == 2 );
        REQUIRE( uut.add( 111 ) == true );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 40 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 111 );
        REQUIRE( uut.getNumItems() == 3 );

        // Block Add: Write idx:= 0, Read idx:= 3
        uint64_t  newItems[NUM_ELEMENTS] = { 100, 200, 300, 400, 500 };
        unsigned  numFlatItems           = 11;
        unsigned  NUM_ITEMS_XFER         = 2;
        uint64_t* dstPtr                 = uut.peekNextAddItems( numFlatItems );
        REQUIRE( dstPtr != nullptr );
        REQUIRE( numFlatItems == NUM_ITEMS_XFER );
        memcpy( dstPtr, newItems, NUM_ITEMS_XFER * sizeof( uint64_t ) );
        uint16_t seqNum = 0;
        uint16_t readSeqNum = 0;
        uut.addElements( NUM_ITEMS_XFER, seqNum );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 40 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 200 );
        REQUIRE( seqNum > readSeqNum );
        REQUIRE( mpElemCount_.read( elemCount, &readSeqNum ) == true );
        REQUIRE( elemCount == 5 );
        REQUIRE( seqNum == readSeqNum );

    }

    SECTION( "Block Operations#1" )
    {
        MyRingBuffer uut( mpElemCount_ );

        // Buffer empty
        uint64_t item         = 0;
        uint64_t peekItem     = 0;
        unsigned numFlatItems = 11;
        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.peekHead( peekItem ) == false );
        REQUIRE( uut.peekTail( peekItem ) == false );
        REQUIRE( uut.peekNextAddItems( numFlatItems ) == uut.getMemoryPtr() );
        REQUIRE( numFlatItems == NUM_ELEMENTS );
        numFlatItems = 12;
        REQUIRE( uut.peekNextRemoveItems( numFlatItems ) == nullptr );
        REQUIRE( numFlatItems == 0 );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.remove( item ) == false );

        // Partial fill
        uint64_t  items[NUM_ELEMENTS] = { 10, 20, 30, 40, 50 };
        unsigned  NUM_ITEMS_XFER      = sizeof( items ) / sizeof( items[0] ) - 2;
        uint64_t* dstPtr              = uut.peekNextAddItems( numFlatItems );
        REQUIRE( dstPtr != nullptr );
        REQUIRE( numFlatItems == NUM_ELEMENTS );
        memcpy( dstPtr, items, NUM_ITEMS_XFER * sizeof( uint64_t ) );
        uut.addElements( NUM_ITEMS_XFER );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == items[NUM_ITEMS_XFER - 1] );

        // Partial fill (remaining linear space)
        dstPtr = uut.peekNextAddItems( numFlatItems );
        REQUIRE( dstPtr != nullptr );
        REQUIRE( numFlatItems == NUM_ELEMENTS - NUM_ITEMS_XFER );
        memcpy( dstPtr, items, ( NUM_ELEMENTS - NUM_ITEMS_XFER ) * sizeof( uint64_t ) );
        uut.addElements( NUM_ELEMENTS - NUM_ITEMS_XFER );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == items[NUM_ELEMENTS - NUM_ITEMS_XFER - 1] );

        // Remove all
        uint64_t  removedItems[NUM_ELEMENTS]  = {};
        uint64_t  expectedItems[NUM_ELEMENTS] = { 10, 20, 30, 10, 20 };
        uint64_t* srcPtr                      = uut.peekNextRemoveItems( numFlatItems );
        REQUIRE( srcPtr == uut.getMemoryPtr() );
        REQUIRE( numFlatItems == NUM_ELEMENTS );
        memcpy( removedItems, srcPtr, numFlatItems * sizeof( uint64_t ) );
        REQUIRE( memcmp( removedItems, expectedItems, sizeof( removedItems ) ) == 0 );
        uut.removeElements( numFlatItems );
        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.peekHead( peekItem ) == false );
        REQUIRE( uut.peekTail( peekItem ) == false );

        // Fill at end-of-linear space
        dstPtr           = uut.peekNextAddItems( numFlatItems );
        uint64_t srcItem = 0x1234567800080003;
        REQUIRE( dstPtr != nullptr );
        REQUIRE( numFlatItems == 1 );
        memcpy( dstPtr, &srcItem, sizeof( srcItem ) );
        uut.addElements( 1 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == srcItem );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == srcItem );

        // Remove at the end-of-linear space
        srcPtr = uut.peekNextRemoveItems( numFlatItems );
        REQUIRE( srcPtr == uut.getMemoryPtr() + NUM_ELEMENTS );
        REQUIRE( numFlatItems == 1 );
        uint64_t dstItem = 0;
        memcpy( &dstItem, srcPtr, sizeof( uint64_t ) );
        REQUIRE( dstItem == srcItem );
        uut.removeElements( numFlatItems );
        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.peekHead( peekItem ) == false );
        REQUIRE( uut.peekTail( peekItem ) == false );
    }

    SECTION( "Operations-int8" )
    {
        RingBufferMPAllocate<int8_t, NUM_ELEMENTS + 1> uut( mpElemCount_ );

        int8_t item     = 0;
        int8_t peekItem = 0;

        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.peekHead( peekItem ) == false );
        REQUIRE( uut.peekTail( peekItem ) == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 0 );
        REQUIRE( uut.remove( item ) == false );
        item = 10;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 1 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 10 );

        item = 20;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 2 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 20 );
        item = 30;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 3 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 30 );
        item = 40;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 4 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 40 );
        item = 50;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == true );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 50 );
        item = 60;
        REQUIRE( uut.add( item ) == false );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == true );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 50 );


        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 4 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 20 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 50 );
        REQUIRE( item == 10 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 3 );
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
        REQUIRE( uut.getNumItems() == 4 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 30 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 60 );
        item = 70;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == true );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 5 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 30 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 70 );


        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 4 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 40 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 70 );
        REQUIRE( item == 30 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 3 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 50 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 70 );
        REQUIRE( item == 40 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 2 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 60 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 70 );
        REQUIRE( item == 50 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 1 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 70 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 70 );
        REQUIRE( item == 60 );
        REQUIRE( uut.remove( item ) == true );
        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 0 );
        REQUIRE( uut.peekHead( peekItem ) == false );
        REQUIRE( uut.peekTail( peekItem ) == false );


        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.peekHead( peekItem ) == false );
        REQUIRE( uut.peekTail( peekItem ) == false );
        REQUIRE( uut.getMaxItems() == 5 );

        item = 10;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 1 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 10 );
        item = 20;
        REQUIRE( uut.add( item ) == true );
        REQUIRE( uut.isEmpty() == false );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 2 );
        REQUIRE( uut.peekHead( peekItem ) );
        REQUIRE( peekItem == 10 );
        REQUIRE( uut.peekTail( peekItem ) );
        REQUIRE( peekItem == 20 );

        uut.clearTheBuffer();
        REQUIRE( uut.isEmpty() == true );
        REQUIRE( uut.isFull() == false );
        REQUIRE( uut.peekHead( peekItem ) == false );
        REQUIRE( uut.peekTail( peekItem ) == false );
        REQUIRE( uut.getMaxItems() == 5 );
        REQUIRE( uut.getNumItems() == 0 );
    }


    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}