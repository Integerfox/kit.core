#ifndef KIT_TEXT_FORMAT_H_
#define KIT_TEXT_FORMAT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Text/IString.h"
#include <stdlib.h>


///
namespace Kit {
///
namespace Text {

/** This static class contains some general purpose string formatting functions.
 */
class Format
{
public:
    /** This method will take a raw data buffer and convert it to an 'viewable'
        string.  Non-printable values will displayed as '.'.  Note: The default
        operation is to clear the destString before the conversion.  The method
        will return true if buffer was successfully converted, i.e. the ENTIRE
        buffer was converted; else false is returned (this include the cases of
        null 'buffer' pointer, 'len' equals zero, or not enough memory in 'destString'
        to contain the final result).
     */
    static bool string( const void* buffer,
                        int         len,
                        IString&    destString,
                        bool        appendToString = false ) noexcept;


    /** This method will convert a binary buffer to 'ASCII HEX', e.g. given the
        binary data of { 12, F2, 54 }, destString:= "12F254". The method will
        return true if buffer was successfully converted, i.e. the ENTIRE buffer
        was converted to a string; else false is returned (this include the cases
        of null 'buffer' pointer, 'len' equals zero, or not enough memory in
        'destString' to contain the final result).

        If 'separator' does not equal '\0', then it inserted between the
        individual bytes in the output string.

        Note: The default operation is to use uppercase text and to clear
              the destString before the conversion.
     */
    static bool asciiHex( const void* binaryData,
                          int         len,
                          IString&    destString,
                          bool        upperCase      = true,
                          bool        appendToString = false,
                          char        separator      = '\0' ) noexcept;


    /** This method converts the binary buffer to a single string that is ASCII
        BINARY. The number of digits in the output string is always a multiple of
        8. The default order for traversing the 'binaryData' is to start with
        binaryData[0].

        The converted result is returned via 'buffer'. If the results where
        truncated by the no enough memory in 'buffer' then false is returned; else
        true is returned.

        Examples:
        binaryData = 0x844A, reverse=false, destString = "1000010001001010"
        binaryData = 0x844A, reverse=true,  destString = "0100101010000100"

     */
    static bool asciiBinary( const void* binaryData,
                             int         len,
                             IString&    destString,
                             bool        appendToString = false,
                             bool        reverse        = false ) noexcept;


    /** This method converts the binary buffer to a single string that is the
        ASCII HEX followed by 'separator', then by the binary data as 'viewable'
        text.  The converted result is returned via 'buffer'. If the results where
        truncated by the no enough memory in 'buffer' then false is returned; else
        true is returned.
     */
    static bool viewer( const void* binaryData,
                        int         len,
                        IString&    destString,
                        int         bytesPerLine   = 16,
                        const char* separator      = "    ",
                        bool        upperCase      = true,
                        bool        appendToString = false ) noexcept;


    /** This method converts the binary 32 bit millisecond count of 'timeStampInMsecs'
        to a string with the following format: "[DD ]HH:MM:SS[.SSS]". The converted
        result is returned via 'buffer'. If the results where truncated due to
        insufficient memory in 'buffer' then false is returned; else true is
        returned.

        NOTE: When 'encodedDay' is set to false, the 'number of days' is encoded
              as 'hours', e.g 3 days and 3 hours would be 75 hours.
     */
    static inline bool timestamp( IString& buffer,
                                  uint32_t timeStampInMsecs,
                                  bool     encodeMsec     = true,
                                  bool     encodeDay      = true,
                                  bool     appendToString = false ) noexcept
    {
        return formatMsec( buffer, static_cast<long long>( timeStampInMsecs ), encodeMsec, encodeDay, appendToString );
    }

    /** This method is the same as timeStamp() but converts a 64bit millisecond
        counter.
     */
    static inline bool timestamp( IString& buffer,
                                  uint64_t timeStampInMsecs,
                                  bool     encodeMsec     = true,
                                  bool     encodeDay      = true,
                                  bool     appendToString = false ) noexcept
    {
        return formatMsec( buffer, static_cast<long long>( timeStampInMsecs ), encodeMsec, encodeDay, appendToString );
    }

protected:
    /// Helper method
    static bool formatMsec( IString&  buffer,
                            long long elapsedMsec,
                            bool      encodeMsec,
                            bool      encodeDay,
                            bool      appendToString ) noexcept;
};

}  // end namespaces
}
#endif  // end header latch
