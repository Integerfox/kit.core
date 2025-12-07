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
#include "Kit/Text/BString.h"
#include <string.h>


///
using namespace Kit::Text;
using namespace Kit::System;


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "BString" )
{
    ShutdownUnitTesting::clearAndUseCounter();
    char     rawMemory0[11];
    char     rawMemory1[6];
    BString  foo( rawMemory0, sizeof( rawMemory0 ), "Hello World, this is bob" );
    BString  bar( rawMemory1, sizeof( rawMemory1 ), "Hello World, this is bob" );
    IString& bref = bar;


    SECTION( "Some basic operation" )
    {
        REQUIRE( foo != bar );
        REQUIRE( bar == bref );

        /* 0123456789 */
        foo.insertAt( 0, "HI" );
        REQUIRE( foo == "HIHello Wo" );
        foo.insertAt( -1, "HI" );
        REQUIRE( foo == "HIHIHello " );
        foo.insertAt( 9, "HI" );
        REQUIRE( foo == "HIHIHelloH" );
        foo.insertAt( 10, "!!" );
        REQUIRE( foo == "HIHIHelloH" );
        foo.insertAt( 5, "<*>" );
        REQUIRE( foo == "HIHIH<*>el" );
        foo = "123";
        foo.insertAt( 10, "Bob's here" );
        REQUIRE( foo == "123Bob's h" );
        foo = "456";
        foo.insertAt( 1, "Uncle" );
        REQUIRE( foo == "4Uncle56" );
    }

    SECTION( "Constructors" )
    {
        const char* nullPtr = 0;
        char        rawMemory2[4];
        BString     s0( rawMemory2, sizeof( rawMemory2 ), nullPtr );
        REQUIRE( s0 == "" );

        char    rawMemory3[4];
        BString s1( rawMemory3, sizeof( rawMemory3 ) );
        REQUIRE( s1 == "" );

        s1 = "uncle";
        char    rawMemory4[4];
        BString s2( rawMemory4, sizeof( rawMemory4 ), s1 );
        REQUIRE( s2 == "unc" );

        char    rawMemory5[4];
        BString s3( rawMemory5, sizeof( rawMemory5 ), bref );
        REQUIRE( s3 == "Hel" );

        char    rawMemory6[4];
        BString s4( rawMemory6, sizeof( rawMemory6 ), "abcdefghijklmnopqrstuvwxyz" );
        REQUIRE( s4 == "abc" );

        char    rawMemory7[4];
        BString s5( rawMemory7, sizeof( rawMemory7 ), '@' );
        REQUIRE( s5 == '@' );

        char    rawMemory8[4];
        BString s6( rawMemory8, sizeof( rawMemory8 ), -32 );
        REQUIRE( s6 == "-32" );

        char    rawMemory9[4];
        BString s7( rawMemory9, sizeof( rawMemory9 ), (unsigned)42 );
        REQUIRE( s7 == "42" );

        char    rawMemory10[4];
        BString s8( rawMemory10, sizeof( rawMemory10 ), (long)-10000000 );
        REQUIRE( s8 == "-10" );


        char    rawMemory11[4];
        BString s9( rawMemory11, sizeof( rawMemory11 ), (unsigned long)81000000 );
        REQUIRE( s9 == "810" );

        char    rawMemory12[4];
        BString s10( rawMemory12, sizeof( rawMemory12 ), (long long)-200000000000 );
        REQUIRE( s10 == "-20" );


        char    rawMemory13[4];
        BString s11( rawMemory13, sizeof( rawMemory13 ), (unsigned long long)9100000000000 );
        REQUIRE( s11 == "910" );

        char    rawMemory14[2];
        BString s12( rawMemory14, 0, "hello" );
        REQUIRE( s12 == "" );

        char    rawMemory15[6];
        BString s13( rawMemory15, 1, "hello" );
        REQUIRE( s13 == "" );

        BString s14( nullptr, 2, "hello" );
        REQUIRE( s14 == "" );
    }

    SECTION( "Assignments" )
    {
        const char* nullPtr = 0;
        char        rawMemory2[4];
        BString     s0( rawMemory2, sizeof( rawMemory2 ), "****" );
        s0 = nullPtr;
        REQUIRE( s0 == "" );

        char    rawMemory3[4];
        BString s1( rawMemory3, sizeof( rawMemory3 ), "****" );
        s1 = "";
        REQUIRE( s1 == "" );

        s1 = "uncle";
        char    rawMemory4[4];
        BString s2( rawMemory4, sizeof( rawMemory4 ), "****" );
        s2 = s1;
        REQUIRE( s2 == "unc" );

        char    rawMemory5[4];
        BString s3( rawMemory5, sizeof( rawMemory5 ), "****" );
        s3 = bref;
        REQUIRE( s3 == "Hel" );

        char    rawMemory6[4];
        BString s4( rawMemory6, sizeof( rawMemory6 ), "****" );
        s4 = "abcdefghijklmnopqrstuvwxyz";
        REQUIRE( s4 == "abc" );

        char    rawMemory7[4];
        BString s5( rawMemory7, sizeof( rawMemory7 ), "****" );
        s5 = '@';
        REQUIRE( s5 == '@' );

        char    rawMemory8[4];
        BString s6( rawMemory8, sizeof( rawMemory8 ), "****" );
        s6 = -32;
        REQUIRE( s6 == "-32" );

        char    rawMemory9[4];
        BString s7( rawMemory9, sizeof( rawMemory9 ), "****" );
        s7 = (unsigned)42;
        REQUIRE( s7 == "42" );

        char    rawMemory10[4];
        BString s8( rawMemory10, sizeof( rawMemory10 ), "****" );
        s8 = (long)-10000000;
        REQUIRE( s8 == "-10" );

        char    rawMemory11[4];
        BString s9( rawMemory11, sizeof( rawMemory11 ), "****" );
        s9 = (unsigned long)81000000;
        REQUIRE( s9 == "810" );
    }

    SECTION( "Append" )
    {
        const char* nullPtr = 0;
        char        rawMemory2[9];
        BString     s0( rawMemory2, sizeof( rawMemory2 ), "****" );
        s0 += nullPtr;
        REQUIRE( s0 == "****" );

        char    rawMemory3[9];
        BString s1( rawMemory3, sizeof( rawMemory3 ), "****" );
        s1 += "";
        REQUIRE( s1 == "****" );

        s1 = "uncle";
        char    rawMemory4[9];
        BString s2( rawMemory4, sizeof( rawMemory4 ), "****" );
        s2 += s1;
        REQUIRE( s2 == "****uncl" );

        char    rawMemory5[9];
        BString s3( rawMemory5, sizeof( rawMemory5 ), "****" );
        s3 += bref;
        REQUIRE( s3 == "****Hell" );

        char    rawMemory6[9];
        BString s4( rawMemory6, sizeof( rawMemory6 ), "****" );
        s4 += "abcdefghijklmnopqrstuvwxyz";
        REQUIRE( s4 == "****abcd" );

        char    rawMemory7[9];
        BString s5( rawMemory7, sizeof( rawMemory7 ), "****" );
        s5 += '@';
        REQUIRE( s5 == "****@" );

        char    rawMemory8[9];
        BString s6( rawMemory8, sizeof( rawMemory8 ), "****" );
        s6 += -32;
        REQUIRE( s6 == "****-32" );

        char    rawMemory9[9];
        BString s7( rawMemory9, sizeof( rawMemory9 ), "****" );
        s7 += (unsigned)42;
        REQUIRE( s7 == "****42" );

        char    rawMemory10[9];
        BString s8( rawMemory10, sizeof( rawMemory10 ), "****" );
        s8 += (long)-10000000;
        REQUIRE( s8 == "****-100" );

        char    rawMemory11[9];
        BString s9( rawMemory11, sizeof( rawMemory11 ), "****" );
        s9 += (unsigned long)81000000;
        REQUIRE( s9 == "****8100" );
    }

    REQUIRE( ShutdownUnitTesting::getAndClearCounter() == 0u );
}
