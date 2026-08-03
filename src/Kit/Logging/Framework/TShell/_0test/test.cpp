/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "test.h"
#include "Kit/TShell/Processor.h"
#include "Kit/TShell/NoSecurity.h"
#include "Kit/System/Api.h"
#include "Kit/System/Private.h"
#include "Kit/TShell/Command/Bye.h"
#include "Kit/TShell/Command/Help.h"
#include "Kit/TShell/Command/Echo.h"
#include "Kit/TShell/Command/Trace.h"
#include "Kit/TShell/Command/Wait.h"
#include "Kit/TShell/StdioThread.h"
#include "Kit/Logging/Framework/JournalLogSink.h"
#include "Kit/Dm/TShell/Read.h"
#include "Kit/Dm/TShell/Write.h"
#include "Kit/Dm/ModelDatabase.h"
#include "Kit/Logging/Framework/TShell/Viewer.h"
#include "Kit/Logging/Framework/IApplication.h"
#include "Kit/Logging/Framework/EntryData.h"
#include "Kit/Persistence/Record/Journal/Server.h"
#include "Kit/Persistence/Record/Journal/EntryRecord.h"
#include "Kit/Persistence/Record/Journal/HeadRecord.h"
#include "Kit/Persistence/Record/Chunk/Crc.h"
#include "Kit/Persistence/Record/Media/FileAdapter.h"
#include "Kit/Checksum/Crc16CcittFast.h"
#include "Kit/EventQueue/Server.h"
#include "Kit/Container/RingBufferMP.h"
#include "Kit/Dm/Mp/Uint32.h"
#include "Kit/Io/File/System.h"
#include "Kit/Logging/Pkg/Log.h"
#include "Kit/Logging/Framework/Logger.h"
#include <inttypes.h>

#define SECT_ "_0test"

///
using namespace Kit::System;
using namespace Kit::TShell;
using namespace Kit::Type;
using namespace Kit::Dm;
using namespace Kit::Dm::TShell;

namespace {

class KitLoggingOnlyApp : public Kit::Logging::Framework::IApplication
{
public:
    /// Constructor
    KitLoggingOnlyApp( Kit::Container::RingBufferMP<Kit::Logging::Framework::EntryData_T>& logFifo ) noexcept
        : m_kitPackage()
    {
        Kit::Logging::Framework::initialize( *this, logFifo );
    }

public:
    bool isClassificationIdValid( uint8_t classificationId ) noexcept override
    {
        return Kit::Logging::Pkg::ClassificationId::_from_integral_nothrow( classificationId );
    }

    const char* classificationIdToString( uint8_t classificationId ) noexcept override
    {
        return Kit::Type::betterEnumToString<Kit::Logging::Pkg::ClassificationId, uint8_t>(
            classificationId,
            NULL_CLASSIFICATION_ID_TEXT );
    }

    Kit::Logging::Framework::IPackage* getPackage( uint8_t packageId ) noexcept override
    {
        if ( packageId == m_kitPackage.PACKAGE_ID )
        {
            return &m_kitPackage;
        }
        return nullptr;
    }

public:
    Kit::Logging::Pkg::Package m_kitPackage;
};

}  // end anonymous namespace


static ModelDatabase       mpDatabase_( "ignore_static_constructor" );
static Kit::Dm::Mp::Uint32 logQueueCount_( mpDatabase_, "logQueueCount" );

static constexpr unsigned    ENTRY_FIFO_SIZE     = 12;
static constexpr unsigned    ENTRY_REGION_SIZE   = 4096;
static constexpr const char* MEDIA_FILE_NAME     = "logs.bin";
static constexpr const char* MEDIA_IDX_FILE_NAME = "logidx.bin";

static Kit::Logging::Framework::EntryData_T                               entryFifoMem_[ENTRY_FIFO_SIZE];
static Kit::Container::RingBufferMP<Kit::Logging::Framework::EntryData_T> entryFifo_( logQueueCount_, entryFifoMem_, ENTRY_FIFO_SIZE, false );

