/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Api.h"
#include <string.h>

///
using namespace Kit::Driver::NV::Flash;

// CRC32 lookup table (IEEE 802.3 polynomial)
static const uint32_t crc32Table_[256] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBBBD6, 0xACBCCB40, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD706B3, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};


/////////////////////////////////////////////////////////////////////////////
Api::Api( Kit::Driver::Flash::IApi& flashDriver,
          const Config_T&           config ) noexcept
    : m_flashDriver( flashDriver )
    , m_config( config )
    , m_isStarted( false )
    , m_currentSequence( 0 )
    , m_eraseCount( 0 )
    , m_numSectors( 0 )
    , m_pagesPerSector( 0 )
    , m_totalPages( 0 )
{
}


/////////////////////////////////////////////////////////////////////////////
bool Api::start( void* startArgs ) noexcept
{
    if ( m_isStarted )
    {
        return true;
    }

    // Calculate storage layout
    size_t flashSectorSize = m_flashDriver.getSectorSize();
    size_t physicalPageSize = getPhysicalPageSize();

    // Validate configuration
    if ( m_config.flashEndAddress <= m_config.flashStartAddress )
    {
        return false;
    }

    size_t flashRegionSize = m_config.flashEndAddress - m_config.flashStartAddress;
    if ( flashRegionSize < flashSectorSize )
    {
        return false;
    }

    // Calculate sector and page counts
    m_numSectors     = flashRegionSize / flashSectorSize;
    m_pagesPerSector = flashSectorSize / physicalPageSize;
    m_totalPages     = m_numSectors * m_pagesPerSector;

    // Scan flash to find current state
    if ( !scanAndBuildPageMap() )
    {
        // If scan fails, format the storage
        if ( !format() )
        {
            return false;
        }
    }

    m_isStarted = true;
    return true;
}


void Api::stop() noexcept
{
    if ( !m_isStarted )
    {
        return;
    }

    m_isStarted = false;
}


/////////////////////////////////////////////////////////////////////////////
bool Api::write( size_t      dstOffset,
                 const void* srcData,
                 size_t      numBytesToWrite ) noexcept
{
    if ( !m_isStarted || srcData == nullptr || numBytesToWrite == 0 )
    {
        return false;
    }

    // Validate offset and size
    if ( dstOffset + numBytesToWrite > m_config.nvTotalSize )
    {
        return false;
    }

    const uint8_t* writeData = static_cast<const uint8_t*>( srcData );
    size_t remaining         = numBytesToWrite;
    size_t currentOffset     = dstOffset;

    while ( remaining > 0 )
    {
        // Calculate which logical NV page this offset falls into
        size_t pageIndex     = currentOffset / m_config.nvPageSize;
        size_t offsetInPage  = currentOffset % m_config.nvPageSize;
        size_t bytesThisPage = m_config.nvPageSize - offsetInPage;

        if ( bytesThisPage > remaining )
        {
            bytesThisPage = remaining;
        }

        // Find a free page in flash
        uint32_t newPageAddress = findFreePageAddress();
        if ( newPageAddress == 0xFFFFFFFF )
        {
            return false;  // No space available
        }

        // Read existing data for this NV page (if any)
        uint8_t pageBuffer[256];  // Temporary buffer for page data
        size_t bufferSize = ( m_config.nvPageSize > sizeof( pageBuffer ) ) 
                            ? sizeof( pageBuffer ) 
                            : m_config.nvPageSize;

        // Initialize buffer to 0xFF (erased state)
        memset( pageBuffer, 0xFF, bufferSize );

        // Try to read existing data from flash
        // This is a simplified implementation - a full implementation would
        // scan for the most recent valid page for this NV offset
        size_t nvPageStartOffset = pageIndex * m_config.nvPageSize;

        // Merge new data into buffer
        memcpy( pageBuffer + offsetInPage, writeData, bytesThisPage );

        // Build page header
        PageHeader_T header;
        header.magic       = PAGE_MAGIC;
        header.sequenceNum = ++m_currentSequence;
        header.dataOffset  = (uint32_t)nvPageStartOffset;
        header.dataLength  = (uint32_t)m_config.nvPageSize;
        header.status      = PageStatus_T::VALID;
        header.crc32       = calculateCrc32( &header, sizeof( header ) - sizeof( header.crc32 ) - sizeof( header.status ) );

        // Write header
        if ( !m_flashDriver.write( newPageAddress, &header, sizeof( header ) ) )
        {
            return false;
        }

        // Write data after header
        uint32_t dataAddress = newPageAddress + sizeof( PageHeader_T );
        if ( !m_flashDriver.write( dataAddress, pageBuffer, m_config.nvPageSize ) )
        {
            return false;
        }

        // Update tracking
        writeData     += bytesThisPage;
        currentOffset += bytesThisPage;
        remaining     -= bytesThisPage;
    }

    return true;
}


