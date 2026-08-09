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
#include "Kit/Text/Tokenizer/Basic.h"
#include "Kit/System/Trace.h"
#include "Kit/System/_testsupport/ShutdownUnitTesting.h"
#include <string.h>


///
using namespace Kit::Text::Tokenizer;


#define SECT_ "_0test"

////////////////////////////////////////////////////////////////////////////////


#define DELIMITERS_ ",.()"


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "basic" )
{
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    SECTION( "Whitespace#1..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "bob's  your \n uncle";
        Basic                  parser( rawString.getBuffer( dummy ) );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "bob's" );
        REQUIRE( parser.numTokens() == 1 );
        REQUIRE( parser.getToken( 1 ) == 0 );
        token = parser.getToken( 0 );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "bob's" );
        token = parser.remaining();
        KIT_SYSTEM_TRACE_MSG( SECT_, "  remaining=[%s]", token.getString() );
        REQUIRE( token == " your \n uncle" );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "your" );
        REQUIRE( parser.numTokens() == 2 );
        REQUIRE( parser.getToken( 2 ) == 0 );
        token = parser.getToken( 0 );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "bob's" );
        token = parser.getToken( 1 );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "your" );
        token = parser.remaining();
        KIT_SYSTEM_TRACE_MSG( SECT_, "  remaining=[%s]", token.getString() );
        REQUIRE( token == "\n uncle" );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "uncle" );
        REQUIRE( parser.numTokens() == 3 );
        REQUIRE( parser.getToken( 3 ) == 0 );
        token = parser.getToken( 0 );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "bob's" );
        token = parser.getToken( 1 );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "your" );
        token = parser.getToken( 2 );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "uncle" );
        token = parser.remaining();
        KIT_SYSTEM_TRACE_MSG( SECT_, "  remaining=[%s]", token.getString() );
        REQUIRE( token == "" );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
        REQUIRE( parser.numTokens() == 3 );
        REQUIRE( parser.getToken( 4 ) == 0 );
        token = parser.remaining();
        KIT_SYSTEM_TRACE_MSG( SECT_, "  remaining=[%s]", token.getString() );
        REQUIRE( token == "" );

        Basic parser2( nullptr );
        REQUIRE( parser2.next() == nullptr );
    }

    SECTION( "Whitespace#2..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = " \t ";
        Basic                  parser( rawString.getBuffer( dummy ) );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
    }

    SECTION( "Whitespace#3..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = " \t1 2 3    ";
        Basic                  parser( rawString.getBuffer( dummy ) );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "1" );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "2" );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "3" );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
    }

    SECTION( "Delimiter set#1..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "bob's,  (your). \n uncle";
        Basic                  parser( rawString.getBuffer( dummy ), DELIMITERS_ );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "bob's" );
        REQUIRE( parser.numTokens() == 1 );
        REQUIRE( parser.getToken( 1 ) == 0 );
        token = parser.getToken( 0 );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "bob's" );
        token = parser.remaining();
        KIT_SYSTEM_TRACE_MSG( SECT_, "  remaining=[%s]", token.getString() );
        REQUIRE( token == "  (your). \n uncle" );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "  " );
        REQUIRE( parser.numTokens() == 2 );
        REQUIRE( parser.getToken( 2 ) == 0 );
        token = parser.getToken( 0 );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "bob's" );
        token = parser.getToken( 1 );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "  " );
        token = parser.remaining();
        KIT_SYSTEM_TRACE_MSG( SECT_, "  remaining=[%s]", token.getString() );
        REQUIRE( token == "your). \n uncle" );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "your" );
        REQUIRE( parser.numTokens() == 3 );
        REQUIRE( parser.getToken( 3 ) == 0 );
        token = parser.getToken( 0 );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "bob's" );
        token = parser.getToken( 1 );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "  " );
        token = parser.getToken( 2 );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "your" );
        token = parser.remaining();
        KIT_SYSTEM_TRACE_MSG( SECT_, "  remaining=[%s]", token.getString() );
        REQUIRE( token == ". \n uncle" );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == " \n uncle" );
        REQUIRE( parser.numTokens() == 4 );
        REQUIRE( parser.getToken( 4 ) == 0 );
        token = parser.getToken( 0 );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "bob's" );
        token = parser.getToken( 1 );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "  " );
        token = parser.getToken( 2 );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "your" );
        token = parser.getToken( 3 );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == " \n uncle" );
        token = parser.remaining();
        KIT_SYSTEM_TRACE_MSG( SECT_, "  remaining=[%s]", token.getString() );
        REQUIRE( token == "" );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
        REQUIRE( parser.numTokens() == 4 );
        REQUIRE( parser.getToken( 4 ) == 0 );
        token = parser.remaining();
        KIT_SYSTEM_TRACE_MSG( SECT_, "  remaining=[%s]", token.getString() );
        REQUIRE( token == "" );
    }

    SECTION( "Delimiter set#2..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = DELIMITERS_ DELIMITERS_;
        Basic                                          parser( rawString.getBuffer( dummy ), DELIMITERS_ );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
    }

    SECTION( "Delimiter set#3..." )
    {
        int                    dummy;
        Kit::Text::FString<64> token;
        Kit::Text::FString<64> rawString = "((1,.2), 3..(),";
        Basic                  parser( rawString.getBuffer( dummy ), DELIMITERS_ );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "1" );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == "2" );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() == false );
        REQUIRE( token == " 3" );

        token = parser.next();
        KIT_SYSTEM_TRACE_MSG( SECT_, "token=[%s]", token.getString() );
        REQUIRE( token.isEmpty() );
    }

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