static Kit::Persistence::Record::Media::FileAdapter indexFd_( MEDIA_IDX_FILE_NAME, 128 );
static Kit::Persistence::Record::Media::FileAdapter entriesFd_( MEDIA_FILE_NAME, ENTRY_REGION_SIZE );
static Kit::Checksum::Crc16CcittFast                indexCrc_;
static Kit::Checksum::Crc16CcittFast                entriesCrc_;
static Kit::Persistence::Record::Chunk::Crc         indexChunk_( indexFd_, indexCrc_ );
static Kit::Persistence::Record::Chunk::Crc         entriesChunk_( entriesFd_, entriesCrc_ );
//
static Kit::Persistence::Record::Journal::HeadRecord  headRecord_( indexChunk_ );
static Kit::Persistence::Record::Journal::EntryRecord entryRecord_( entriesChunk_,
                                                                    Kit::Logging::Framework::EntryData_T::entryLen,
                                                                    entriesFd_,
                                                                    headRecord_ );
//
static Kit::EventQueue::Server                                                         persistentEventQueue_;
static Kit::Persistence::Record::Journal::Server<Kit::Logging::Framework::EntryData_T> journalServer_( persistentEventQueue_,
                                                                                                       entryRecord_,
                                                                                                       entryFifo_ );
static Kit::System::Thread*                                                            persistentThread_ = nullptr;

static KitLoggingOnlyApp appLogInfo_( entryFifo_ );
static Kit::Logging::Framework::JournalLogSink journalLogSink_( persistentEventQueue_, entryFifo_, entryRecord_ );


Kit::Container::OrderedList<Kit::TShell::ICommand> g_commandList( "ignore_static_constructor" );
static NoSecurity                                  securityPolicy_;
static Kit::Framing::StreamSource                  streamSrc_;
static Kit::Framing::StreamDestination             streamDst_;

static Processor tshell_( g_commandList,
                          streamSrc_,
                          streamDst_,
                          securityPolicy_,
                          Kit::System::PrivateLocks::tracingOutput() );

static StdioThread                             stdioThread_( tshell_ );
static Command::Bye                            byeCmd_( g_commandList );
static Command::Help                           helpCmd_( g_commandList );
static Command::Echo                           echoCmd_( g_commandList );
static Command::Trace                          traceCmd_( g_commandList );
static Command::Wait                           waitCmd_( g_commandList );
static Read                                    dmrCmd_( g_commandList, mpDatabase_ );
static Write                                   dmwCmd_( g_commandList, mpDatabase_ );
static Kit::Logging::Framework::TShell::Viewer viewerCmd_( g_commandList, appLogInfo_, journalServer_ );

void shell_test( Kit::Io::IInput& infd, Kit::Io::IOutput& outfd )
{
    Kit::Io::File::System::remove( MEDIA_IDX_FILE_NAME );
    Kit::Io::File::System::remove( MEDIA_FILE_NAME );

    // Seed a few log entries before persistent storage is opened/started.
    KIT_LOGGING_LOG_SYSTEM( Kit::Logging::Pkg::ClassificationId::WARNING,
                            Kit::Logging::Pkg::SystemMsgId::SHUTDOWN,
                            "persisted entry ONE" );

    KIT_LOGGING_LOG_SYSTEM( Kit::Logging::Pkg::ClassificationId::EVENT,
                            Kit::Logging::Pkg::SystemMsgId::SHUTDOWN,
                            "persisted entry TWO" );
    KIT_LOGGING_LOG_DRIVER( Kit::Logging::Pkg::ClassificationId::INFO,
                            Kit::Logging::Pkg::DriverMsgId::STOP_ERR,
                            "persisted entry THREE" );
    KIT_LOGGING_LOG_DRIVER( Kit::Logging::Pkg::ClassificationId::WARNING,
                            Kit::Logging::Pkg::DriverMsgId::START_ERR,
                            "persisted entry FOUR" );

    // Bring up the persistence server used by the Viewer command.
    persistentThread_ = Kit::System::Thread::create( persistentEventQueue_, "LOG-PERSIST" );
    if ( persistentThread_ != nullptr )
    {
        journalServer_.open();
        journalLogSink_.open();
    }

    stdioThread_.launchTShell( infd, outfd );

    // Wait forever - the 'bye' command is responsible for exiting
    Kit::System::sleep( 0xFFFFFFFF );
}