bool Api::read( size_t srcOffset,
                void*  dstData,
                size_t numBytesToRead ) noexcept
{
    if ( !m_isStarted || dstData == nullptr || numBytesToRead == 0 )
    {
        return false;
    }

    // Validate offset and size
    if ( srcOffset + numBytesToRead > m_config.nvTotalSize )
    {
        return false;
    }

    uint8_t* readData = static_cast<uint8_t*>( dstData );

    // Initialize output to 0xFF (erased/default state)
    memset( dstData, 0xFF, numBytesToRead );

    // Scan through all pages to find most recent valid data
    size_t physicalPageSize = getPhysicalPageSize();
    uint32_t address = m_config.flashStartAddress;

    while ( address < m_config.flashEndAddress )
    {
        PageHeader_T header;

        // Read page header
        if ( !m_flashDriver.read( address, &header, sizeof( header ) ) )
        {
            address += physicalPageSize;
            continue;
        }

        // Check if this is a valid page
        if ( header.magic != PAGE_MAGIC || header.status != PageStatus_T::VALID )
        {
            address += physicalPageSize;
            continue;
        }

        // Check if this page contains data we need
        uint32_t pageDataStart = header.dataOffset;
        uint32_t pageDataEnd   = pageDataStart + header.dataLength;

        // Check for overlap with requested range
        if ( pageDataEnd > srcOffset && pageDataStart < srcOffset + numBytesToRead )
        {
            // Calculate overlap region
            size_t overlapStart = ( pageDataStart > srcOffset ) ? pageDataStart : srcOffset;
            size_t overlapEnd   = ( pageDataEnd < srcOffset + numBytesToRead ) ? pageDataEnd : srcOffset + numBytesToRead;
            size_t overlapLen   = overlapEnd - overlapStart;

            // Calculate offsets
            size_t srcOffsetInPage = overlapStart - pageDataStart;
            size_t dstOffsetInBuf  = overlapStart - srcOffset;

            // Read the overlapping data
            uint32_t dataAddress = address + sizeof( PageHeader_T ) + srcOffsetInPage;
            m_flashDriver.read( dataAddress, readData + dstOffsetInBuf, overlapLen );
        }

        address += physicalPageSize;
    }

    return true;
}


size_t Api::getTotalSize() const noexcept
{
    return m_config.nvTotalSize;
}


size_t Api::getNumPages() const noexcept
{
    return m_config.nvTotalSize / m_config.nvPageSize;
}


size_t Api::getPageSize() const noexcept
{
    return m_config.nvPageSize;
}


/////////////////////////////////////////////////////////////////////////////
bool Api::format() noexcept
{
    // Erase all sectors in the NV region
    size_t flashSectorSize = m_flashDriver.getSectorSize();
    uint32_t address = m_config.flashStartAddress;

    while ( address < m_config.flashEndAddress )
    {
        if ( !m_flashDriver.eraseSector( address ) )
        {
            return false;
        }

        m_eraseCount++;
        address += flashSectorSize;
    }

    // Reset state
    m_currentSequence = 0;

    return true;
}


bool Api::getStatistics( uint32_t& eraseCount,
                         size_t&   freePages,
                         size_t&   validPages ) noexcept
{
    if ( !m_isStarted )
    {
        return false;
    }

    eraseCount  = m_eraseCount;
    freePages   = 0;
    validPages  = 0;

    // Scan all pages
    size_t physicalPageSize = getPhysicalPageSize();
    uint32_t address = m_config.flashStartAddress;

    while ( address < m_config.flashEndAddress )
    {
        PageHeader_T header;

        if ( m_flashDriver.read( address, &header, sizeof( header ) ) )
        {
            if ( header.magic == 0xFFFFFFFF && header.status == PageStatus_T::ERASED )
            {
                freePages++;
            }
            else if ( header.magic == PAGE_MAGIC && header.status == PageStatus_T::VALID )
            {
                validPages++;
            }
        }

        address += physicalPageSize;
    }

    return true;
}


