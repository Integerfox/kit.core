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


#include "kit_config.h"
#include "Kit/Driver/NV/IApi.h"
#include "Kit/Driver/Flash/IApi.h"

///
namespace Kit {
///
namespace Driver {
///
namespace NV {
///
namespace Flash {


/** Page header structure used for wear leveling and data validity tracking.
    
    The header is placed at the beginning of each logical NV page within flash.
    Pages cycle through states: ERASED -> VALID -> INVALID
 */
struct PageHeader_T
{
    uint32_t magic;         ///< Magic number to identify valid header (0xA5A5A5A5)
    uint32_t sequenceNum;   ///< Monotonically increasing sequence number
    uint32_t dataOffset;    ///< Starting offset of data within NV storage
    uint32_t dataLength;    ///< Length of data in this page
    uint32_t crc32;         ///< CRC32 of header fields (excluding crc32 itself)
    uint32_t status;        ///< Page status: ERASED=0xFFFFFFFF, VALID=0x00000000, INVALID=0x55555555
};

/** Page status values */
struct PageStatus_T
{
    static constexpr uint32_t ERASED  = 0xFFFFFFFF;  ///< Page is erased and available
    static constexpr uint32_t VALID   = 0x00000000;  ///< Page contains valid data
    static constexpr uint32_t INVALID = 0x55555555;  ///< Page data is obsolete
};

/** Magic number for valid page headers */
static constexpr uint32_t PAGE_MAGIC = 0xA5A5A5A5;


/** Configuration structure for flash-based NV storage */
struct Config_T
{
    uint32_t flashStartAddress;   ///< Starting address in flash for NV storage
    uint32_t flashEndAddress;     ///< Ending address in flash for NV storage
    size_t   nvPageSize;          ///< Logical NV page size (data portion)
    size_t   nvTotalSize;         ///< Total NV storage size presented to user
};


/** This concrete class implements the NV (Non-Volatile storage) driver interface
    using flash memory as the underlying storage medium. This provides EEPROM-like
    semantics on flash by implementing wear leveling.

    ## Wear Leveling Strategy

    The implementation uses a simple but effective page-based wear leveling:

    1. The flash region is divided into logical pages, each containing:
       - A header (PageHeader_T) with sequence number and validity info
       - User data

    2. When writing:
       - Find the current valid page(s) containing the affected data
       - Read existing data and merge with new data
       - Write merged data to next free page
       - Mark old page as invalid

    3. When a sector is full (no free pages), erase and reuse

    ## Configuration

    The user must provide:
    - A flash driver instance
    - Start and end addresses for NV storage region
    - Logical NV page size (should be sized for typical access patterns)
    - Total NV size to present to users

    ## Thread Safety

    This implementation is NOT thread safe. The application must provide
    appropriate synchronization if accessed from multiple threads.

    ## Usage Example
    ```cpp
    // Flash driver (platform specific)
    Kit::Driver::Flash::W25Q::Api flash( spi, csPin, Kit::Driver::Flash::W25Q::Devices::W25Q128 );

    // NV configuration: use 128KB of flash for 4KB of NV storage
    Kit::Driver::NV::Flash::Config_T nvConfig = {
        .flashStartAddress = 0x000000,
        .flashEndAddress   = 0x020000,  // 128KB
        .nvPageSize        = 256,       // 256 bytes per logical page
        .nvTotalSize       = 4096       // 4KB presented to user
    };

    // Create NV driver
    Kit::Driver::NV::Flash::Api nv( flash, nvConfig );

    // Use like EEPROM
    nv.start();
    
    uint8_t data[] = { 1, 2, 3, 4 };
    nv.write( 0, data, sizeof(data) );
    
    uint8_t readBack[4];
    nv.read( 0, readBack, sizeof(readBack) );
    ```
 */
class Api : public Kit::Driver::NV::IApi
{
public:
    /** Constructor.

        @param flashDriver  Reference to the underlying flash driver
        @param config       Configuration for the NV storage region
     */
    Api( Kit::Driver::Flash::IApi& flashDriver,
         const Config_T&           config ) noexcept;


public:
    /// See Kit::Driver::IStart
    bool start( void* startArgs = nullptr ) noexcept override;

    /// See Kit::Driver::IStop
    void stop() noexcept override;


public:
    /// See Kit::Driver::NV::IApi
    bool write( size_t      dstOffset,
                const void* srcData,
                size_t      numBytesToWrite ) noexcept override;

    /// See Kit::Driver::NV::IApi
    bool read( size_t srcOffset,
               void*  dstData,
               size_t numBytesToRead ) noexcept override;

    /// See Kit::Driver::NV::IApi
    size_t getTotalSize() const noexcept override;

    /// See Kit::Driver::NV::IApi
    size_t getNumPages() const noexcept override;

    /// See Kit::Driver::NV::IApi
    size_t getPageSize() const noexcept override;


public:
    /** This method forces an erase of all NV storage and reinitializes.
        All stored data will be lost.

        @return true if successful, false otherwise
     */
    bool format() noexcept;

    /** This method returns statistics about the NV storage.

        @param eraseCount       Output: Total sector erase count
        @param freePages        Output: Number of free pages available
        @param validPages       Output: Number of pages with valid data

        @return true if successful, false otherwise
     */
    bool getStatistics( uint32_t& eraseCount,
                        size_t&   freePages,
                        size_t&   validPages ) noexcept;


protected:
    /** Scans flash to find current valid data and build page map */
    bool scanAndBuildPageMap() noexcept;

    /** Finds next free page for writing, erasing a sector if needed */
    uint32_t findFreePageAddress() noexcept;

    /** Marks a page as invalid */
    bool markPageInvalid( uint32_t pageAddress ) noexcept;

    /** Erases a sector containing the given address */
    bool eraseSectorIfNeeded( uint32_t address ) noexcept;

    /** Calculates CRC32 of a data block */
    uint32_t calculateCrc32( const void* data, size_t length ) noexcept;

    /** Gets the flash physical page size for this page */
    size_t getPhysicalPageSize() const noexcept;

    /** Converts NV offset to internal page index */
    size_t offsetToPageIndex( size_t offset ) const noexcept;


protected:
    /// Reference to the underlying flash driver
    Kit::Driver::Flash::IApi& m_flashDriver;

    /// Configuration
    const Config_T& m_config;

    /// Flag indicating if driver is started
    bool m_isStarted;

    /// Current sequence number for new pages
    uint32_t m_currentSequence;

    /// Total erase count (for statistics)
    uint32_t m_eraseCount;

    /// Number of flash sectors in NV region
    size_t m_numSectors;

    /// Number of logical pages per flash sector
    size_t m_pagesPerSector;

    /// Total number of logical pages
    size_t m_totalPages;


private:
    /// Prevent access to the copy constructor
    Api( const Api& ) = delete;

    /// Prevent access to the assignment operator
    const Api& operator=( const Api& ) = delete;
};


};  // end namespaces
};
};
};
#endif  // end header latch
