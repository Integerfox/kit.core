#ifndef KIT_TEXT_TO_STRING_H_
#define KIT_TEXT_TO_STRING_H_
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
#include <stdlib.h>
#include <stdint.h>

/** Data type of largest unsigned integer type supported by the platform.  This
    type is 'base' used for conversions to strings.
    Default: 32bit platform
 */
#ifndef KitTextToStringMaxUnsigned_T
#define KitTextToStringMaxUnsigned_T uint32_t
#endif

///
namespace Kit {
///
namespace Text {

/** This static class is used to convert integer values to a null terminated
    string WITHOUT using snprintf() function calls. This is for platforms/situations
    where it is not possible or undesirable (i.e. stack usage) to call snprintf.

    Note: When using the template methods - do NOT use a data type larger than 
          KitTextToStringMaxUnsigned_T.
*/
class ToString
{
public:
    /** This method converts a signed integer to a string.
        @param 'num'          Number to convert
        @param 'dstString'    Buffer to hold the output
        @param 'maxChars'     Size, in bytes, of 'dstString'.  Note: This includes the space for the null terminator
        @param 'padChar'      The character to use to pad any unused leading characters in 'dstString'

        @returns A pointer to the beginning of the converted number when
                 successful; else nullptr is returned on error (e.g. 'dstString'
                 does not have sufficient space). The returned pointer is essentially
                 a left justified string of the converted value. For a right
                 justified result, use the original pointer passed as 'dstString'.
     */
    template <typename T>
    static const char* signedInt( T num, char* dstString, size_t maxChars, char padChar = ' ' ) noexcept
    {
        return convert_( (KitTextToStringMaxUnsigned_T)num, dstString, maxChars, 10, padChar, num < 0 );
    }

public:
    /** This method converts a unsigned integer to a string.
        @param 'num'          Number to convert
        @param 'dstString'    Buffer to hold the output
        @param 'maxChars'     Size, in bytes, of 'dstString'.  Note: This includes the space for the null terminator
        @param 'base'         Number base for conversion
        @param 'padChar'      The character to use to pad any unused leading characters in 'dstString'

        @returns A pointer to the beginning of the converted number when
                 successful; else nullptr is returned on error (e.g. 'dstString'
                 does not have sufficient space). The returned pointer is essentially
                 a left justified string of the converted value. For a right
                 justified result, use the original pointer passed as 'dstString'.
     */
    template <typename T>
    static const char* unsignedInt( T num, char* dstString, size_t maxChars, unsigned base = 10, char padChar = ' ' ) noexcept
    {
        return convert_( num, dstString, maxChars, base, padChar, false );
    }

protected:
    /// Helper method
    static const char* convert_( KitTextToStringMaxUnsigned_T num, char* dstString, size_t maxChars, unsigned base, char padChar, bool isNegative ) noexcept;
};

}  // end namespaces
}
#endif  // end header latch
