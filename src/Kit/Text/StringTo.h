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

#include <stdlib.h>
#include <type_traits>  // For type traits
#include <limits>       // For numeric_limits


///
namespace Kit {
///
namespace Text {

/** This static class contains collection of methods that wrap the standard C
    library functions for converting text/string to binary values.
*/
class StringTo
{
public:
    /** This template method converts the specified string to an signed integer.
        The method returns true if the conversion was successful. When false is
        returned, the 'convertedValue' argument is NOT updated.

        By default the conversion assumes a base 10 number and that the 'end-of-number'
        is end-of-string.

        If endptr is specified and the conversation was successful, a pointer
        to the first character 'after' the number is returned.

        NOTE: Only base 10 conversions are supported.

        The template argument 'T' must be a signed integer type.
    */
    template <typename T>
    static bool signedInt( T&           convertedValue,
                           const char*  string,
                           const char*  validStopChars = nullptr,
                           const char** endptr         = nullptr ) noexcept
    {
        // Static assertion to enforce that T is a signed integer type
        static_assert( std::is_signed<T>::value, "T must be a signed integer type" );

        long long dstVal;
        bool      result = a2ll( dstVal, string, 10, validStopChars, endptr );

        // Check if the value is within the range of type T
        if ( result && ( dstVal >= std::numeric_limits<T>::min() && dstVal <= std::numeric_limits<T>::max() ) )
        {
            convertedValue = static_cast<T>( dstVal );
            return true;
        }
        return false;
    }

public:
    /** This template method converts the specified string to an unsigned integer.
        The method returns true if the conversion was successful. When false is
        returned, the 'convertedValue' argument is NOT updated.

        By default the conversion assumes a base 10 number and that the 'end-of-number'
        is end-of-string.

        If endptr is specified and the conversation was successful, a pointer
        to the first character 'after' the number is returned.  If the input
        string is negative number (i.e. has leading minus sign) then the
        conversion fails.

        The template argument 'T' must be a unsigned integer type.
    */
    template <typename T>
    static bool unsignedInt( T&           convertedValue,
                             const char*  string,
                             int          base           = 10,
                             const char*  validStopChars = nullptr,
                             const char** endptr         = nullptr ) noexcept
    {
        // Static assertion to enforce that T is an unsigned integer type
        static_assert( std::is_unsigned<T>::value, "T must be an unsigned integer type" );

        unsigned long long dstVal;
        bool               result = a2ull( dstVal, string, base, validStopChars, endptr );

        // Check if the value is within the range of type T
        if ( result && dstVal <= std::numeric_limits<T>::max() )
        {
            convertedValue = static_cast<T>( dstVal );
            return true;
        }
        return false;
    }

public:
    /** This template method converts the specified string to an floating point
        number. The method returns true if the conversion was successful. When
        false is returned, the 'convertedValue' argument is NOT updated. By
        default the conversion assumes that the 'end-of-number' is end-of-string.
        If endptr is specified and the method returns true, a pointer to the
        first character 'after' the number is returned.
     */
    template <typename T>
    static bool floating( T&           convertedValue,
                          const char*  string,
                          const char*  validStopChars = nullptr,
                          const char** endptr         = nullptr ) noexcept
    {
        // Static assertion to enforce that T is an floating point type
        static_assert( std::is_floating_point<T>::value, "T must be a floating point type" );

        double dstVal;
        bool   result = a2d( dstVal, string, validStopChars, endptr );
        if ( result )
        {
            convertedValue = static_cast<T>( dstVal );
            return true;
        }
        return false;
    }

public:
    /** This method convert the specified string to an boolean.  The method returns
        true if the conversion was successful. When false is returned, the
        'convertedValue' argument is NOT updated. If the first N characters of
        'string' match the specified boolean token - the conversion is consider
        successful (i.e. there is no required separator/terminator character for
        a boolean token). If endptr is specified and the method returns true, a
        pointer to the first character 'after' the boolean token is returned.
     */
    static bool boolean( bool&        convertedValue,
                         const char*  string,
                         const char*  trueToken  = "T",
                         const char*  falseToken = "F",
                         const char** endptr     = nullptr ) noexcept;


#if 0
    /** This method will convert an 'ASCII HEX' string to an equivalent binary
        buffer, i.e. the reverse of bufferToAsciiHex() in format.h.  If the entire
        string was not able to be converted then -1 is returned, else the number
        of converted bytes are returned.
     */
    long asciiHexToBuffer( void* dstBinary, const char* srcString, size_t dstMaxLen );

    /** This method will convert an 'ASCII BINARY' string to an equivalent binary
        buffer, i.e. the reverse of bufferToAsciiBinary() in format.h.  The
        'reverse' argument when set to true will store the binary data starting
        with the last byte of 'dstBinary'.

        If the number of '1' and '0' in 'srcString' is not a multiple of eight - the
        'missing' bits will be set to zero in the binary output.

        If the entire string was not able to be converted then -1 is returned OR if
        there are non '1'/'0' characters in the srcString; else the number of
        converted bits are returned.

        Note: 'srcString' MUST be a null terminated string
     */
    long asciiBinaryToBuffer( void* dstBinary, const char* srcString, size_t dstMaxLen, bool reverse = false );


    /** This method parses a 'timestamp' with the following format: [DD ]HH:MM:SS[.sss]
        The method returns true if the parse was successful.
     */
    bool parsePrecisionTimeStamp( const char* timeStampToParse, Cpl::System::ElapsedTime::Precision_T& convertedValue );
#endif

protected:
    /// Helper method
    static bool a2ll( long long& convertedValue, const char* string, int base, const char* validStopChars, const char** end ) noexcept;

    /// Helper method
    static bool a2ull( unsigned long long& convertedValue, const char* string, int base = 10, const char* validStopChars = nullptr, const char** endptr = nullptr ) noexcept;

    /// Helper method
    static bool a2d( double& convertedValue, const char* string, const char* validStopChars, const char** end ) noexcept;
};

}  // end namespaces
}
#endif  // end header latch
