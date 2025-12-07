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


#include "Kit/Memory/CursorBE.h"

// #include "Kit/Persistent/Payload.h" // TODO: Add persistent logging support
#include <cstdint>
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
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_CLASSIFICATION_ID_TEXT
#define OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_CLASSIFICATION_ID_TEXT 16
#endif

/** Maximum number of characters in the application DomainID's enum symbols
 */
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_PACKAGE_ID_TEXT
#define OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_PACKAGE_ID_TEXT 16
#endif

/** Maximum number of characters in the application SourceID's enum symbols
 */
#ifndef OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_SUBSYSTEM_ID_TEXT
#define OPTION_KIT_LOGGING_FRAMEWORK_MAX_LEN_SUBSYSTEM_ID_TEXT 16
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
    uint64_t m_timestamp;                                                    //!< Time-stamp - as Kit::Time::BootTime - for the entry.
    uint8_t  m_classificationId;                                             //!< Classification identifier.  Valid Range is [1 to the number of bits in KitLoggingClassificationMask_T]
    uint8_t  m_packageId;                                                    //!< Package identifier. Valid Range is [1 to the number of bits in KitLoggingPackageMask_T]
    uint8_t  m_subSystemId;                                                  //!< SubSystem identifier. Valid Range is [0-254]. SubSystem values are ONLY unique WITHIN a Package.
    uint8_t  m_messageId;                                                    //!< Message type enumeration identifier.  Valid Range is [0-254]. Message IDs are ONLY unique WITHIN a SubSystem.
    char     m_infoText[OPTION_KIT_LOGGING_FRAMEWORK_MAX_MSG_TEXT_LEN + 1];  //!< The information text associated with log entry.

public:
    /// Total 'packed' length
    static constexpr unsigned entryLen = sizeof( m_timestamp ) + sizeof( m_classificationId ) + sizeof( m_packageId ) + sizeof( m_subSystemId ) + sizeof( m_messageId ) + sizeof( m_infoText );

public:
    /// Constructor
    EntryData_T()
        : m_timestamp( 0 )
        , m_classificationId( 0 )
        , m_packageId( 0 )
        , m_subSystemId( 0 )
        , m_messageId( 0 )
    {
        memset( m_infoText, 0, sizeof( m_infoText ) );
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
            cursor.writeU8( m_classificationId );
            cursor.writeU8( m_packageId );
            cursor.writeU8( m_subSystemId );
            cursor.writeU8( m_messageId );
            cursor.write( m_infoText, sizeof( m_infoText ) );
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
            cursor.readU8( m_classificationId );
            cursor.readU8( m_packageId );
            cursor.readU8( m_subSystemId );
            cursor.readU8( m_messageId );
            cursor.read( m_infoText, sizeof( m_infoText ) );
            return !cursor.errorOccurred();
        }
        return false;
    }
};


}  // end namespaces
}
}
#endif  // end header latch
