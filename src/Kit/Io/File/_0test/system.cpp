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
#include "Kit/Type/TraverserStatus.h"
#include "catch2/catch_test_macros.hpp"
#include "Kit/System/Trace.h"
#include "Kit/Io/File/System.h"
#include "Kit/Text/FString.h"


#define SECT_ "_0test"


/** Hack to get around the fact the SCM systems will convert newline
    characters in Text Files based on the target host, i.e. on Windows my
    newline character will be 2 chars, whereas on posix/linux it will be
    a single character -->hence delta in the file length.
 */
#define WIN32_TESTINPUT_TXT_FILE_LENGTH   106
#define WIN32_TESTINPUT_TEXT_HELLO_OFFEST 0x5D
#define POSIX_TESTINPUT_TXT_FILE_LENGTH   101
#define POSIX_TESTINPUT_TEXT_HELLO_OFFEST 0x58

///
using namespace Kit::Io::File;

#define TEST_PATH1     "A:/hello/world.txt"
#define TEST_PATH2     "world.txt"
#define TEST_PATH3     "world"
#define TEST_PATH4     ".txt"
#define TEST_PATH5     "A:/hello/world.txt/foo"
#define TEST_PATH6     "A:/hello/world.txt/foo/bar/just.kidding"
#define TEST_PATH7     "/hello"
#define TEST_PATH8     "/hello/foo"
#define TEST_PATH9     "A:hello."

#define TEST_DOS_PATH1 "A:\\hello\\world.txt"
#define TEST_DOS_PATH2 "world.txt"
#define TEST_DOS_PATH3 "world"
#define TEST_DOS_PATH4 ".txt"
#define TEST_DOS_PATH5 "A:\\hello\\world.txt\\foo"
#define TEST_DOS_PATH6 "A:\\hello\\world.txt\\foo\\bar\\just.kidding"
#define TEST_DOS_PATH7 "\\hello"
#define TEST_DOS_PATH8 "\\hello\\foo"
#define TEST_DOS_PATH9 "A:hello."


////////////////////////////////////////////////////////////////////////////////
namespace {

class Walker : public System::DirectoryWalker
{
public:
    int        m_depth;
    bool       m_files;
    bool       m_dirs;
    NameString m_workName;
    bool       m_contentCheck;

public:
    ///
    Walker( int depth, bool files = true, bool dirs = true )
        : m_depth( depth ), m_files( files ), m_dirs( dirs ), m_contentCheck( true )
    {
    }

public:
    Kit::Type::TraverserStatus item( const char* currentParent, const char* fsEntryName, System::Info_T& entryInfo )
    {
        // File check
        m_workName = fsEntryName;
        if ( entryInfo.m_isFile )
        {
            if ( !m_files )
            {
                m_contentCheck = false;
            }

            else if ( m_depth > 2 )
            {
                if ( m_workName != "d1.txt" && m_workName != "d2.txt" && m_workName != "d3.txt" )
                {
                    m_contentCheck = false;
                }
            }
            else if ( m_depth > 1 )
            {
                if ( m_workName != "d1.txt" && m_workName != "d2.txt" )
                {
                    m_contentCheck = false;
                }
            }
            else if ( m_workName != "d1.txt" )
            {
                m_contentCheck = false;
            }
        }

        // Dir check
        m_workName = fsEntryName;
        if ( entryInfo.m_isDir )
        {
            if ( !m_dirs )
            {
                m_contentCheck = false;
            }

            else if ( m_depth >= 2 )
            {
                if ( m_workName != "d2" && m_workName != "d3" )
                {
                    m_contentCheck = false;
                }
            }
            else if ( m_depth >= 1 )
            {
                if ( m_workName != "d2" )
                {
                    m_contentCheck = false;
                }
            }
            else
            {
                m_contentCheck = false;
            }
        }

        KIT_SYSTEM_TRACE_MSG( SECT_, "%s%s Parent=%s, item=%s.  (content=%d)", entryInfo.m_isFile ? "f" : "-", entryInfo.m_isDir ? "d" : "-", currentParent, fsEntryName, m_contentCheck );
        return Kit::Type::TraverserStatus::eCONTINUE;
    }
};

};  // end anonymous namespace


