#ifndef KIT_DRIVER_FLASH_TESTSUPPORT_MOCKFLASH_H_
#define KIT_DRIVER_FLASH_TESTSUPPORT_MOCKFLASH_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file
    Mock Flash driver for unit testing.  Simulates a flash device in RAM
    with proper flash semantics:
    - Erased state is 0xFF
    - Writes can only clear bits (1->0), never set bits
    - Erase operations set all bytes in a sector to 0xFF

    Template parameter TOTAL_SIZE specifies the simulated flash size in bytes.
*/


#include "Kit/Driver/Flash/IApi.h"
#include <stdint.h>
#include <string.h>


///
namespace Kit {
///
namespace Driver {
///
namespace Flash {
///
namespace TestSupport {


/** Mock flash driver that simulates flash memory in RAM.
    Template parameter TOTAL_SIZE specifies the total flash size,
    SECTOR_SIZE specifies the erase sector size, and PAGE_SIZE
    specifies the write page size.
 */
template <size_t TOTAL_SIZE, size_t SECTOR_SIZE = 4096, size_t PAGE_SIZE = 256>
class MockFlash : public IApi
{
public:
    MockFlash() noexcept
        : m_started( false )
        , m_failNext( false )
        , m_readCount( 0 )
        , m_writeCount( 0 )
        , m_eraseCount( 0 )
    {
        // Initialize to erased state
        memset( m_flash, 0xFF, sizeof( m_flash ) );
    }


public:
    bool start( void* startArgs = nullptr ) noexcept override
    {
        m_started = true;
        return true;
    }

    void stop() noexcept override
    {
        m_started = false;
    }


public:
    bool read( size_t srcOffset,
               void*  dstBuffer,
               size_t numBytes ) noexcept override
    {
        if ( m_failNext )
        {
            m_failNext = false;
            return false;
        }

        if ( dstBuffer == nullptr || numBytes == 0 )
        {
            return false;
        }

        if ( srcOffset + numBytes > TOTAL_SIZE )
        {
            return false;
        }

        memcpy( dstBuffer, m_flash + srcOffset, numBytes );
        m_readCount++;
        return true;
    }

    bool write( size_t      dstOffset,
                const void* srcBuffer,
                size_t      numBytes ) noexcept override
    {
        if ( m_failNext )
        {
            m_failNext = false;
            return false;
        }

        if ( srcBuffer == nullptr || numBytes == 0 )
        {
            return false;
        }

        if ( dstOffset + numBytes > TOTAL_SIZE )
        {
            return false;
        }

        // Simulate flash write semantics: can only clear bits (AND operation)
        const uint8_t* src = static_cast<const uint8_t*>( srcBuffer );
        for ( size_t i = 0; i < numBytes; i++ )
        {
            m_flash[dstOffset + i] &= src[i];
        }

        m_writeCount++;
        return true;
    }

    bool eraseSector( size_t sectorAddress ) noexcept override
    {
        if ( m_failNext )
        {
            m_failNext = false;
            return false;
        }

        if ( sectorAddress + SECTOR_SIZE > TOTAL_SIZE )
        {
            return false;
        }

        // Align to sector boundary
        size_t aligned = sectorAddress - ( sectorAddress % SECTOR_SIZE );
        memset( m_flash + aligned, 0xFF, SECTOR_SIZE );
        m_eraseCount++;
        return true;
    }

    bool eraseBlock32K( size_t blockAddress ) noexcept override
    {
        if ( m_failNext )
        {
            m_failNext = false;
            return false;
        }

        static constexpr size_t BLOCK_32K = 32 * 1024;
        if ( blockAddress + BLOCK_32K > TOTAL_SIZE )
        {
            return false;
        }

        size_t aligned = blockAddress - ( blockAddress % BLOCK_32K );
        memset( m_flash + aligned, 0xFF, BLOCK_32K );
        m_eraseCount++;
        return true;
    }

    bool eraseBlock64K( size_t blockAddress ) noexcept override
    {
        if ( m_failNext )
        {
            m_failNext = false;
            return false;
        }

        static constexpr size_t BLOCK_64K = 64 * 1024;
        if ( blockAddress + BLOCK_64K > TOTAL_SIZE )
        {
            return false;
        }

        size_t aligned = blockAddress - ( blockAddress % BLOCK_64K );
        memset( m_flash + aligned, 0xFF, BLOCK_64K );
        m_eraseCount++;
        return true;
    }

    bool eraseChip() noexcept override
    {
        if ( m_failNext )
        {
            m_failNext = false;
            return false;
        }

        memset( m_flash, 0xFF, TOTAL_SIZE );
        m_eraseCount++;
        return true;
    }


public:
    size_t getTotalSize() const noexcept override { return TOTAL_SIZE; }
    size_t getSectorSize() const noexcept override { return SECTOR_SIZE; }
    size_t getPageSize() const noexcept override { return PAGE_SIZE; }
    size_t getNumSectors() const noexcept override { return TOTAL_SIZE / SECTOR_SIZE; }


public:
    /// Causes the next flash operation to fail
    void setFailNext() noexcept { m_failNext = true; }

    /// Resets the entire flash to erased state and clears counters
    void reset() noexcept
    {
        memset( m_flash, 0xFF, TOTAL_SIZE );
        m_failNext   = false;
        m_readCount  = 0;
        m_writeCount = 0;
        m_eraseCount = 0;
    }

    /// Direct access to flash memory for test verification (const)
    const uint8_t* getFlashMemory() const noexcept { return m_flash; }
    /// Direct access to flash memory for test verification (mutable)
    uint8_t* getFlashMemory() noexcept { return m_flash; }

    /// Returns the number of read operations performed
    uint32_t getReadCount() const noexcept { return m_readCount; }
    /// Returns the number of write operations performed
    uint32_t getWriteCount() const noexcept { return m_writeCount; }
    /// Returns the number of erase operations performed
    uint32_t getEraseCount() const noexcept { return m_eraseCount; }


protected:
    uint8_t  m_flash[TOTAL_SIZE];  //!< Simulated flash memory
    bool     m_started;            //!< Whether the driver has been started
    bool     m_failNext;           //!< If true, the next operation will fail
    uint32_t m_readCount;          //!< Number of read operations performed
    uint32_t m_writeCount;         //!< Number of write operations performed
    uint32_t m_eraseCount;         //!< Number of erase operations performed
};


}  // end namespaces
}
}
}
#endif  // end header latch
