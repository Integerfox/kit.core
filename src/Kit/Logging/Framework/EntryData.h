#ifndef KIT_LOGGING_FRAMEWORK_ENTRYDATA_H_
#define KIT_LOGGING_FRAMEWORK_ENTRYDATA_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Logging/Framework/TimeApi.h"
#include "Kit/Memory/CursorBE.h"

// #include "Kit/Persistent/Payload.h" // TODO: Add persistent logging support
#include <stdint.h>
#include <string.h>

/** The size, in bytes, reserved to store the text portion of the log entry.
    The size does NOT include the space reserved for the null terminator
 */
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_MAX_MSG_TEXT_LEN
#define OPTION_KIT_LOGGING_FRAMEWORK_MAX_MSG_TEXT_LEN 128
#endif

/** Maximum number of characters in the application CategoryID's enum symbols
 */
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_CATEGORY_ID_TEXT
#define OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_CATEGORY_ID_TEXT 16
#endif

/** Maximum number of characters in the application DomainID's enum symbols
 */
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_DOMAIN_ID_TEXT
#define OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_DOMAIN_ID_TEXT 16
#endif

/** Maximum number of characters in the application SourceID's enum symbols
 */
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_SOURCE_ID_TEXT
#define OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_SOURCE_ID_TEXT 16
#endif

/** Maximum number of characters in the application MessageID's enum symbols
 */
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_MESSAGE_ID_TEXT
#define OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_MESSAGE_ID_TEXT 32
#endif


///
namespace Kit {
///
namespace Logging {
///
namespace Framework {

/// Defines the content of the Log entry
struct EntryData_T  // : public Kit::Persistent::Payload
{
public:
    KitLoggingTime_T m_timestamp;                                                   //!< Time-stamp for the entry
    uint8_t          m_category;                                                    //!< Category identifier.  Valid Range is [1-32]
    uint8_t          m_domain;                                                      //!< Domain identifier. Valid Range is [1-32]
    uint8_t          m_subSystem;                                                   //!< SubSystem identifier. Valid Range is [0-255]. SubSystem values are ONLY unique WITHIN a Domain.
    uint8_t          m_msgId;                                                       //!< Message type enumeration identifier.  Valid Range is [0-255]. Message IDs are ONLY unique WITHIN a SubSystem.
    char             m_msgText[OPTION_KIT_LOGGING_FRAMEWORK_MAX_MSG_TEXT_LEN + 1];  //!< The 'text' associated with log entry.

public:
    /// Total 'packed' length
    static constexpr unsigned entryLen = sizeof( m_timestamp ) + sizeof( m_category ) + sizeof( m_domain ) + sizeof( m_subSystem ) + sizeof( m_msgId ) + sizeof( m_msgText );

public:
    /// Constructor
    EntryData_T()
        : m_timestamp( 0 )
        , m_category( 0 )
        , m_domain( 0 )
        , m_subSystem( 0 )
        , m_msgId( 0 )
    {
        memset( m_msgText, 0, sizeof( m_msgText ) );
    }

public:
    /// See Kit::Persistent::Payload (manual copy to ensure no pad bytes are copied)
    /// NOTE: The data is stored in Big-Endian format in persistent storage
    size_t getData( void* dst, size_t maxDstLen ) noexcept
    {
        if ( maxDstLen >= entryLen )
        {
            Kit::Memory::CursorBE cursor( (uint8_t*)dst, maxDstLen );
            cursor.writeU64( m_timestamp );
            cursor.writeU8( m_category );
            cursor.writeU8( m_domain );
            cursor.writeU8( m_subSystem );
            cursor.writeU8( m_msgId );
            cursor.write( m_msgText, sizeof( m_msgText ) );
            return cursor.errorOccurred() ? 0 : entryLen;
        }
        return 0;
    }

    /// See Kit::Persistent::Payload
    bool putData( const void* src, size_t srcLen ) noexcept
    {
        if ( srcLen >= entryLen )
        {
            Kit::Memory::CursorBE cursor( (uint8_t*)src, srcLen );
            cursor.readU64( m_timestamp );
            cursor.readU8( m_category );
            cursor.readU8( m_domain );
            cursor.readU8( m_subSystem );
            cursor.readU8( m_msgId );
            cursor.read( m_msgText, sizeof( m_msgText ) );
            return !cursor.errorOccurred();
        }
        return false;
    }
};


}  // end namespaces
}
}
#endif  // end header latch
