#ifndef KIT_DRIVER_NV_FLASH_API_H_
#define KIT_DRIVER_NV_FLASH_API_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */


#include "Kit/Driver/NV/IApi.h"
#include "Kit/Driver/Flash/IApi.h"
#include "Kit/Checksum/IEdc.h"
#include "Kit/System/Assert.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>

///
namespace Kit {
///
namespace Driver {
///
namespace NV {
///
namespace Flash {


/** NV Record status values.  Transitions are one-directional:
    ERASED -> VALID -> INVALID.  Each transition only requires clearing
    bits (1->0), which flash hardware allows without an erase cycle.
 */
struct PageStatus_T
{
    static constexpr uint32_t ERASED  = 0xFFFFFFFF;  //!< Record slot is empty
    static constexpr uint32_t VALID   = 0xFFFF0000;  //!< Record contains current data
    static constexpr uint32_t INVALID = 0x00000000;  //!< Record is obsolete
};


/** NV Record header structure (24 bytes).  Stored at the beginning of each
    NV Record in flash.
 */
struct PageHeader_T
{
    uint32_t magic;        //!< Magic number (0xA5A5A5A5) identifying a valid header
    uint32_t sequenceNum;  //!< Monotonically increasing sequence number
    uint32_t dataOffset;   //!< Starting offset of this record within the logical NV space
    uint32_t dataLength;   //!< Length of the data payload (equals nvPageSize)
    uint32_t crc32;        //!< CRC32 over the preceding 16 bytes (magic through dataLength)
    uint32_t status;       //!< Record lifecycle state (ERASED, VALID, or INVALID)
};


/** Magic number used to identify valid NV Record headers */
static constexpr uint32_t MAGIC_NUMBER = 0xA5A5A5A5;

/** Size of the NV Record header in bytes */
static constexpr size_t HEADER_SIZE = sizeof( PageHeader_T );

/** Number of header bytes covered by the CRC calculation (magic through dataLength) */
static constexpr size_t CRC_HEADER_FIELD_SIZE = 16;

/** Value used in the page map to indicate no valid physical page exists */
static constexpr uint32_t INVALID_PAGE_ADDRESS = 0xFFFFFFFF;

/** Byte value representing the erased state of flash memory */
static constexpr uint8_t ERASED_BYTE_VALUE = 0xFF;


/** Configuration structure for the NV flash driver */
struct Config_T
{
    uint32_t flashStartAddress;  //!< Base address of the NV region in flash
    uint32_t flashEndAddress;    //!< End address (exclusive) of the NV region in flash
    size_t   nvPageSize;         //!< Logical NV page size (data portion only, e.g. 256 bytes)
    size_t   nvTotalSize;        //!< Total NV storage size presented to the application
};


/** This class implements the NV::IApi interface using a flash device (via
    Flash::IApi) for persistent storage with wear leveling.

    The implementation uses a log-structured approach where each write creates
    a new NV Record containing a record header and a full page of data.  A
    read-modify-write pattern ensures that unmodified bytes within a page are
    preserved.

    An in-memory page map provides O(1) lookup from logical page index to
    physical flash address.  On startup, the page map is rebuilt by scanning
    all NV Records in the flash region.

    Sector reclamation uses a two-phase approach: first scan for already-erased
    slots, and only erase a sector when no erased slots remain and the sector
    contains no VALID records.

    Template parameter MAX_LOGICAL_PAGES defines the maximum number of logical
    pages supported (determines size of the in-memory page map array).
 */
template <size_t MAX_LOGICAL_PAGES>
class Api : public NV::IApi
{
public:
    /** Constructor.  The 'flashDriver' and 'crcAlgo' references must remain
        valid for the lifetime of this object.  The 'workBuffer' must be at
        least config.nvPageSize bytes and must remain valid for the lifetime
        of this object.
     */
    Api( Kit::Driver::Flash::IApi& flashDriver,
         Kit::Checksum::IEdc&      crcAlgo,
         const Config_T&           config,
         uint8_t*                  workBuffer,
         size_t                    workBufferSize ) noexcept
        : m_flashDriver( flashDriver )
        , m_crcAlgo( crcAlgo )
        , m_workBuffer( workBuffer )
        , m_config( config )
        , m_currentSequence( 0 )
        , m_workBufferSize( workBufferSize )
        , m_numSectors( 0 )
        , m_pagesPerSector( 0 )
        , m_totalPages( 0 )
        , m_numLogicalPages( 0 )
        , m_eraseCount( 0 )
        , m_isStarted( false )
    {
        KIT_SYSTEM_ASSERT( config.nvPageSize > 0 );
        KIT_SYSTEM_ASSERT( config.nvTotalSize > 0 );
        KIT_SYSTEM_ASSERT( config.flashEndAddress > config.flashStartAddress );
        KIT_SYSTEM_ASSERT( workBuffer != nullptr );
        KIT_SYSTEM_ASSERT( workBufferSize >= config.nvPageSize );

        if ( config.nvPageSize > 0 )
        {
            m_numLogicalPages = config.nvTotalSize / config.nvPageSize;
            KIT_SYSTEM_ASSERT( m_numLogicalPages <= MAX_LOGICAL_PAGES );
        }

        memset( m_pageMap, ERASED_BYTE_VALUE, sizeof( m_pageMap ) );
    }


public:
    bool start( void* startArgs = nullptr ) noexcept override
    {
        if ( m_isStarted )
        {
            return true;
        }

        size_t sectorSize   = m_flashDriver.getSectorSize();
        size_t nvRegionSize = m_config.flashEndAddress - m_config.flashStartAddress;
        m_numSectors        = nvRegionSize / sectorSize;
        m_pagesPerSector    = sectorSize / getPhysicalPageSize();
        m_totalPages        = m_numSectors * m_pagesPerSector;

        memset( m_pageMap, ERASED_BYTE_VALUE, sizeof( m_pageMap ) );

        if ( !scanAndBuildPageMap() )
        {
            return false;
        }

        m_isStarted = true;
        return true;
    }

