#ifndef KIT_TEXT_PARSE_H_
#define KIT_TEXT_PARSE_H_
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

/** This static class contains collection of methods that parse 'formatted'
    text strings to binary data.
*/
class Parse
{
public:
    /** This method will convert an 'ASCII HEX' string to an equivalent binary
        buffer, i.e. the reverse of Format::asciiHex() in Format.h.  If the entire
        string was not able to be converted then SIZE_MAX is returned, else the
        number of converted bytes are returned.

        Note: 'srcString' MUST be a null terminated string
     */
    static size_t asciiHex( void*       dstBinary,
                            const char* srcString,
                            size_t      dstMaxLen ) noexcept;

    /** This method will convert an 'ASCII BINARY' string to an equivalent binary
        buffer, i.e. the reverse of Format::asciiBinary() in format.h.  The
        'reverse' argument when set to true will store the binary data starting
        with the last byte of 'dstBinary'.

        If the number of '1' and '0' in 'srcString' is not a multiple of eight - the
        'missing' bits will be set to zero in the binary output.

        If the entire string was not able to be converted then SIZE_MAX is returned
        OR if there are non '1'/'0' characters in the srcString; else the number of
        converted BITS are returned.

        Note: 'srcString' MUST be a null terminated string
     */
    static size_t asciiBinary( void*       dstBinary,
                               const char* srcString,
                               size_t      dstMaxLen,
                               bool        reverse = false ) noexcept;

public:
    /** This method parses a 'timestamp' with the following format:
        [DD ]HH:MM:SS[.sss] (it is the inverse of Format::timestampXXX()
        methods) into a elapsed time value.

        The method returns true if the parse was successful.

        Note: The maximum number of days is limited to 49 days, i.e. 2^32-1 milliseconds
     */
    static bool timestamp( const char* timeStampToParse,
                           uint32_t&   convertedValue ) noexcept;

    /** This method is same as 32bit timestamp() method - except that the
        maximum number of days increased to 75 years.
     */
    static bool timestamp( const char* timeStampToParse,
                           uint64_t&   convertedValue ) noexcept;


public:
    /** Returns the binary value for the 'ASCII HEX' character 'c'.  Returns
        UINT8_MAX if there is an error (e.g. 'c' is not valid 'hex' character).

        This really is a an 'internal' method, but is exposed because it
        occasionally is useful for external widgets/application code.
     */
    static uint8_t hexChar( char c ) noexcept;


    /** This UNSAFE method converts up to 'numCharToScan' characters from the
        ASCII HEX text string.  The converted binary data is stored in 'outData'.

        The application is responsible for ENSURING that the 'outData' is large
        enough to hold the converted output! Application code should AVOID
        using this method when possible.

        This really is a an 'internal' method, but is exposed because it
        occasionally is useful for external widgets/application code.
     */
    static bool asciiHexUnsafe( const char* inString,
                                size_t      numCharToScan,
                                uint8_t*    outData ) noexcept;
};

}  // end namespaces
}
#endif  // end header latch
