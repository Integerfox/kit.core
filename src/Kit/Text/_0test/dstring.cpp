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
#include "Kit/Text/DString.h"
#include "Kit/Memory/_testsupport/NewUnitTesting.h"

///
using namespace Kit::Text;
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "DString" )
{
    ShutdownUnitTesting::clearAndUseCounter();
    DString foo( "Hello World, this is bob" );
    DString bar( "Hello World, this is bob" );
    IString& bref = bar;


    SECTION( "Some basic operation" )
    {
        REQUIRE( foo == bar );
        REQUIRE( bar == bref );

        /* 0123456789 */
        foo.insertAt( 0, "HI" );
        REQUIRE( foo == "HIHello World, this is bob" );
        foo.insertAt( -1, "HI" );
        REQUIRE( foo == "HIHIHello World, this is bob" );
        foo.insertAt( 9, "HI" );
        REQUIRE( foo == "HIHIHelloHI World, this is bob" );
        foo.insertAt( 10, "!!" );
        REQUIRE( foo == "HIHIHelloH!!I World, this is bob" );
        foo.insertAt( 5, "<*>" );
        REQUIRE( foo == "HIHIH<*>elloH!!I World, this is bob" );
        foo = "123";
        foo.insertAt( 10, "Bob's here" );
        REQUIRE( foo == "123Bob's here" );
        foo = "456";
        foo.insertAt( 1, "Uncle" );
        REQUIRE( foo == "4Uncle56" );

        DString s( "123456789 123456" );
        REQUIRE( s == "123456789 123456" );
    }

    SECTION( "Constructors" )
    {
        const char* nullPtr = 0;
        DString s0( nullPtr );
        REQUIRE( s0 == "" );

        DString s1;
        REQUIRE( s1 == "" );

        s1 = "uncle";
        DString s2( s1 );
        REQUIRE( s2 == "uncle" );

        DString s3( bref );
        REQUIRE( s3 == "Hello World, this is bob" );

        DString s4( "abcdefghijklmnopqrstuvwxyz" );
        REQUIRE( s4 == "abcdefghijklmnopqrstuvwxyz" );

        DString s5( '@' );
        REQUIRE( s5 == '@' );

        DString s6( -32 );
        REQUIRE( s6 == "-32" );

        DString s7( (unsigned) 42 );
        REQUIRE( s7 == "42" );

        DString s8( (long) -10000000 );
        REQUIRE( s8 == "-10000000" );

        DString s9( (unsigned long) 81000000 );
        REQUIRE( s9 == "81000000" );

        DString s10( (long long) -200000000000 );
        REQUIRE( s10 == "-200000000000" );

        DString s11( ( unsigned long long )9100000000000 );
        REQUIRE( s11 == "9100000000000" );
    }

    SECTION( "Assignments" )
    {
        const char* nullPtr = 0;
        DString s0( "****" );
        s0 = nullPtr;
        REQUIRE( s0 == "" );

        DString s1( "****" );
        s1 = "";
        REQUIRE( s1 == "" );

        s1 = "uncle";
        DString s2( "****" );
        s2 = s1;
        REQUIRE( s2 == "uncle" );

        DString s3( "****" );
        s3 = bref;
        REQUIRE( s3 == "Hello World, this is bob" );

        DString s4( "****" );
        s4 = "abcdefghijklmnopqrstuvwxyz";
        REQUIRE( s4 == "abcdefghijklmnopqrstuvwxyz" );

        DString s5( "****" );
        s5 = '@';
        REQUIRE( s5 == '@' );

        DString s6( "****" );
        s6 = -32;
        REQUIRE( s6 == "-32" );

        DString s7( "****" );
        s7 = (unsigned) 42;
        REQUIRE( s7 == "42" );

        DString s8( "****" );
        s8 = (long) -10000000;
        REQUIRE( s8 == "-10000000" );

        DString s9( "****" );
        s9 = (unsigned long) 81000000;
        REQUIRE( s9 == "81000000" );

        DString s10( "****" );
        s10 = (long long) -200000000000;
        REQUIRE( s10 == "-200000000000" );

        DString s11( "****" );
        s11 = ( unsigned long long ) 9100000000000;
        REQUIRE( s11 == "9100000000000" );
    }

    SECTION( "Append" )
    {
        const char* nullPtr = 0;
        DString s0( "****" );
        s0 += nullPtr;
        REQUIRE( s0 == "****" );

        DString s1( "****" );
        s1 += "";
        REQUIRE( s1 == "****" );

        s1 = "uncle";
        DString s2( "****" );
        s2 += s1;
        REQUIRE( s2 == "****uncle" );

        DString s3( "****" );
        s3 += bref;
        REQUIRE( s3 == "****Hello World, this is bob" );

        DString s4( "****" );
        s4 += "abcdefghijklmnopqrstuvwxyz";
        REQUIRE( s4 == "****abcdefghijklmnopqrstuvwxyz" );

        DString s5( "****" );
        s5 += '@';
        REQUIRE( s5 == "****@" );

        DString s6( "****" );
        s6 += -32;
        REQUIRE( s6 == "****-32" );

        DString s7( "****" );
        s7 += (unsigned) 42;
        REQUIRE( s7 == "****42" );

        DString s8( "****" );
        s8 += (long) -10000000;
        REQUIRE( s8 == "****-10000000" );

        DString s9( "****" );
        s9 += (unsigned long) 81000000;
        REQUIRE( s9 == "****81000000" );

        DString s10( "****" );
        s10 += (long long) -200000000000;
        REQUIRE( s10 == "****-200000000000" );

        DString s11( "****" );
        s11 += ( unsigned long long ) 9100000000000;
        REQUIRE( s11 == "****9100000000000" );
    }

    SECTION( "Memory..." )
    {
        DString s1( "unclexx" );
        Kit::Memory::NewUnitTesting::disable();
        DString s2( s1 );
        DString s3( bref );
        DString s4( "abcdefghijklmnopqrstuvwxyz" );
        DString s5( '%' );
        DString s6( -32 );
        DString s7( (unsigned) 42 );
        DString s8( (long) -10000000 );
        DString s9( (unsigned long) 81000000 );
        DString s10( (long long) -81000000 );
        DString s11( ( unsigned long long )9100000000000 );
        Kit::Memory::NewUnitTesting::enable();
        REQUIRE( s1 == ( const char* )"unclexx" );
        REQUIRE( s2 == "" );
        REQUIRE( s2.length() == 0 );
        REQUIRE( s3 == "" );
        REQUIRE( s3.length() == 0 );
        REQUIRE( s4 == "" );
        REQUIRE( s4.length() == 0 );
        REQUIRE( s5 == "" );
        REQUIRE( s5.length() == 0 );
        REQUIRE( s6 == "" );
        REQUIRE( s6.length() == 0 );
        REQUIRE( s7 == "" );
        REQUIRE( s7.length() == 0 );
        REQUIRE( s8 == "" );
        REQUIRE( s8.length() == 0 );
        REQUIRE( s9 == "" );
        REQUIRE( s9.length() == 0 );
        REQUIRE( s10 == "" );
        REQUIRE( s10.length() == 0 );
        REQUIRE( s11 == "" );
        REQUIRE( s11.length() == 0 );


        DString s12( "Hi" );
        Kit::Memory::NewUnitTesting::disable();
        s12 += " World";
        s12+= "overflow the block size on the append operation!!!!!! add a few more characters just to max sure";
        Kit::Memory::NewUnitTesting::enable();
        REQUIRE( s12.length() == 15 );
        REQUIRE( s12 == "Hi Worldoverflo" );

        s12 = "bob";
        REQUIRE( s12 == "bob" );
        Kit::Memory::NewUnitTesting::disable();
        s12 = "overflow the block size on the append operation!!!!!! add a few more characters just to max sure";
        Kit::Memory::NewUnitTesting::enable();
        REQUIRE( s12.length() == 15 );
        REQUIRE( s12 == "overflow the bl" );

        Kit::Memory::NewUnitTesting::disable();
        s12.insertAt( 5, "[hello]" );
        Kit::Memory::NewUnitTesting::enable();
        REQUIRE( s12 == "overf[hello]low" );

        Kit::Memory::NewUnitTesting::disable();
        s2.insertAt( 5, "[hello]" );
        s3.insertAt( 1, "" );
        Kit::Memory::NewUnitTesting::enable();
        REQUIRE( s2 == "" );
        REQUIRE( s2.length() == 0 );
        REQUIRE( s2.maxLength() == 0 );
        REQUIRE( s3 == "" );
        REQUIRE( s3.length() == 0 );
        REQUIRE( s3.maxLength() == 0 );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