    void stop() noexcept override
    {
        m_isStarted = false;
    }


public:
    bool write( size_t      dstOffset,
                const void* srcData,
                size_t      numBytesToWrite ) noexcept override
    {
        if ( !m_isStarted || srcData == nullptr || numBytesToWrite == 0 )
        {
            return false;
        }

        if ( dstOffset + numBytesToWrite > m_config.nvTotalSize )
        {
            return false;
        }

        const uint8_t* src       = static_cast<const uint8_t*>( srcData );
        size_t         remaining = numBytesToWrite;
        size_t         offset    = dstOffset;

        while ( remaining > 0 )
        {
            size_t pageIndex = offsetToPageIndex( offset );
            if ( pageIndex >= m_numLogicalPages )
            {
                return false;
            }

            size_t offsetInPage  = offset % m_config.nvPageSize;
            size_t bytesThisPage = m_config.nvPageSize - offsetInPage;
            if ( bytesThisPage > remaining )
            {
                bytesThisPage = remaining;
            }

            if ( !readCurrentPageData( pageIndex, m_workBuffer ) )
            {
                return false;
            }

            memcpy( m_workBuffer + offsetInPage, src, bytesThisPage );

            uint32_t oldPageAddr = m_pageMap[pageIndex];
            uint32_t freeAddr    = findFreePageAddress( oldPageAddr );
            if ( freeAddr == INVALID_PAGE_ADDRESS )
            {
                return false;
            }

            PageHeader_T header;
            header.magic       = MAGIC_NUMBER;
            header.sequenceNum = static_cast<uint32_t>( ++m_currentSequence );
            header.dataOffset  = static_cast<uint32_t>( pageIndex * m_config.nvPageSize );
            header.dataLength  = static_cast<uint32_t>( m_config.nvPageSize );
            header.crc32       = calculateCrc( &header, CRC_HEADER_FIELD_SIZE );
            header.status      = PageStatus_T::VALID;

            if ( !m_flashDriver.write( freeAddr, &header, HEADER_SIZE ) )
            {
                return false;
            }

            if ( !m_flashDriver.write( freeAddr + HEADER_SIZE, m_workBuffer, m_config.nvPageSize ) )
            {
                return false;
            }

            if ( m_pageMap[pageIndex] != INVALID_PAGE_ADDRESS )
            {
                if ( !markPageInvalid( m_pageMap[pageIndex] ) )
                {
                    return false;
                }
            }

            m_pageMap[pageIndex] = freeAddr;

            src       += bytesThisPage;
            offset    += bytesThisPage;
            remaining -= bytesThisPage;
        }

        return true;
    }

