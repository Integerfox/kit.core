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
#include <stdint.h>

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
    static constexpr uint32_t ERASED  = 0xFFFFFFFF; //!< Record slot is empty
    static constexpr uint32_t VALID   = 0xFFFF0000; //!< Record contains current data
    static constexpr uint32_t INVALID = 0x00000000; //!< Record is obsolete
};


/** NV Record header structure (24 bytes).  Stored at the beginning of each
    NV Record in flash.
 */
struct PageHeader_T
{
    uint32_t magic;       //!< Magic number (0xA5A5A5A5) identifying a valid header
    uint32_t sequenceNum; //!< Monotonically increasing sequence number
    uint32_t dataOffset;  //!< Starting offset of this record within the logical NV space
    uint32_t dataLength;  //!< Length of the data payload (equals nvPageSize)
    uint32_t crc32;       //!< CRC32 over the preceding 16 bytes (magic through dataLength)
    uint32_t status;      //!< Record lifecycle state (ERASED, VALID, or INVALID)
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

/** Maximum supported NV page size (stack buffer limit) */
static constexpr size_t MAX_NV_PAGE_SIZE = 512;

/** CRC32 polynomial (ISO 3309 / ITU-T V.42) */
static constexpr uint32_t CRC32_POLYNOMIAL = 0xEDB88320;


/** Configuration structure for the NV flash driver */
struct Config_T
{
    uint32_t flashStartAddress; //!< Base address of the NV region in flash
    uint32_t flashEndAddress;   //!< End address (exclusive) of the NV region in flash
    size_t   nvPageSize;        //!< Logical NV page size (data portion only, e.g. 256 bytes)
    size_t   nvTotalSize;       //!< Total NV storage size presented to the application
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
    /** Constructor.  The 'flashDriver' reference must remain valid for the
        lifetime of this object.
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
               size_t sizeDstData,
               size_t numBytesToRead ) noexcept override;

    /// See Kit::Driver::NV::IApi
    size_t getTotalSize() const noexcept override;

    /// See Kit::Driver::NV::IApi
    size_t getNumPages() const noexcept override;

    /// See Kit::Driver::NV::IApi
    size_t getPageSize() const noexcept override;


public:
    /** Erases all sectors in the NV region and resets the driver state.
        This is equivalent to a factory reset of NV storage.

        Returns true on success; false on failure.
     */
    bool format() noexcept;

    /** Retrieves driver statistics.
        @param eraseCount [out] Total number of sector erase operations performed
        @param freePages  [out] Number of currently free (erased) record slots
        @param validPages [out] Number of currently valid record slots
        Returns true on success.
     */
    bool getStatistics( uint32_t& eraseCount,
                        size_t&   freePages,
                        size_t&   validPages ) noexcept;


protected:
    /// Scans flash and builds the in-memory page map.  Returns true on success.
    bool scanAndBuildPageMap() noexcept;

    /// Finds and returns the physical address of a free (erased) record slot.
    /// 'aboutToInvalidate' is the address of a record that will be marked INVALID
    /// after this write, allowing its sector to be considered for reclamation.
    /// Returns INVALID_PAGE_ADDRESS if no free slot is available.
    uint32_t findFreePageAddress( uint32_t aboutToInvalidate = INVALID_PAGE_ADDRESS ) noexcept;

    /// Marks the NV Record at the given address as INVALID.
    bool markPageInvalid( uint32_t pageAddress ) noexcept;

    /// Erases the sector containing the given address if it has no VALID records.
    bool eraseSectorIfNeeded( size_t sectorAddress ) noexcept;

    /// Reads the current data for a logical page into 'buffer'.
    /// If no valid data exists, fills the buffer with 0xFF.
    bool readCurrentPageData( size_t pageIndex, uint8_t* buffer ) noexcept;

    /// Calculates a CRC32 over the given data.
    static uint32_t calculateCrc32( const void* data, size_t length ) noexcept;

    /// Returns the total physical NV Record size (header + data)
    size_t getPhysicalPageSize() const noexcept;

    /// Converts a logical NV offset to a logical page index
    size_t offsetToPageIndex( size_t offset ) const noexcept;


protected:
    Kit::Driver::Flash::IApi& m_flashDriver; //!< Reference to flash driver
    Config_T        m_config;           //!< Storage configuration
    bool            m_isStarted;        //!< Driver state flag
    uint32_t        m_currentSequence;  //!< Highest sequence number seen
    uint32_t        m_eraseCount;       //!< Total erase count
    size_t          m_numSectors;       //!< Number of sectors in NV region
    size_t          m_pagesPerSector;   //!< Physical NV Records per sector
    size_t          m_totalPages;       //!< Total physical NV Record slots
    size_t          m_numLogicalPages;  //!< Number of logical pages
    uint32_t        m_pageMap[MAX_LOGICAL_PAGES]; //!< Logical page -> physical address map
};


}  // end namespaces
}
}
}

// Include the template implementation
#include "Api_.h"

#endif  // end header latch
