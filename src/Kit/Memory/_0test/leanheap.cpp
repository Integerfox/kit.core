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
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include "Kit/Memory/LeanHeap.h"
#include "Kit/System/Trace.h"
#include <cstddef>

#define SECT_ "_0test"

///
using namespace Kit::Memory;

#define WORD_TYPE           uint16_t
#define WORD_TYPE2          std::max_align_t
#define ALIGNED_WORD_VALUE  alignof( uint16_t )
#define ALIGNED_WORD_VALUE2 alignof( std::max_align_t )
#define MAX_WORDS           10


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "LeanHeap" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    LeanHeap<MAX_WORDS, WORD_TYPE> uut;
    KIT_SYSTEM_TRACE_MSG( SECT_,
                          "HeapSize=%zu Bytes, Num Blocks=%u, Alignment=%zu",
                          MAX_WORDS * sizeof( WORD_TYPE ),
                          MAX_WORDS,
                          ALIGNED_WORD_VALUE );

    ContiguousAllocator::MemoryInfo_T memInfo;
    uut.getMemoryStart( memInfo );
    REQUIRE( memInfo.m_memoryStartPtr != nullptr );
    REQUIRE( memInfo.m_freeWords == memInfo.m_maxWords );
    KIT_SYSTEM_TRACE_MSG( SECT_,
                          "UUT: Heap Start Ptr=%p, Total Len=%zu Bytes, Max Words=%zu, Free Words=%zu",
                          memInfo.m_memoryStartPtr,
                          memInfo.m_totalLenInBytes,
                          memInfo.m_maxWords,
                          memInfo.m_freeWords );

    // Allocate...
    uint8_t* ptr    = (uint8_t*)uut.allocate( 1 );
    size_t   ptrInt = (size_t)ptr;
    REQUIRE( ptr != nullptr );
    REQUIRE( ( ptrInt % ALIGNED_WORD_VALUE ) == 0 );
    uut.getMemoryStart( memInfo );
    REQUIRE( memInfo.m_freeWords == memInfo.m_maxWords - 1 );

    uint8_t* ptr2 = static_cast<uint8_t*>( uut.allocate( 2 ) );
    REQUIRE( ptr2 != nullptr );
    ptrInt = (size_t)ptr2;
    REQUIRE( ( ptrInt % ALIGNED_WORD_VALUE ) == 0 );
    REQUIRE( ( ptr + 1 * sizeof( WORD_TYPE ) ) == ptr2 );
    uut.getMemoryStart( memInfo );
    REQUIRE( memInfo.m_freeWords == memInfo.m_maxWords - 2 );

    // Over allocate
    ptr = (uint8_t*)uut.allocate( ( MAX_WORDS * sizeof( WORD_TYPE ) ) );
    REQUIRE( ptr == nullptr );
    ptr = (uint8_t*)uut.allocate( ( MAX_WORDS - 1 ) * sizeof( WORD_TYPE ) );
    REQUIRE( ptr == nullptr );
    ptr = (uint8_t*)uut.allocate( ( MAX_WORDS - 2 ) * sizeof( WORD_TYPE ) );
    REQUIRE( ptr != nullptr );

    // Reset heap
    ptr = (uint8_t*)uut.allocate( sizeof( WORD_TYPE ) );
    REQUIRE( ptr == nullptr );
    uut.reset();
    uut.getMemoryStart( memInfo );
    REQUIRE( memInfo.m_freeWords == memInfo.m_maxWords );
    ptr = static_cast<uint8_t*>( uut.allocate( ( MAX_WORDS * sizeof( WORD_TYPE ) ) ) );
    REQUIRE( ptr != nullptr );


    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}