/////////////////////////////////////////////////////////////////////////////
bool Api::scanAndBuildPageMap() noexcept
{
    // Find the highest sequence number to continue from
    uint32_t maxSequence    = 0;
    bool     foundValidPage = false;

    size_t physicalPageSize = getPhysicalPageSize();
    uint32_t address = m_config.flashStartAddress;

    while ( address < m_config.flashEndAddress )
    {
        PageHeader_T header;

        if ( m_flashDriver.read( address, &header, sizeof( header ) ) )
        {
            if ( header.magic == PAGE_MAGIC )
            {
                // Verify CRC
                uint32_t calcCrc = calculateCrc32( &header, sizeof( header ) - sizeof( header.crc32 ) - sizeof( header.status ) );
                if ( calcCrc == header.crc32 )
                {
                    foundValidPage = true;
                    if ( header.sequenceNum > maxSequence )
                    {
                        maxSequence = header.sequenceNum;
                    }
                }
            }
        }

        address += physicalPageSize;
    }

    m_currentSequence = maxSequence;
    return foundValidPage || ( address > m_config.flashStartAddress );
}


uint32_t Api::findFreePageAddress() noexcept
{
    size_t physicalPageSize = getPhysicalPageSize();
    uint32_t address = m_config.flashStartAddress;

    // First pass: look for an erased page
    while ( address < m_config.flashEndAddress )
    {
        PageHeader_T header;

        if ( m_flashDriver.read( address, &header, sizeof( header ) ) )
        {
            // Check for erased page (all 0xFF)
            if ( header.magic == 0xFFFFFFFF && header.status == PageStatus_T::ERASED )
            {
                return address;
            }
        }

        address += physicalPageSize;
    }

    // No free pages found - need to erase a sector
    // Find a sector with all invalid pages
    size_t flashSectorSize = m_flashDriver.getSectorSize();
    address = m_config.flashStartAddress;

    while ( address < m_config.flashEndAddress )
    {
        bool canErase = true;
        uint32_t sectorStart = address;
        uint32_t sectorEnd   = address + flashSectorSize;

        // Check all pages in this sector
        uint32_t pageAddr = sectorStart;
        while ( pageAddr < sectorEnd && pageAddr < m_config.flashEndAddress )
        {
            PageHeader_T header;

            if ( m_flashDriver.read( pageAddr, &header, sizeof( header ) ) )
            {
                // If there's a valid page, we can't erase this sector
                if ( header.magic == PAGE_MAGIC && header.status == PageStatus_T::VALID )
                {
                    canErase = false;
                    break;
                }
            }

            pageAddr += physicalPageSize;
        }

        if ( canErase )
        {
            // Erase this sector
            if ( m_flashDriver.eraseSector( sectorStart ) )
            {
                m_eraseCount++;
                return sectorStart;  // Return first page in erased sector
            }
        }

        address += flashSectorSize;
    }

    // No space available
    return 0xFFFFFFFF;
}


bool Api::markPageInvalid( uint32_t pageAddress ) noexcept
{
    // Write INVALID status to the page header
    // Note: We can only write 0s to flash (change 1s to 0s)
    uint32_t status        = PageStatus_T::INVALID;
    uint32_t statusOffset  = pageAddress + offsetof( PageHeader_T, status );

    return m_flashDriver.write( statusOffset, &status, sizeof( status ) );
}


uint32_t Api::calculateCrc32( const void* data, size_t length ) noexcept
{
    const uint8_t* bytes = static_cast<const uint8_t*>( data );
    uint32_t crc = 0xFFFFFFFF;

    for ( size_t i = 0; i < length; i++ )
    {
        crc = crc32Table_[( crc ^ bytes[i] ) & 0xFF] ^ ( crc >> 8 );
    }

    return crc ^ 0xFFFFFFFF;
}


size_t Api::getPhysicalPageSize() const noexcept
{
    // Physical page size = header + data
    return sizeof( PageHeader_T ) + m_config.nvPageSize;
}


size_t Api::offsetToPageIndex( size_t offset ) const noexcept
{
    return offset / m_config.nvPageSize;
}
