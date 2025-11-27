/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "ILog.h"

#define NUM_BITS(type) ( sizeof(type) * 8 )

//------------------------------------------------------------------------------
namespace Kit {
namespace Logging {
namespace Framework {

KitLoggingClassificationMask_T ILog::classificationIdToMask( uint8_t classificationId ) noexcept
{
    if ( classificationId == 0 || classificationId > NUM_BITS(KitLoggingClassificationMask_T) )
    {
        return 0;
    }
    return ( (KitLoggingClassificationMask_T)1 ) << ( classificationId - 1 );
}

uint8_t ILog::maskToClassificationId( KitLoggingClassificationMask_T classificationMask ) noexcept
{
    for ( uint8_t i = 0; i < NUM_BITS(KitLoggingClassificationMask_T); ++i )
    {
        if ( classificationMask == ( ( (KitLoggingClassificationMask_T)1 ) << i ) )
        {
            return i + 1;
        }
    }
    return 0;
}

KitLoggingPackageMask_T ILog::packageIdToMask( uint8_t packageId ) noexcept
{
    if ( packageId == 0 || packageId > NUM_BITS(KitLoggingPackageMask_T) )
    {
        return 0;
    }
    return ( (KitLoggingPackageMask_T)1 ) << ( packageId - 1 );
}

uint8_t ILog::maskToPackageId( KitLoggingPackageMask_T packageMask ) noexcept
{
    for ( uint8_t i = 0; i < NUM_BITS(KitLoggingPackageMask_T); ++i )
    {
        if ( packageMask == ( ( (KitLoggingPackageMask_T)1 ) << i ) )
        {
            return i + 1;
        }
    }
    return 0;
}

}  // end namespace
}
}
//------------------------------------------------------------------------------