    bool read( size_t srcOffset,
               void*  dstData,
               size_t sizeDstData,
               size_t numBytesToRead ) noexcept override
    {
        if ( !m_isStarted || dstData == nullptr || numBytesToRead == 0 )
        {
            return false;
        }

        if ( sizeDstData < numBytesToRead )
        {
            return false;
        }

        if ( srcOffset + numBytesToRead > m_config.nvTotalSize )
        {
            return false;
        }

        uint8_t* dst       = static_cast<uint8_t*>( dstData );
        size_t   remaining = numBytesToRead;
        size_t   offset    = srcOffset;

        while ( remaining > 0 )
        {
            size_t pageIndex = offsetToPageIndex( offset );
            if ( pageIndex >= m_numLogicalPages )
            {
                return false;
            }

            size_t offsetInPage  = offset % m_config.nvPageSize;
            size_t bytesThisPage = m_config.nvPageSize - offsetInPage;
            if ( bytesThisPage > remaining )
            {
                bytesThisPage = remaining;
            }

            if ( m_pageMap[pageIndex] != INVALID_PAGE_ADDRESS )
            {
                size_t dataAddr = m_pageMap[pageIndex] + HEADER_SIZE + offsetInPage;
                if ( !m_flashDriver.read( dataAddr, dst, bytesThisPage ) )
                {
                    return false;
                }
            }
            else
            {
                memset( dst, ERASED_BYTE_VALUE, bytesThisPage );
            }

            dst       += bytesThisPage;
            offset    += bytesThisPage;
            remaining -= bytesThisPage;
        }

        return true;
    }

    size_t getTotalSize() const noexcept override { return m_config.nvTotalSize; }
    size_t getNumPages() const noexcept override { return m_numLogicalPages; }
    size_t getPageSize() const noexcept override { return m_config.nvPageSize; }


public:
    /// Erases all sectors and resets the page map, effectively formatting the NV storage.
    bool format() noexcept
    {
        if ( !m_isStarted )
        {
            return false;
        }

        size_t sectorSize = m_flashDriver.getSectorSize();

        for ( size_t i = 0; i < m_numSectors; i++ )
        {
            size_t sectorAddr = m_config.flashStartAddress + ( i * sectorSize );
            if ( !m_flashDriver.eraseSector( sectorAddr ) )
            {
                return false;
            }
            m_eraseCount++;
        }

        m_currentSequence = 0;
        memset( m_pageMap, ERASED_BYTE_VALUE, sizeof( m_pageMap ) );

        return true;
    }

