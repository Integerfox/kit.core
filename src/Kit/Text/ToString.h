#ifndef Cpl_Text_btoa_h_
#define Cpl_Text_btoa_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include <stdlib.h>
#include <stdint.h>

///
namespace Kit {
///
namespace Text {

/** This static class is used to convert integer values to a null terminated
    string WITHOUT using snprintf() function calls. This is for platforms/situations
    where it is not possible or undesirable (i.e. stack usage) to call snprintf.


    THE FUNCTION ARGUMENTS FOR ALL PUBLIC METHODS:
    ------------------------------------------------------
    'num'          Number to convert
    'dstString'    Buffer to hold the output
    'maxChars'     Size, in bytes, of 'dstString'.  Note: This includes the space for the null terminator
    'base'         Number base for conversion
    'padChar'      The character to use to pad any unused leading characters in 'dstString'


    THE RETURN VALUES FOR ALL PUBLIC METHODS:
    ------------------------------------------------------
    A pointer to the beginning of the converted number when successful; else
    nullptr is returned on error (e.g. illegal 'base' value).

    The returned pointer is essentially a left justified string of the converted
    value. For a right justified result, use the original pointer passed as
    'dstString'.

    When attempting to convert the INT64_MIN value, the conversion will fail 
    (i.e. return nullptr)


    THE CONVERSION SEMANTICS FOR ALL PUBLIC METHODS:
    --------------------------------------------------------------
    If the number of converted digits exceeds the size of 'dstString',
    then the MOST significant digit(s) are discarded.

    When the 'number' is negative there will always be a leading minus
    sign ('-') regardless of 'base' selection AND if it means discarding a 
    MOST significant digit.

    There is no feedback if/when digits are discarded.
*/
class ToString
{
public:
    /// This method converts a signed integer to a string.  See class comments for additional details.
    template <typename T>
    static const char* signedInt( T num, char* dstString, size_t maxChars, unsigned base = 10, char padChar = ' ' ) noexcept
    {
        int64_t signedNum = (int64_t) num;
        int result = negativeCheck( signedNum );
        if ( result == 0 )
        {
            return nullptr;
        }
        return convert_( signedNum, dstString, maxChars, base, padChar, result < 0 );
    }

    /// This method converts a unsigned integer to a string.  See class comments for additional details.
    template <typename T>
    static const char* unsignedInt( T num, char* dstString, size_t maxChars, unsigned base = 10, char padChar = ' ' ) noexcept
    {
        return convert_( num, dstString, maxChars, base, padChar, false );
    }

protected:
    /// Helper method. Returns 0 if error, 1 if positive, -1 if negative
    static int negativeCheck( int64_t& signedNum ) noexcept
    {
        if ( signedNum < 0 )
        {
            // For two complement math there is no positive equivalent for the 
            // INT64_MIN value.  So we fail the conversation when 'signedNum' == INT64_MIN
            if ( signedNum == INT64_MIN )
            {
                return 0;
            }
            signedNum  = -signedNum;
            return -1;
        }
        return 1;
    }

    /// Helper method
    static const char* convert_( uint64_t num, char* dstString, size_t maxChars, unsigned base, char padChar, bool isNegative ) noexcept;
};

}  // end namespaces
}
#endif  // end header latch
