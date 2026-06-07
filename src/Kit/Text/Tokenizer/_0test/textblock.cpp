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
#include "Kit/Text/FString.h"
#include "Kit/Text/Tokenizer/TextBlock.h"
#include "Kit/System/Trace.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include <string.h>


///
using namespace Kit::Text::Tokenizer;


#define SECT_ "_0test"

////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "textblock" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "Defaults#1..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = " arg1 ,arg2 , arg 3,, \"arg,5\\\\\\\"\" ";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ) );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 5 );
        REQUIRE( parser.isTerminated() == false );
        REQUIRE( parser.getParameter( 5 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "arg1" );
        token = parser.getParameter( 1 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "arg2" );
        token = parser.getParameter( 2 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "arg 3" );
        token = parser.getParameter( 3 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
        token = parser.getParameter( 4 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "arg,5\\\"" );

        TextBlock parser2( nullptr );
        REQUIRE( parser2.isValidTokens() == false );
    }

    SECTION( "Defaults#2..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "  ";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ) );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 0 );
        REQUIRE( parser.isTerminated() == false );
        REQUIRE( parser.getParameter( 5 ) == 0 );
        REQUIRE( parser.getParameter( 0 ) == 0 );
    }

    SECTION( "Defaults#3..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "  ; ";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ) );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 1 );
        REQUIRE( parser.isTerminated() == true );
        REQUIRE( parser.getParameter( 1 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
    }

    SECTION( "Defaults#4..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = " , , ";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ) );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 3 );
        REQUIRE( parser.isTerminated() == false );
        REQUIRE( parser.getParameter( 3 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
        token = parser.getParameter( 1 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
        token = parser.getParameter( 2 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
    }

    SECTION( "Defaults#4..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "a,";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ) );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 2 );
        REQUIRE( parser.isTerminated() == false );
        REQUIRE( parser.getParameter( 2 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "a" );
        token = parser.getParameter( 1 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
    }

    SECTION( "Defaults#5..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "  a  1  ,  a2,  a3,a  4 ,  a  5";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ) );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 5 );
        REQUIRE( parser.isTerminated() == false );
        REQUIRE( parser.getParameter( 5 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "a  1" );
        token = parser.getParameter( 1 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "a2" );
        token = parser.getParameter( 2 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "a3" );
        token = parser.getParameter( 3 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "a  4" );
        token = parser.getParameter( 4 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "a  5" );
        REQUIRE( *( parser.remaining() ) == '\0' );
    }

    SECTION( "Defaults#6..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = " ; ";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ) );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 1 );
        REQUIRE( parser.isTerminated() == true );
        REQUIRE( parser.getParameter( 1 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
    }

    SECTION( "Defaults#7..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "a , ; ";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ) );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 2 );
        REQUIRE( parser.isTerminated() == true );
        REQUIRE( parser.getParameter( 2 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "a" );
        token = parser.getParameter( 1 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
    }

    SECTION( "Defaults#8..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "a,b;bob's your uncle";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ) );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 2 );
        REQUIRE( parser.isTerminated() == true );
        REQUIRE( parser.getParameter( 2 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "a" );
        token = parser.getParameter( 1 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "b" );
        token = parser.remaining();
        KIT_SYSTEM_TRACE_MSG( SECT_, " remaining=[%s]", token.getString() );
        REQUIRE( token == "bob's your uncle" );
    }


    SECTION( "Defaults#9..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = ";bob's your uncle";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ) );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 1 );
        REQUIRE( parser.isTerminated() == true );
        REQUIRE( parser.getParameter( 1 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
        token = parser.remaining();
        KIT_SYSTEM_TRACE_MSG( SECT_, " remaining=[%s]", token.getString() );
        REQUIRE( token == "bob's your uncle" );
    }

    SECTION( "Alternate#1..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = " arg1 .  arg2 . arg 3.. $arg,5```$$";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ), '.', '!', '$', '`' );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 5 );
        REQUIRE( parser.isTerminated() == false );
        REQUIRE( parser.getParameter( 5 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "arg1" );
        token = parser.getParameter( 1 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "arg2" );
        token = parser.getParameter( 2 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "arg 3" );
        token = parser.getParameter( 3 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
        token = parser.getParameter( 4 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "arg,5`$" );
    }

    SECTION( "Alternate#2..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "  ! ";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ), '.', '!', '$', '`' );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 1 );
        REQUIRE( parser.isTerminated() == true );
        REQUIRE( parser.getParameter( 1 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
    }

    SECTION( "Alternate#3..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "a. $ hello $!";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ), '.', '!', '$', '`' );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 2 );
        REQUIRE( parser.isTerminated() == true );
        REQUIRE( parser.getParameter( 2 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "a" );
        token = parser.getParameter( 1 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == " hello " );
    }

    SECTION( "Alternate#4..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "a$ $  ! ";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ), '.', '!', '$', '`' );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 1 );
        REQUIRE( parser.isTerminated() == true );
        REQUIRE( parser.getParameter( 1 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "a " );
    }

    SECTION( "Alternate#5..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "$ $a.b.$   $ . d ";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ), '.', '!', '$', '`' );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 4 );
        REQUIRE( parser.isTerminated() == false );
        REQUIRE( parser.getParameter( 4 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == " a" );
        token = parser.getParameter( 1 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "b" );
        token = parser.getParameter( 2 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "   " );
        token = parser.getParameter( 3 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "d" );
    }

    SECTION( "Spaces#1..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = " arg1 arg2  arg 3 \"arg 4\\\\\\\"\" ";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ), ' ' );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 5 );
        REQUIRE( parser.isTerminated() == false );
        REQUIRE( parser.getParameter( 5 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "arg1" );
        token = parser.getParameter( 1 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "arg2" );
        token = parser.getParameter( 2 );
        KIT_SYSTEM_TRACE_MSG( SECT_,  " token=[%s]", token.getString() );
        REQUIRE( token == "arg" );
        token = parser.getParameter( 3 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "3" );
        token = parser.getParameter( 4 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "arg 4\\\"" );
    }

    SECTION( "Spaces#2..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "  ";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ), ' ' );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 0 );
        REQUIRE( parser.isTerminated() == false );
        REQUIRE( parser.getParameter( 5 ) == 0 );
        REQUIRE( parser.getParameter( 0 ) == 0 );
    }

    SECTION( "Spaces#3..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "  ; ";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ), ' ' );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 1 );
        REQUIRE( parser.isTerminated() == true );
        REQUIRE( parser.getParameter( 1 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
    }

    SECTION( "Spaces#4..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = " , , ";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ), ' ' );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 2 );
        REQUIRE( parser.isTerminated() == false );
        REQUIRE( parser.getParameter( 2 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "," );
        token = parser.getParameter( 1 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "," );
    }

    SECTION( "Spaces#4..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "a ";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ), ' ' );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 1 );
        REQUIRE( parser.isTerminated() == false );
        REQUIRE( parser.getParameter( 1 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "a" );
    }

    SECTION( "Spaces#5..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "  \"a  1\",  a2,  \"a3,a \" 4 ,  \"a  5\" ";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ), ' ' );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 6 );
        REQUIRE( parser.isTerminated() == false );
        REQUIRE( parser.getParameter( 6 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "a  1," );
        token = parser.getParameter( 1 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "a2," );
        token = parser.getParameter( 2 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "a3,a " );
        token = parser.getParameter( 3 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "4" );
        token = parser.getParameter( 4 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "," );
        token = parser.getParameter( 5 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "a  5" );
        REQUIRE( *( parser.remaining() ) == '\0' );
    }

    SECTION( "Spaces#6..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = " ; ";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ), ' ' );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 1 );
        REQUIRE( parser.isTerminated() == true );
        REQUIRE( parser.getParameter( 1 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
    }

    SECTION( "Spaces#7..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "a , ; ";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ), ' ' );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 2 );
        REQUIRE( parser.isTerminated() == true );
        REQUIRE( parser.getParameter( 2 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "a" );
        token = parser.getParameter( 1 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "," );
    }

    SECTION( "Spaces#8..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "a b;bob's your uncle";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ), ' ' );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 2 );
        REQUIRE( parser.isTerminated() == true );
        REQUIRE( parser.getParameter( 2 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "a" );
        token = parser.getParameter( 1 );
        KIT_SYSTEM_TRACE_MSG( SECT_, " token=[%s]", token.getString() );
        REQUIRE( token == "b" );
        token = parser.remaining();
        KIT_SYSTEM_TRACE_MSG( SECT_, " remaining=[%s]", token.getString() );
        REQUIRE( token == "bob's your uncle" );
    }


    SECTION( "Spaces#9..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = ";bob's your uncle";
        KIT_SYSTEM_TRACE_MSG( SECT_, "original=[%s]", rawString.getString() );
        TextBlock parser( rawString.getBuffer( dummy ), ' ' );

        REQUIRE( parser.isValidTokens() );
        REQUIRE( parser.numParameters() == 1 );
        REQUIRE( parser.isTerminated() == true );
        REQUIRE( parser.getParameter( 1 ) == 0 );
        token = parser.getParameter( 0 );
        KIT_SYSTEM_TRACE_MSG( SECT_,  " token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
        token = parser.remaining();
        KIT_SYSTEM_TRACE_MSG( SECT_, " remaining=[%s]", token.getString() );
        REQUIRE( token == "bob's your uncle" );
    }


    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
