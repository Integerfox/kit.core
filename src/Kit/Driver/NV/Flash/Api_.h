#ifndef KIT_DRIVER_NV_FLASH_API__H_
#define KIT_DRIVER_NV_FLASH_API__H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Template implementation for Kit::Driver::NV::Flash::Api.
    This file is included by Api.h and should not be included directly.
*/


#include <string.h>
#include <stddef.h>

///
namespace Kit {
///
namespace Driver {
///
namespace NV {
///
namespace Flash {


//////////////////////////////////////////////////////////////////////////////
template <size_t MAX_LOGICAL_PAGES>
Api<MAX_LOGICAL_PAGES>::Api( Kit::Driver::Flash::IApi& flashDriver,
                             const Config_T&           config ) noexcept
    : m_flashDriver( flashDriver )
    , m_config( config )
    , m_isStarted( false )
    , m_currentSequence( 0 )
    , m_eraseCount( 0 )
    , m_numSectors( 0 )
    , m_pagesPerSector( 0 )
    , m_totalPages( 0 )
    , m_numLogicalPages( 0 )
{
    memset( m_pageMap, ERASED_BYTE_VALUE, sizeof( m_pageMap ) );
}


//////////////////////////////////////////////////////////////////////////////
template <size_t MAX_LOGICAL_PAGES>
bool Api<MAX_LOGICAL_PAGES>::start( void* startArgs ) noexcept
{
    if ( m_isStarted )
    {
        return true;
    }

    // Validate configuration
    if ( m_config.nvPageSize == 0 || m_config.nvTotalSize == 0 )
    {
        return false;
    }
    if ( m_config.flashEndAddress <= m_config.flashStartAddress )
    {
        return false;
    }

    // Calculate layout parameters
    size_t sectorSize    = m_flashDriver.getSectorSize();
    size_t nvRegionSize  = m_config.flashEndAddress - m_config.flashStartAddress;
    m_numSectors         = nvRegionSize / sectorSize;
    m_pagesPerSector     = sectorSize / getPhysicalPageSize();
    m_totalPages         = m_numSectors * m_pagesPerSector;
    m_numLogicalPages    = m_config.nvTotalSize / m_config.nvPageSize;

    // Validate that we have enough logical page slots
    if ( m_numLogicalPages > MAX_LOGICAL_PAGES )
    {
        return false;
    }

    // Initialize page map to "no valid data"
    memset( m_pageMap, ERASED_BYTE_VALUE, sizeof( m_pageMap ) );

    // Scan flash and build page map
    if ( !scanAndBuildPageMap() )
    {
        return false;
    }

    m_isStarted = true;
    return true;
}


//////////////////////////////////////////////////////////////////////////////
template <size_t MAX_LOGICAL_PAGES>
void Api<MAX_LOGICAL_PAGES>::stop() noexcept
{
    m_isStarted = false;
}


//////////////////////////////////////////////////////////////////////////////
template <size_t MAX_LOGICAL_PAGES>
bool Api<MAX_LOGICAL_PAGES>::write( size_t      dstOffset,
                                    const void* srcData,
                                    size_t      numBytesToWrite ) noexcept
{
    if ( !m_isStarted || srcData == nullptr || numBytesToWrite == 0 )
    {
        return false;
    }

    // Validate bounds
    if ( dstOffset + numBytesToWrite > m_config.nvTotalSize )
    {
        return false;
    }

    const uint8_t* src       = static_cast<const uint8_t*>( srcData );
    size_t         remaining = numBytesToWrite;
    size_t         offset    = dstOffset;

    // Process each logical page that this write touches
    while ( remaining > 0 )
    {
        size_t pageIndex    = offsetToPageIndex( offset );
        if ( pageIndex >= m_numLogicalPages )
        {
            return false;
        }

        size_t offsetInPage = offset % m_config.nvPageSize;
        size_t bytesThisPage = m_config.nvPageSize - offsetInPage;
        if ( bytesThisPage > remaining )
        {
            bytesThisPage = remaining;
        }

        // Allocate a stack buffer for read-modify-write
        // For embedded safety, we use a fixed max and validate at runtime
        uint8_t pageBuf[MAX_NV_PAGE_SIZE]; // Max supported nvPageSize
        if ( m_config.nvPageSize > MAX_NV_PAGE_SIZE )
        {
            return false; // Page size exceeds our buffer
        }

        // Step 1: Read existing page data (or fill with 0xFF)
        if ( !readCurrentPageData( pageIndex, pageBuf ) )
        {
            return false;
        }

        // Step 2: Merge user data into the page buffer
        memcpy( pageBuf + offsetInPage, src, bytesThisPage );

        // Step 3: Find a free record slot (pass old page so its sector can be reclaimed)
        uint32_t oldPageAddr = m_pageMap[pageIndex];
        uint32_t freeAddr = findFreePageAddress( oldPageAddr );
        if ( freeAddr == INVALID_PAGE_ADDRESS )
        {
            return false; // No space available
        }

        // Step 4: Build the NV Record header
        PageHeader_T header;
        header.magic       = MAGIC_NUMBER;
        header.sequenceNum = ++m_currentSequence;
        header.dataOffset  = static_cast<uint32_t>( pageIndex * m_config.nvPageSize );
        header.dataLength  = static_cast<uint32_t>( m_config.nvPageSize );
        header.crc32       = calculateCrc32( &header, CRC_HEADER_FIELD_SIZE ); // CRC over first 4 fields
        header.status      = PageStatus_T::VALID;

        // Step 5: Write header to flash
        if ( !m_flashDriver.write( freeAddr, &header, HEADER_SIZE ) )
        {
            return false;
        }

        // Step 6: Write merged data to flash
        if ( !m_flashDriver.write( freeAddr + HEADER_SIZE, pageBuf, m_config.nvPageSize ) )
        {
            return false;
        }

        // Step 7: Mark old record as INVALID (if one existed)
        if ( m_pageMap[pageIndex] != INVALID_PAGE_ADDRESS )
        {
            if ( !markPageInvalid( m_pageMap[pageIndex] ) )
            {
                return false;
            }
        }

        // Step 8: Update page map
        m_pageMap[pageIndex] = freeAddr;

        // Advance
        src       += bytesThisPage;
        offset    += bytesThisPage;
        remaining -= bytesThisPage;
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////
template <size_t MAX_LOGICAL_PAGES>
bool Api<MAX_LOGICAL_PAGES>::read( size_t srcOffset,
                                   void*  dstData,
                                   size_t numBytesToRead ) noexcept
{
    if ( !m_isStarted || dstData == nullptr || numBytesToRead == 0 )
    {
        return false;
    }

    // Validate bounds
    if ( srcOffset + numBytesToRead > m_config.nvTotalSize )
    {
        return false;
    }

    uint8_t* dst       = static_cast<uint8_t*>( dstData );
    size_t   remaining = numBytesToRead;
    size_t   offset    = srcOffset;

    // Process each logical page that this read touches
    while ( remaining > 0 )
    {
        size_t pageIndex    = offsetToPageIndex( offset );
        if ( pageIndex >= m_numLogicalPages )
        {
            return false;
        }

        size_t offsetInPage = offset % m_config.nvPageSize;
        size_t bytesThisPage = m_config.nvPageSize - offsetInPage;
        if ( bytesThisPage > remaining )
        {
            bytesThisPage = remaining;
        }

        if ( m_pageMap[pageIndex] != INVALID_PAGE_ADDRESS )
        {
            // O(1) page map lookup: read directly from flash
            size_t dataAddr = m_pageMap[pageIndex] + HEADER_SIZE + offsetInPage;
            if ( !m_flashDriver.read( dataAddr, dst, bytesThisPage ) )
            {
                return false;
            }
        }
        else
        {
            // No valid data for this page - fill with erased state
            memset( dst, ERASED_BYTE_VALUE, bytesThisPage );
        }

        // Advance
        dst       += bytesThisPage;
        offset    += bytesThisPage;
        remaining -= bytesThisPage;
    }

    return true;
}


//////////////////////////////////////////////////////////////////////////////
template <size_t MAX_LOGICAL_PAGES>
size_t Api<MAX_LOGICAL_PAGES>::getTotalSize() const noexcept
{
    return m_config.nvTotalSize;
}

template <size_t MAX_LOGICAL_PAGES>
size_t Api<MAX_LOGICAL_PAGES>::getNumPages() const noexcept
{
    return m_numLogicalPages;
}

template <size_t MAX_LOGICAL_PAGES>
size_t Api<MAX_LOGICAL_PAGES>::getPageSize() const noexcept
{
    return m_config.nvPageSize;
}


//////////////////////////////////////////////////////////////////////////////
template <size_t MAX_LOGICAL_PAGES>
bool Api<MAX_LOGICAL_PAGES>::format() noexcept
{
    if ( !m_isStarted )
    {
        return false;
    }

    size_t sectorSize = m_flashDriver.getSectorSize();

    // Erase all sectors in the NV region
    for ( size_t i = 0; i < m_numSectors; i++ )
    {
        size_t sectorAddr = m_config.flashStartAddress + ( i * sectorSize );
        if ( !m_flashDriver.eraseSector( sectorAddr ) )
        {
            return false;
        }
        m_eraseCount++;
    }

    // Reset driver state
    m_currentSequence = 0;
    memset( m_pageMap, ERASED_BYTE_VALUE, sizeof( m_pageMap ) );

    return true;
}


//////////////////////////////////////////////////////////////////////////////
template <size_t MAX_LOGICAL_PAGES>
bool Api<MAX_LOGICAL_PAGES>::getStatistics( uint32_t& eraseCount,
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

    size_t sectorSize     = m_flashDriver.getSectorSize();
    size_t physPageSize   = getPhysicalPageSize();

    // Count free and valid pages by scanning headers
    for ( size_t i = 0; i < m_totalPages; i++ )
    {
        size_t sectorIndex = i / m_pagesPerSector;
        size_t slotIndex   = i % m_pagesPerSector;
        size_t address     = m_config.flashStartAddress
                           + ( sectorIndex * sectorSize )
                           + ( slotIndex * physPageSize );

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


//////////////////////////////////////////////////////////////////////////////
template <size_t MAX_LOGICAL_PAGES>
bool Api<MAX_LOGICAL_PAGES>::scanAndBuildPageMap() noexcept
{
    size_t sectorSize   = m_flashDriver.getSectorSize();
    size_t physPageSize  = getPhysicalPageSize();
    uint32_t maxSeq     = 0;

    // Track the sequence number for each logical page currently in the map
    uint32_t pageSeqNums[MAX_LOGICAL_PAGES];
    memset( pageSeqNums, 0, sizeof( pageSeqNums ) );

    // Scan all record slots in the NV region
    for ( size_t i = 0; i < m_totalPages; i++ )
    {
        size_t sectorIndex = i / m_pagesPerSector;
        size_t slotIndex   = i % m_pagesPerSector;
        size_t address     = m_config.flashStartAddress
                           + ( sectorIndex * sectorSize )
                           + ( slotIndex * physPageSize );

        PageHeader_T header;
        if ( !m_flashDriver.read( address, &header, HEADER_SIZE ) )
        {
            return false;
        }

        // Skip non-valid records
        if ( header.magic != MAGIC_NUMBER || header.status != PageStatus_T::VALID )
        {
            continue;
        }

        // Validate CRC
        uint32_t expectedCrc = calculateCrc32( &header, CRC_HEADER_FIELD_SIZE );
        if ( header.crc32 != expectedCrc )
        {
            continue; // Corrupted header
        }

        // Determine logical page index
        size_t logicalIndex = header.dataOffset / m_config.nvPageSize;
        if ( logicalIndex >= m_numLogicalPages )
        {
            continue; // Invalid index
        }

        // Track highest sequence number
        if ( header.sequenceNum > maxSeq )
        {
            maxSeq = header.sequenceNum;
        }

        // Update page map if this record is newer
        if ( m_pageMap[logicalIndex] == INVALID_PAGE_ADDRESS ||
             header.sequenceNum > pageSeqNums[logicalIndex] )
        {
            // Mark old record as INVALID if one existed
            if ( m_pageMap[logicalIndex] != INVALID_PAGE_ADDRESS )
            {
                markPageInvalid( static_cast<uint32_t>( m_pageMap[logicalIndex] ) );
            }

            m_pageMap[logicalIndex]   = static_cast<uint32_t>( address );
            pageSeqNums[logicalIndex] = header.sequenceNum;
        }
        else
        {
            // This is an older duplicate - mark it INVALID
            markPageInvalid( static_cast<uint32_t>( address ) );
        }
    }

    m_currentSequence = maxSeq;
    return true;
}


//////////////////////////////////////////////////////////////////////////////
template <size_t MAX_LOGICAL_PAGES>
uint32_t Api<MAX_LOGICAL_PAGES>::findFreePageAddress( uint32_t aboutToInvalidate ) noexcept
{
    size_t sectorSize   = m_flashDriver.getSectorSize();
    size_t physPageSize  = getPhysicalPageSize();

    // Phase 1: Scan for an already-erased record slot
    for ( size_t i = 0; i < m_totalPages; i++ )
    {
        size_t sectorIndex = i / m_pagesPerSector;
        size_t slotIndex   = i % m_pagesPerSector;
        size_t address     = m_config.flashStartAddress
                           + ( sectorIndex * sectorSize )
                           + ( slotIndex * physPageSize );

        PageHeader_T header;
        if ( !m_flashDriver.read( address, &header, HEADER_SIZE ) )
        {
            continue;
        }

        // Check if the slot is erased (all 0xFF in magic field indicates erased)
        if ( header.magic == PageStatus_T::ERASED )
        {
            return static_cast<uint32_t>( address );
        }
    }

    // Phase 2: Try to reclaim a sector with no VALID records
    // (treating 'aboutToInvalidate' address as already INVALID)
    for ( size_t s = 0; s < m_numSectors; s++ )
    {
        size_t sectorAddr = m_config.flashStartAddress + ( s * sectorSize );
        bool   hasValid   = false;

        // Check all record slots in this sector
        for ( size_t slot = 0; slot < m_pagesPerSector; slot++ )
        {
            size_t slotAddr = sectorAddr + ( slot * physPageSize );

            // Treat the about-to-be-invalidated address as already invalid
            if ( static_cast<uint32_t>( slotAddr ) == aboutToInvalidate )
            {
                continue;
            }

            PageHeader_T header;
            if ( !m_flashDriver.read( slotAddr, &header, HEADER_SIZE ) )
            {
                hasValid = true; // Err on the side of caution
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
            // Mark the about-to-be-invalidated record before erasing its sector
            if ( aboutToInvalidate != INVALID_PAGE_ADDRESS )
            {
                size_t aboutToInvalidateSector = aboutToInvalidate - ( aboutToInvalidate % sectorSize );
                if ( aboutToInvalidateSector == sectorAddr )
                {
                    markPageInvalid( aboutToInvalidate );
                }
            }

            // Erase this sector
            if ( m_flashDriver.eraseSector( sectorAddr ) )
            {
                m_eraseCount++;
                return static_cast<uint32_t>( sectorAddr ); // Return first slot
            }
        }
    }

    // No space available
    return INVALID_PAGE_ADDRESS;
}


//////////////////////////////////////////////////////////////////////////////
template <size_t MAX_LOGICAL_PAGES>
bool Api<MAX_LOGICAL_PAGES>::markPageInvalid( uint32_t pageAddress ) noexcept
{
    // Write INVALID status to the status field (offset 20 within the header)
    uint32_t invalidStatus = PageStatus_T::INVALID;
    size_t   statusOffset  = pageAddress + offsetof( PageHeader_T, status );
    return m_flashDriver.write( statusOffset, &invalidStatus, sizeof( invalidStatus ) );
}


//////////////////////////////////////////////////////////////////////////////
template <size_t MAX_LOGICAL_PAGES>
bool Api<MAX_LOGICAL_PAGES>::eraseSectorIfNeeded( size_t sectorAddress ) noexcept
{
    size_t sectorSize   = m_flashDriver.getSectorSize();
    size_t physPageSize  = getPhysicalPageSize();

    // Check all record slots in this sector
    for ( size_t slot = 0; slot < m_pagesPerSector; slot++ )
    {
        size_t slotAddr = sectorAddress + ( slot * physPageSize );
        PageHeader_T header;
        if ( !m_flashDriver.read( slotAddr, &header, HEADER_SIZE ) )
        {
            return false;
        }

        if ( header.magic == MAGIC_NUMBER && header.status == PageStatus_T::VALID )
        {
            return false; // Cannot erase - sector has valid data
        }
    }

    // No valid records - safe to erase
    if ( m_flashDriver.eraseSector( sectorAddress ) )
    {
        m_eraseCount++;
        return true;
    }

    return false;
}


//////////////////////////////////////////////////////////////////////////////
template <size_t MAX_LOGICAL_PAGES>
bool Api<MAX_LOGICAL_PAGES>::readCurrentPageData( size_t   pageIndex,
                                                  uint8_t* buffer ) noexcept
{
    if ( pageIndex >= m_numLogicalPages )
    {
        return false;
    }

    if ( m_pageMap[pageIndex] != INVALID_PAGE_ADDRESS )
    {
        // Read existing data from flash
        size_t dataAddr = m_pageMap[pageIndex] + HEADER_SIZE;
        return m_flashDriver.read( dataAddr, buffer, m_config.nvPageSize );
    }
    else
    {
        // No valid data - fill with erased state
        memset( buffer, ERASED_BYTE_VALUE, m_config.nvPageSize );
        return true;
    }
}


//////////////////////////////////////////////////////////////////////////////
template <size_t MAX_LOGICAL_PAGES>
uint32_t Api<MAX_LOGICAL_PAGES>::calculateCrc32( const void* data,
                                                  size_t      length ) noexcept
{
    // Standard CRC32 (ISO 3309 / ITU-T V.42) implementation
    const uint8_t* bytes = static_cast<const uint8_t*>( data );
    uint32_t       crc   = 0xFFFFFFFF;

    for ( size_t i = 0; i < length; i++ )
    {
        crc ^= bytes[i];
        for ( int bit = 0; bit < 8; bit++ )
        {
            if ( crc & 1 )
            {
                crc = ( crc >> 1 ) ^ CRC32_POLYNOMIAL;
            }
            else
            {
                crc = crc >> 1;
            }
        }
    }

    return ~crc;
}


//////////////////////////////////////////////////////////////////////////////
template <size_t MAX_LOGICAL_PAGES>
size_t Api<MAX_LOGICAL_PAGES>::getPhysicalPageSize() const noexcept
{
    return HEADER_SIZE + m_config.nvPageSize;
}

template <size_t MAX_LOGICAL_PAGES>
size_t Api<MAX_LOGICAL_PAGES>::offsetToPageIndex( size_t offset ) const noexcept
{
    return offset / m_config.nvPageSize;
}


}  // end namespaces
}
}
}
#endif  // end header latch