    /// Returns current statistics: cumulative erase count, free pages, and valid pages.
    bool getStatistics( uint32_t& eraseCount,
                        size_t&   freePages,
                        size_t&   validPages ) noexcept
    {
        if ( !m_isStarted )
        {
            return false;
        }

        eraseCount = m_eraseCount;
        freePages  = 0;
        validPages = 0;

        size_t sectorSize   = m_flashDriver.getSectorSize();
        size_t physPageSize = getPhysicalPageSize();

        for ( size_t i = 0; i < m_totalPages; i++ )
        {
            size_t sectorIndex = i / m_pagesPerSector;
            size_t slotIndex   = i % m_pagesPerSector;
            size_t address     = m_config.flashStartAddress + ( sectorIndex * sectorSize ) + ( slotIndex * physPageSize );

            PageHeader_T header;
            if ( !m_flashDriver.read( address, &header, HEADER_SIZE ) )
            {
                return false;
            }

            if ( header.magic == 0xFFFFFFFF && header.status == PageStatus_T::ERASED )
            {
                freePages++;
            }
            else if ( header.magic == MAGIC_NUMBER && header.status == PageStatus_T::VALID )
            {
                validPages++;
            }
        }

        return true;
    }


protected:
    /// Scans all physical pages and rebuilds the logical-to-physical page map.
    bool scanAndBuildPageMap() noexcept
    {
        size_t   sectorSize   = m_flashDriver.getSectorSize();
        size_t   physPageSize = getPhysicalPageSize();
        uint64_t maxSeq       = 0;

        uint32_t pageSeqNums[MAX_LOGICAL_PAGES];
        memset( pageSeqNums, 0, sizeof( pageSeqNums ) );

        for ( size_t i = 0; i < m_totalPages; i++ )
        {
            size_t sectorIndex = i / m_pagesPerSector;
            size_t slotIndex   = i % m_pagesPerSector;
            size_t address     = m_config.flashStartAddress + ( sectorIndex * sectorSize ) + ( slotIndex * physPageSize );

            PageHeader_T header;
            if ( !m_flashDriver.read( address, &header, HEADER_SIZE ) )
            {
                return false;
            }

            if ( header.magic != MAGIC_NUMBER || header.status != PageStatus_T::VALID )
            {
                continue;
            }

            uint32_t expectedCrc = calculateCrc( &header, CRC_HEADER_FIELD_SIZE );
            if ( header.crc32 != expectedCrc )
            {
                continue;
            }

            size_t logicalIndex = header.dataOffset / m_config.nvPageSize;
            if ( logicalIndex >= m_numLogicalPages )
            {
                continue;
            }

            if ( header.sequenceNum > maxSeq )
            {
                maxSeq = header.sequenceNum;
            }

            if ( m_pageMap[logicalIndex] == INVALID_PAGE_ADDRESS ||
                 header.sequenceNum > pageSeqNums[logicalIndex] )
            {
                if ( m_pageMap[logicalIndex] != INVALID_PAGE_ADDRESS )
                {
                    markPageInvalid( static_cast<uint32_t>( m_pageMap[logicalIndex] ) );
                }

                m_pageMap[logicalIndex]   = static_cast<uint32_t>( address );
                pageSeqNums[logicalIndex] = header.sequenceNum;
            }
            else
            {
                markPageInvalid( static_cast<uint32_t>( address ) );
            }
        }

        m_currentSequence = maxSeq;
        return true;
    }

    /// Finds a free physical page slot, reclaiming a sector if necessary.
    uint32_t findFreePageAddress( uint32_t aboutToInvalidate = INVALID_PAGE_ADDRESS ) noexcept
    {
        size_t sectorSize   = m_flashDriver.getSectorSize();
        size_t physPageSize = getPhysicalPageSize();

        // Phase 1: Scan for an already-erased record slot
        for ( size_t i = 0; i < m_totalPages; i++ )
        {
            size_t sectorIndex = i / m_pagesPerSector;
            size_t slotIndex   = i % m_pagesPerSector;
            size_t address     = m_config.flashStartAddress + ( sectorIndex * sectorSize ) + ( slotIndex * physPageSize );

            PageHeader_T header;
            if ( !m_flashDriver.read( address, &header, HEADER_SIZE ) )
            {
                continue;
            }

            if ( header.magic == PageStatus_T::ERASED )
            {
                return static_cast<uint32_t>( address );
            }
        }

        // Phase 2: Try to reclaim a sector with no VALID records
        for ( size_t s = 0; s < m_numSectors; s++ )
        {
            size_t sectorAddr = m_config.flashStartAddress + ( s * sectorSize );
            bool   hasValid   = false;

            for ( size_t slot = 0; slot < m_pagesPerSector; slot++ )
            {
                size_t slotAddr = sectorAddr + ( slot * physPageSize );

                if ( static_cast<uint32_t>( slotAddr ) == aboutToInvalidate )
                {
                    continue;
                }

                PageHeader_T header;
                if ( !m_flashDriver.read( slotAddr, &header, HEADER_SIZE ) )
                {
                    hasValid = true;
                    break;
                }

                if ( header.magic == MAGIC_NUMBER && header.status == PageStatus_T::VALID )
                {
                    hasValid = true;
                    break;
                }
            }

            if ( !hasValid )
            {
                if ( aboutToInvalidate != INVALID_PAGE_ADDRESS )
                {
                    size_t aboutToInvalidateSector = aboutToInvalidate - ( aboutToInvalidate % sectorSize );
                    if ( aboutToInvalidateSector == sectorAddr )
                    {
                        markPageInvalid( aboutToInvalidate );
                    }
                }

                if ( m_flashDriver.eraseSector( sectorAddr ) )
                {
                    m_eraseCount++;
                    return static_cast<uint32_t>( sectorAddr );
                }
            }
        }

        return INVALID_PAGE_ADDRESS;
    }