TEST_CASE( "LeanHeap2" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();
    LeanHeap<MAX_WORDS, WORD_TYPE2> uut;
    KIT_SYSTEM_TRACE_MSG( SECT_,
                          "HeapSize=%zu Bytes, Num Blocks=%u, Word Size=%zu, Alignment=%zu",
                          MAX_WORDS * sizeof( WORD_TYPE2 ),
                          MAX_WORDS,
                          sizeof( WORD_TYPE2 ),
                          ALIGNED_WORD_VALUE2 );

    ContiguousAllocator::MemoryInfo_T memInfo;
    uut.getMemoryStart( memInfo );
    REQUIRE( memInfo.m_memoryStartPtr != nullptr );
    REQUIRE( memInfo.m_freeWords == memInfo.m_maxWords );
    KIT_SYSTEM_TRACE_MSG( SECT_,
                          "UUT: Heap Start Ptr=%p, Total Len=%zu Bytes, Max Words=%zu, Free Words=%zu",
                          memInfo.m_memoryStartPtr,
                          memInfo.m_totalLenInBytes,
                          memInfo.m_maxWords,
                          memInfo.m_freeWords );

    // Allocate...
    uint8_t* ptr    = (uint8_t*)uut.allocate( 1 * sizeof( WORD_TYPE2 ) );
    size_t   ptrInt = (size_t)ptr;
    REQUIRE( ptr != nullptr );
    REQUIRE( ( ptrInt % ALIGNED_WORD_VALUE2 ) == 0 );
    uut.getMemoryStart( memInfo );
    KIT_SYSTEM_TRACE_MSG( SECT_,
                          "UUT: Heap Start Ptr=%p, Total Len=%zu Bytes, Max Words=%zu, Free Words=%zu",
                          memInfo.m_memoryStartPtr,
                          memInfo.m_totalLenInBytes,
                          memInfo.m_maxWords,
                          memInfo.m_freeWords );
    REQUIRE( memInfo.m_freeWords == memInfo.m_maxWords - 1 );

    uint8_t* ptr2 = static_cast<uint8_t*>( uut.allocate( 2 * sizeof( WORD_TYPE2 ) ) );
    REQUIRE( ptr2 != nullptr );
    ptrInt = (size_t)ptr2;
    REQUIRE( ( ptrInt % ALIGNED_WORD_VALUE2 ) == 0 );
    REQUIRE( ( ptr + sizeof( WORD_TYPE2 ) ) == ptr2 );
    uut.getMemoryStart( memInfo );
    KIT_SYSTEM_TRACE_MSG( SECT_,
                          "UUT: Heap Start Ptr=%p, Total Len=%zu Bytes, Max Words=%zu, Free Words=%zu",
                          memInfo.m_memoryStartPtr,
                          memInfo.m_totalLenInBytes,
                          memInfo.m_maxWords,
                          memInfo.m_freeWords );
    REQUIRE( memInfo.m_freeWords == memInfo.m_maxWords - 3 );

    // Over allocate
    ptr = (uint8_t*)uut.allocate( ( MAX_WORDS * sizeof( WORD_TYPE2 ) ) );
    REQUIRE( ptr == nullptr );
    ptr = (uint8_t*)uut.allocate( ( MAX_WORDS - 1 ) * sizeof( WORD_TYPE2 ) );
    REQUIRE( ptr == nullptr );
    ptr = (uint8_t*)uut.allocate( ( MAX_WORDS - 3 ) * sizeof( WORD_TYPE2 ) );
    REQUIRE( ptr != nullptr );

    // Reset heap
    ptr = (uint8_t*)uut.allocate( sizeof( WORD_TYPE2 ) );
    REQUIRE( ptr == nullptr );
    uut.reset();
    uut.getMemoryStart( memInfo );
    KIT_SYSTEM_TRACE_MSG( SECT_,
                          "UUT: Heap Start Ptr=%p, Total Len=%zu Bytes, Max Words=%zu, Free Words=%zu",
                          memInfo.m_memoryStartPtr,
                          memInfo.m_totalLenInBytes,
                          memInfo.m_maxWords,
                          memInfo.m_freeWords );
    REQUIRE( memInfo.m_freeWords == memInfo.m_maxWords );
    ptr = static_cast<uint8_t*>( uut.allocate( ( MAX_WORDS * sizeof( WORD_TYPE2 ) ) ) );
    REQUIRE( ptr != nullptr );


    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}