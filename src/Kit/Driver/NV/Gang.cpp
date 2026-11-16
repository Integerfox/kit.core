/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Driver/NV/Gang.h"
#include "Kit/System/Assert.h"


//------------------------------------------------------------------------------
namespace Kit {
namespace Driver {
namespace NV {

//////////////////////////////////////////////////////////////////////////////
Gang::Gang( IApi* drivers[], unsigned numDrivers ) noexcept
    : m_drivers( drivers )
    , m_totalSize( 0 )
    , m_numDrivers( numDrivers )
    , m_started( false )
{
    // Note: The constructor does NOT access the drivers (they may not be constructed yet)
    KIT_SYSTEM_ASSERT( drivers != nullptr );
    KIT_SYSTEM_ASSERT( numDrivers > 0 );
    for ( unsigned i = 0; drivers != nullptr && i < numDrivers; i++ )
    {
        KIT_SYSTEM_ASSERT( drivers[i] != nullptr );
    }
}

bool Gang::start( void* startArgs ) noexcept
{
    // Skip processing if already started
    if ( !m_started )
    {
        // Start the individual drivers
        for ( unsigned i = 0; i < m_numDrivers; i++ )
        {
            if ( !m_drivers[i]->start( startArgs ) )
            {
                // Roll back: stop the drivers that were already started
                while ( i > 0 )
                {
                    i--;
                    m_drivers[i]->stop();
                }
                return false;
            }
        }

        // Calculate the total size (done after the start in case the drivers only know their size once started)
        m_totalSize = 0;
        for ( unsigned i = 0; i < m_numDrivers; i++ )
        {
            m_totalSize += m_drivers[i]->getTotalSize();
        }

        m_started = true;
        return true;
    }

    return true;
}

void Gang::stop() noexcept
{
    // Skip processing if already stopped
    if ( m_started )
    {
        m_started   = false;
        m_totalSize = 0;

        // Stop the individual drivers
        for ( unsigned i = 0; i < m_numDrivers; i++ )
        {
            m_drivers[i]->stop();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
bool Gang::write( size_t dstOffset, const void* srcData, size_t numBytesToWrite ) noexcept
{
    // Fail if not started or write is out of range
    if ( !m_started ||
         numBytesToWrite > m_totalSize ||
         dstOffset > ( m_totalSize - numBytesToWrite ) )
    {
        return false;
    }

    // Zero-length transfer: nothing to do (the range check above already allows 0 <= dstOffset <= m_totalSize)
    if ( numBytesToWrite == 0 )
    {
        return true;
    }

    // Find the starting driver
    size_t endOffset   = 0;
    size_t beginOffset = 0;
    for ( unsigned i = 0; i < m_numDrivers; i++ )
    {
        // Find the 'starting' page/driver
        size_t driverTotalSize  = m_drivers[i]->getTotalSize();
        endOffset              += driverTotalSize;
        if ( dstOffset < endOffset )
        {
            // Normalize the dstOffset and write the data
            dstOffset -= beginOffset;

            // Loop through as many drivers as needed to complete the write
            const uint8_t* srcPtr = static_cast<const uint8_t*>( srcData );
            while ( numBytesToWrite && i < m_numDrivers )
            {
                bool result = driverWrite( *m_drivers[i], dstOffset, srcPtr, numBytesToWrite );
                if ( !result )
                {
                    return false;
                }

                // Advance to the next driver
                i++;
                dstOffset = 0;
            }

            // Note: numBytesToWrite should always be zero.  But if some how we
            // ran out of drivers before the write completed ->fail the
            // transaction
            return numBytesToWrite == 0;
        }

        // Next driver
        beginOffset += driverTotalSize;
    }

    // If get here - no byte were written
    return false;
}

bool Gang::driverWrite( IApi&           driver,
                        size_t          normalizedDstOffset,
                        const uint8_t*& srcData,
                        size_t&         numBytesToWrite ) noexcept
{
    // Start writing bytes using the current driver
    size_t bytesThisDriver = numBytesToWrite;
    if ( normalizedDstOffset + numBytesToWrite > driver.getTotalSize() )
    {
        bytesThisDriver = driver.getTotalSize() - normalizedDstOffset;
        numBytesToWrite = numBytesToWrite - bytesThisDriver;
    }
    else
    {
        numBytesToWrite = 0;
    }
    bool result  = driver.write( normalizedDstOffset, srcData, bytesThisDriver );
    srcData     += bytesThisDriver;
    return result;
}

//////////////////////////////////////////////////////////////////////////////
bool Gang::read( size_t srcOffset, void* dstData, size_t sizeDstData, size_t numBytesToRead ) noexcept
{
    // Fail if not started, the destination buffer is too small, or read is out of range
    if ( !m_started ||
         sizeDstData < numBytesToRead ||
         numBytesToRead > m_totalSize ||
         srcOffset > ( m_totalSize - numBytesToRead ) )
    {
        return false;
    }

    // Zero-length transfer: nothing to do (the range check above already allows 0 <= srcOffset <= m_totalSize)
    if ( numBytesToRead == 0 )
    {
        return true;
    }

    // Find the starting driver
    size_t endOffset   = 0;
    size_t beginOffset = 0;
    for ( unsigned i = 0; i < m_numDrivers; i++ )
    {
        // Find the 'starting' page/driver
        size_t driverTotalSize  = m_drivers[i]->getTotalSize();
        endOffset              += driverTotalSize;
        if ( srcOffset < endOffset )
        {
            // Normalize the srcOffset and read the data
            srcOffset -= beginOffset;

            // Loop through as many drivers as needed to complete the read
            uint8_t* dstPtr = static_cast<uint8_t*>( dstData );
            while ( numBytesToRead && i < m_numDrivers )
            {
                bool result = driverRead( *m_drivers[i], srcOffset, dstPtr, numBytesToRead );
                if ( !result )
                {
                    return false;
                }

                // Advance to the next driver
                i++;
                srcOffset = 0;
            }

            // Note: numBytesToRead should always be zero.  But if some how we
            // ran out of drivers before the read completed ->fail the
            // transaction
            return numBytesToRead == 0;
        }

        // Next driver
        beginOffset += driverTotalSize;
    }

    // If get here - no bytes were read
    return false;
}

bool Gang::driverRead( IApi&     driver,
                       size_t    normalizedSrcOffset,
                       uint8_t*& dstData,
                       size_t&   numBytesToRead ) noexcept
{
    // Starting reading the data from the current driver
    size_t bytesThisDriver = numBytesToRead;
    if ( normalizedSrcOffset + numBytesToRead > driver.getTotalSize() )
    {
        bytesThisDriver = driver.getTotalSize() - normalizedSrcOffset;
        numBytesToRead  = numBytesToRead - bytesThisDriver;
    }
    else
    {
        numBytesToRead = 0;
    }
    bool result  = driver.read( normalizedSrcOffset, dstData, bytesThisDriver, bytesThisDriver );
    dstData     += bytesThisDriver;
    return result;
}


//////////////////////////////////////////////////////////////////////////////
size_t Gang::getNumPages() const noexcept
{
    if ( !m_started )
    {
        return 0;
    }

    size_t pageSize = getPageSize();
    return pageSize != 0 ? m_totalSize / pageSize : 0;
}

size_t Gang::getPageSize() const noexcept
{
    if ( !m_started )
    {
        return 0;
    }

    // Trap the smallest page size
    size_t pageSize = m_drivers[0]->getPageSize();
    for ( unsigned i = 1; i < m_numDrivers; i++ )
    {
        size_t driverPageSize = m_drivers[i]->getPageSize();
        if ( driverPageSize < pageSize )
        {
            pageSize = driverPageSize;
        }
    }

    return pageSize;
}

size_t Gang::getTotalSize() const noexcept
{
    return m_totalSize;  // NOTE: m_totalSize will be zero when the driver is in the stopped state
}

}  // end namespace
}
}
//------------------------------------------------------------------------------