    /// Marks the page at the specified physical address as invalid.
    bool markPageInvalid( uint32_t pageAddress ) noexcept
    {
        uint32_t invalidStatus = PageStatus_T::INVALID;
        size_t   statusOffset  = pageAddress + offsetof( PageHeader_T, status );
        return m_flashDriver.write( statusOffset, &invalidStatus, sizeof( invalidStatus ) );
    }

    /// Erases the sector if it contains no valid pages.
    bool eraseSectorIfNeeded( size_t sectorAddress ) noexcept
    {
        size_t sectorSize   = m_flashDriver.getSectorSize();
        size_t physPageSize = getPhysicalPageSize();

        for ( size_t slot = 0; slot < m_pagesPerSector; slot++ )
        {
            size_t       slotAddr = sectorAddress + ( slot * physPageSize );
            PageHeader_T header;
            if ( !m_flashDriver.read( slotAddr, &header, HEADER_SIZE ) )
            {
                return false;
            }

            if ( header.magic == MAGIC_NUMBER && header.status == PageStatus_T::VALID )
            {
                return false;
            }
        }

        if ( m_flashDriver.eraseSector( sectorAddress ) )
        {
            m_eraseCount++;
            return true;
        }

        return false;
    }

    /// Reads the data payload of the current physical page for the given logical page index.
    bool readCurrentPageData( size_t pageIndex, uint8_t* buffer ) noexcept
    {
        if ( pageIndex >= m_numLogicalPages )
        {
            return false;
        }

        if ( m_pageMap[pageIndex] != INVALID_PAGE_ADDRESS )
        {
            size_t dataAddr = m_pageMap[pageIndex] + HEADER_SIZE;
            return m_flashDriver.read( dataAddr, buffer, m_config.nvPageSize );
        }
        else
        {
            memset( buffer, ERASED_BYTE_VALUE, m_config.nvPageSize );
            return true;
        }
    }

    /// Computes a CRC-32 over the given data using the configured CRC algorithm.
    uint32_t calculateCrc( const void* data, size_t length ) noexcept
    {
        m_crcAlgo.reset();
        m_crcAlgo.accumulate( data, static_cast<unsigned>( length ) );
        uint32_t result = 0;
        m_crcAlgo.finalize( &result, sizeof( result ) );
        return result;
    }

    /// Returns the physical page size (header + data payload).
    size_t getPhysicalPageSize() const noexcept { return HEADER_SIZE + m_config.nvPageSize; }
    /// Converts a byte offset within the NV region to a logical page index.
    size_t offsetToPageIndex( size_t offset ) const noexcept { return offset / m_config.nvPageSize; }


protected:
    Kit::Driver::Flash::IApi& m_flashDriver;                 //!< Reference to flash driver
    Kit::Checksum::IEdc&      m_crcAlgo;                     //!< Reference to CRC algorithm
    uint8_t*                  m_workBuffer;                  //!< Application-provided work buffer
    uint32_t                  m_pageMap[MAX_LOGICAL_PAGES];  //!< Logical page -> physical address map
    Config_T                  m_config;                      //!< Storage configuration
    uint64_t                  m_currentSequence;             //!< Highest sequence number seen
    size_t                    m_workBufferSize;              //!< Size of application-provided work buffer
    size_t                    m_numSectors;                  //!< Number of sectors in NV region
    size_t                    m_pagesPerSector;              //!< Physical NV Records per sector
    size_t                    m_totalPages;                  //!< Total physical NV Record slots
    size_t                    m_numLogicalPages;             //!< Number of logical pages
    uint32_t                  m_eraseCount;                  //!< Total erase count
    bool                      m_isStarted;                   //!< Driver state flag
};


}  // end namespaces
}
}
}

#endif  // end header latch