////////////////////////////////////////////////////////////////////////////////
TEST_CASE( "api" )
{
    KIT_SYSTEM_TRACE_FUNC( SECT_ );
    Kit::System::ShutdownUnitTesting::clearAndUseCounter();

    NameString name;
    NameString name2;

    ///
    KIT_SYSTEM_TRACE_MSG( SECT_, ( "Walk directories..." ) );
    REQUIRE( System::createDirectory( "d1" ) );
    REQUIRE( System::createFile( "d1/d1.txt" ) );
    REQUIRE( System::createDirectory( "d1/d2" ) );
    REQUIRE( System::createFile( "d1/d2/d2.txt" ) );
    REQUIRE( System::createDirectory( "d1/d2/d3" ) );
    REQUIRE( System::createFile( "d1/d2/d3/d3.txt" ) );
    {
        Walker iterator( 3 );
        KIT_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 100" ) );
        REQUIRE( System::walkDirectory( "d1", iterator, 100 ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator( 3, true, false );
        KIT_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 100" ) );
        REQUIRE( System::walkDirectory( "d1", iterator, 100, true, false ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator( 3, false, true );
        KIT_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 100" ) );
        REQUIRE( System::walkDirectory( "d1", iterator, 100, false, true ) );
        REQUIRE( iterator.m_contentCheck );
    }

    {
        Walker iterator( 2 );
        KIT_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 2" ) );
        REQUIRE( System::walkDirectory( "d1", iterator, 2 ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator( 2, true, false );
        KIT_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 2" ) );
        REQUIRE( System::walkDirectory( "d1", iterator, 2, true, false ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator( 2, false, true );
        KIT_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 2" ) );
        REQUIRE( System::walkDirectory( "d1", iterator, 2, false, true ) );
        REQUIRE( iterator.m_contentCheck );
    }

    {
        Walker iterator( 1 );
        KIT_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 1" ) );
        REQUIRE( System::walkDirectory( "d1", iterator, 1 ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator( 1, true, false );
        KIT_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 12" ) );
        REQUIRE( System::walkDirectory( "d1", iterator, 1, true, false ) );
        REQUIRE( iterator.m_contentCheck );
    }
    {
        Walker iterator( 1, false, true );
        KIT_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 1" ) );
        REQUIRE( System::walkDirectory( "d1", iterator, 1, false, true ) );
        REQUIRE( iterator.m_contentCheck );
    }

    REQUIRE( System::createDirectory( "d1/d2/d3/d4" ) );
    REQUIRE( System::createFile( "d1/d2/d3/d3a.txt" ) );
    {
        Walker iterator( 3, true, false );
        KIT_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 100" ) );
        REQUIRE( System::walkDirectory( "d1", iterator, 100, true, false ) );
        REQUIRE( iterator.m_contentCheck == false );
    }
    {
        Walker iterator( 3, false, true );
        KIT_SYSTEM_TRACE_MSG( SECT_, ( "Walk 'd1', 100" ) );
        REQUIRE( System::walkDirectory( "d1", iterator, 100, false, true ) );
        REQUIRE( iterator.m_contentCheck == false );
    }

    REQUIRE( System::remove( "d1/d2/d3/d4" ) );
    REQUIRE( System::remove( "d1/d2/d3/d3.txt" ) );
    REQUIRE( System::remove( "d1/d2/d3/d3a.txt" ) );
    REQUIRE( System::remove( "d1/d2/d3" ) );
    REQUIRE( System::remove( "d1/d2/d2.txt" ) );
    REQUIRE( System::remove( "d1/d2" ) );
    REQUIRE( System::remove( "d1/d1.txt" ) );
    REQUIRE( System::remove( "d1" ) );


    ///
    KIT_SYSTEM_TRACE_MSG( SECT_, ( "Copy, Appended..." ) );
    REQUIRE( System::exists( "copy.txt" ) == false );
    REQUIRE( System::exists( "copy2.txt" ) == false );
    REQUIRE( System::copyFile( "testinput.txt", "copy.txt" ) );
    REQUIRE( System::isFile( "copy.txt" ) );
    REQUIRE( ( System::size( "copy.txt" ) == WIN32_TESTINPUT_TXT_FILE_LENGTH || System::size( "copy.txt" ) == POSIX_TESTINPUT_TXT_FILE_LENGTH ) );
    REQUIRE( System::appendFile( "testinput.txt", "copy2.txt" ) );
    REQUIRE( ( System::size( "copy2.txt" ) == WIN32_TESTINPUT_TXT_FILE_LENGTH || System::size( "copy2.txt" ) == POSIX_TESTINPUT_TXT_FILE_LENGTH ) );
    REQUIRE( System::appendFile( "testinput.txt", "copy.txt" ) );
    REQUIRE( ( System::size( "copy.txt" ) == 2 * WIN32_TESTINPUT_TXT_FILE_LENGTH || System::size( "copy.txt" ) == 2 * POSIX_TESTINPUT_TXT_FILE_LENGTH ) );
    REQUIRE( System::remove( "copy.txt" ) );
    REQUIRE( System::isFile( "copy.txt" ) == false );
    REQUIRE( System::remove( "copy2.txt" ) );
    REQUIRE( System::exists( "copy2.txt" ) == false );


    ///
    KIT_SYSTEM_TRACE_MSG( SECT_, ( "Create, move, remove, etc. ..." ) );
    REQUIRE( System::exists( "bob" ) == false );
    REQUIRE( System::createDirectory( "bob" ) );
    REQUIRE( System::exists( "bob" ) );
    REQUIRE( System::isDirectory( "bob" ) );
    REQUIRE( System::exists( "uncle.txt" ) == false );
    REQUIRE( System::createFile( "bob/uncle.txt" ) );
    REQUIRE( System::exists( "bob/uncle.txt" ) );
    REQUIRE( System::isFile( "bob/uncle.txt" ) );

    REQUIRE( System::exists( "charlie" ) == false );
    REQUIRE( System::renameInPlace( "bob", "charlie" ) );
    REQUIRE( System::isDirectory( "charlie" ) );
    REQUIRE( System::isFile( "charlie/uncle.txt" ) );

    REQUIRE( System::exists( "uncle.txt" ) == false );
    REQUIRE( System::moveFile( "charlie/uncle.txt", "uncle.txt" ) );
    REQUIRE( System::isFile( "uncle.txt" ) );
    REQUIRE( System::renameInPlace( "uncle.txt", "your.txt" ) );
    REQUIRE( System::isFile( "your.txt" ) );

    REQUIRE( System::createFile( "charlie/test.txt" ) );
    REQUIRE( System::exists( "charlie/test.txt" ) );
    REQUIRE( System::remove( "charlie" ) == false );
    REQUIRE( System::remove( "charlie/test.txt" ) );
    REQUIRE( System::remove( "charlie" ) );
    REQUIRE( System::exists( "charlie" ) == false );
    REQUIRE( System::remove( "your.txt" ) );
    REQUIRE( System::exists( "your.txt" ) == false );


    ///
    KIT_SYSTEM_TRACE_MSG( SECT_, ( "Info..." ) );
    REQUIRE( System::exists( ".." ) );
    REQUIRE( System::exists( "testinput.txt" ) );
    REQUIRE( System::exists( "nothere.txt" ) == false );
    REQUIRE( System::isFile( "." ) == false );
    REQUIRE( System::isFile( "testinput.txt" ) );
    REQUIRE( System::isDirectory( "." ) == true );
    REQUIRE( System::isDirectory( ".." ) == true );
    REQUIRE( System::isDirectory( "testinput.txt" ) == false );
    REQUIRE( System::isReadable( "." ) );
    REQUIRE( System::isReadable( ".." ) );
    REQUIRE( System::isReadable( "testinput.txt" ) );
    REQUIRE( System::isReadable( "testinput.txte" ) == false );
    REQUIRE( System::isWriteable( "." ) );
    REQUIRE( System::isWriteable( ".." ) );
    REQUIRE( System::isWriteable( "testinput.txt" ) );
    REQUIRE( System::isWriteable( "testinput.txtdd" ) == false );
    REQUIRE( ( System::size( "testinput.txt" ) == WIN32_TESTINPUT_TXT_FILE_LENGTH || System::size( "testinput.txt" ) == POSIX_TESTINPUT_TXT_FILE_LENGTH ) );
    time_t t1, t2, t3;
    t1   = System::timeModified( "." );
    t2   = System::timeModified( ".." );
    t3   = System::timeModified( "testinput.txt" );
    name = ctime( &t2 );
    KIT_SYSTEM_TRACE_MSG( SECT_, "'..' mtime           := %s", name.getString() );
    name = ctime( &t1 );
    KIT_SYSTEM_TRACE_MSG( SECT_, "'.' mtime            := %s", name.getString() );
    name = ctime( &t3 );
    KIT_SYSTEM_TRACE_MSG( SECT_, "'testinput.txt' mtime:= %s", name.getString() );
    REQUIRE( t1 != ( (time_t)-1 ) );
    REQUIRE( t2 != ( (time_t)-1 ) );
    REQUIRE( t3 != ( (time_t)-1 ) );


    ///
    KIT_SYSTEM_TRACE_MSG( SECT_, ( "Split..." ) );
    System::splitDrive( TEST_PATH1, name );
    REQUIRE( name == "A" );
    System::splitDrive( TEST_PATH2, name );
    REQUIRE( name == "" );
    System::splitDrive( TEST_PATH3, name );
    REQUIRE( name == "" );
    System::splitDrive( TEST_PATH4, name );
    REQUIRE( name == "" );
    System::splitDrive( TEST_PATH5, name );
    REQUIRE( name == "A" );
    System::splitDrive( TEST_PATH6, name );
    REQUIRE( name == "A" );
    System::splitDrive( TEST_PATH7, name );
    REQUIRE( name == "" );
    System::splitDrive( TEST_PATH8, name );
    REQUIRE( name == "" );
    System::splitDrive( TEST_PATH9, name );
    REQUIRE( name == "A" );

    System::splitFullpath( TEST_PATH1, name );
    REQUIRE( name == "A:/hello/" );
    System::splitFullpath( TEST_PATH2, name );
    REQUIRE( name == "" );
    System::splitFullpath( TEST_PATH3, name );
    REQUIRE( name == "" );
    System::splitFullpath( TEST_PATH4, name );
    REQUIRE( name == "" );
    System::splitFullpath( TEST_PATH5, name );
    REQUIRE( name == "A:/hello/world.txt/" );
    System::splitFullpath( TEST_PATH6, name );
    REQUIRE( name == "A:/hello/world.txt/foo/bar/" );
    System::splitFullpath( TEST_PATH7, name );
    REQUIRE( name == "/" );
    System::splitFullpath( TEST_PATH8, name );
    REQUIRE( name == "/hello/" );
    System::splitFullpath( TEST_PATH9, name );
    REQUIRE( name == "A:" );

    System::splitPath( TEST_PATH1, name );
    REQUIRE( name == "/hello/" );
    System::splitPath( TEST_PATH2, name );
    REQUIRE( name == "" );
    System::splitPath( TEST_PATH3, name );
    REQUIRE( name == "" );
    System::splitPath( TEST_PATH4, name );
    REQUIRE( name == "" );
    System::splitPath( TEST_PATH5, name );
    REQUIRE( name == "/hello/world.txt/" );
    System::splitPath( TEST_PATH6, name );
    REQUIRE( name == "/hello/world.txt/foo/bar/" );
    System::splitPath( TEST_PATH7, name );
    REQUIRE( name == "/" );
    System::splitPath( TEST_PATH8, name );
    REQUIRE( name == "/hello/" );
    System::splitPath( TEST_PATH9, name );
    REQUIRE( name == "" );

    System::splitFullname( TEST_PATH1, name );
    REQUIRE( name == "world.txt" );
    System::splitFullname( TEST_PATH2, name );
    REQUIRE( name == "world.txt" );
    System::splitFullname( TEST_PATH3, name );
    REQUIRE( name == "world" );
    System::splitFullname( TEST_PATH4, name );
    REQUIRE( name == ".txt" );
    System::splitFullname( TEST_PATH5, name );
    REQUIRE( name == "foo" );
    System::splitFullname( TEST_PATH6, name );
    REQUIRE( name == "just.kidding" );
    System::splitFullname( TEST_PATH7, name );
    REQUIRE( name == "hello" );
    System::splitFullname( TEST_PATH8, name );
    REQUIRE( name == "foo" );
    System::splitFullname( TEST_PATH9, name );
    REQUIRE( name == "hello." );

    System::splitName( TEST_PATH1, name );
    REQUIRE( name == "world" );
    System::splitName( TEST_PATH2, name );
    REQUIRE( name == "world" );
    System::splitName( TEST_PATH3, name );
    REQUIRE( name == "world" );
    System::splitName( TEST_PATH4, name );
    REQUIRE( name == "" );
    System::splitName( TEST_PATH5, name );
    REQUIRE( name == "foo" );
    System::splitName( TEST_PATH6, name );
    REQUIRE( name == "just" );
    System::splitName( TEST_PATH7, name );
    REQUIRE( name == "hello" );
    System::splitName( TEST_PATH8, name );
    REQUIRE( name == "foo" );
    System::splitName( TEST_PATH9, name );
    REQUIRE( name == "hello" );

    System::splitExtension( TEST_PATH1, name );
    REQUIRE( name == "txt" );
    System::splitExtension( TEST_PATH2, name );
    REQUIRE( name == "txt" );
    System::splitExtension( TEST_PATH3, name );
    REQUIRE( name == "" );
    System::splitExtension( TEST_PATH4, name );
    REQUIRE( name == "txt" );
    System::splitExtension( TEST_PATH5, name );
    REQUIRE( name == "" );
    System::splitExtension( TEST_PATH6, name );
    REQUIRE( name == "kidding" );
    System::splitExtension( TEST_PATH7, name );
    REQUIRE( name == "" );
    System::splitExtension( TEST_PATH8, name );
    REQUIRE( name == "" );
    System::splitExtension( TEST_PATH9, name );
    REQUIRE( name == "" );

    System::splitFullpathFullName( TEST_PATH1, name, name2 );
    REQUIRE( name == "A:/hello/" );
    REQUIRE( name2 == "world.txt" );
    System::splitFullpathFullName( TEST_PATH2, name, name2 );
    REQUIRE( name == "" );
    REQUIRE( name2 == "world.txt" );
    System::splitFullpathFullName( TEST_PATH3, name, name2 );
    REQUIRE( name == "" );
    REQUIRE( name2 == "world" );
    System::splitFullpathFullName( TEST_PATH4, name, name2 );
    REQUIRE( name == "" );
    REQUIRE( name2 == ".txt" );
    System::splitFullpathFullName( TEST_PATH5, name, name2 );
    REQUIRE( name == "A:/hello/world.txt/" );
    REQUIRE( name2 == "foo" );
    System::splitFullpathFullName( TEST_PATH6, name, name2 );
    REQUIRE( name == "A:/hello/world.txt/foo/bar/" );
    REQUIRE( name2 == "just.kidding" );
    System::splitFullpathFullName( TEST_PATH7, name, name2 );
    REQUIRE( name == "/" );
    REQUIRE( name2 == "hello" );
    System::splitFullpathFullName( TEST_PATH8, name, name2 );
    REQUIRE( name == "/hello/" );
    REQUIRE( name2 == "foo" );
    System::splitFullpathFullName( TEST_PATH9, name, name2 );
    REQUIRE( name == "A:" );
    REQUIRE( name2 == "hello." );


    name = System::getStandard( TEST_DOS_PATH1 );
    KIT_SYSTEM_TRACE_MSG( SECT_, "PATH=[%s], getStandard=[%s]", TEST_DOS_PATH1, name.getString() );
    REQUIRE( name == TEST_PATH1 );
    name = System::getStandard( TEST_DOS_PATH2 );
    REQUIRE( name == TEST_PATH2 );
    name = System::getStandard( TEST_DOS_PATH3 );
    REQUIRE( name == TEST_PATH3 );
    name = System::getStandard( TEST_DOS_PATH4 );
    REQUIRE( name == TEST_PATH4 );
    name = System::getStandard( TEST_DOS_PATH5 );
    REQUIRE( name == TEST_PATH5 );
    name = System::getStandard( TEST_DOS_PATH6 );
    KIT_SYSTEM_TRACE_MSG( SECT_, "PATH=[%s], getStandard=[%s]", TEST_DOS_PATH6, name.getString() );
    REQUIRE( name == TEST_PATH6 );
    name = System::getStandard( TEST_DOS_PATH7 );
    REQUIRE( name == TEST_PATH7 );
    name = System::getStandard( TEST_DOS_PATH8 );
    REQUIRE( name == TEST_PATH8 );
    name = System::getStandard( TEST_DOS_PATH9 );
    REQUIRE( name == TEST_PATH9 );


    name = System::getStandard( TEST_PATH1 );
    KIT_SYSTEM_TRACE_MSG( SECT_, "PATH=[%s], getStandard=[%s]", TEST_PATH1, name.getString() );
    REQUIRE( name == TEST_PATH1 );
    name = System::getStandard( TEST_PATH2 );
    REQUIRE( name == TEST_PATH2 );
    name = System::getStandard( TEST_PATH3 );
    REQUIRE( name == TEST_PATH3 );
    name = System::getStandard( TEST_PATH4 );
    REQUIRE( name == TEST_PATH4 );
    name = System::getStandard( TEST_PATH5 );
    REQUIRE( name == TEST_PATH5 );
    name = System::getStandard( TEST_PATH6 );
    KIT_SYSTEM_TRACE_MSG( SECT_, "PATH=[%s], getStandard=[%s]", TEST_PATH6, name.getString() );
    REQUIRE( name == TEST_PATH6 );
    name = System::getStandard( TEST_PATH7 );
    REQUIRE( name == TEST_PATH7 );
    name = System::getStandard( TEST_PATH8 );
    REQUIRE( name == TEST_PATH8 );
    name = System::getStandard( TEST_PATH9 );
    REQUIRE( name == TEST_PATH9 );

    REQUIRE( Kit::System::ShutdownUnitTesting::getAndClearCounter() == 0u );
}
