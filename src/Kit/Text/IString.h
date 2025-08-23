#ifndef KIT_TEXT_ISTRING_H_
#define KIT_TEXT_ISTRING_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Container/Key.h"
#include "Kit/System/printfchecker.h"
#include <stdarg.h>
#include <iostream>

///
namespace Kit {
///
namespace Text {

/** This abstract class defines the operations that can be before on
    a NULL terminated string. The intent is to provide a simple mechanism
    for manipulating and "passing-around" strings rather than using error
    prone explicit string operations.

    NOTES:

        o The memory management for the string data is determined by
          the concrete sub-class!
        o The 'IString' class IS A Kit::Container::Key, but it is NOT a
          Kit::Container::xxxItem instance.  Use the 'StringItem' class
          for storing a string directly in a container.
 */

class IString : public Kit::Container::Key
{
public:
    /** Assignment of a string. The argument string is copied
        to the internal storage of the object.
     */
    virtual IString& operator=( const IString& string ) noexcept = 0;

    /** Assignment of a string. The argument string is copied
        to the internal storage of the object.
     */
    virtual IString& operator=( const char* string ) noexcept = 0;

    /** Assigns the character to first position in string and then
        NULL terminates the string.
     */
    virtual IString& operator=( char c ) noexcept = 0;

    /** Converts the 'number' to a string and stores its in
        the internal storage of the object.
     */
    virtual IString& operator=( int num ) noexcept = 0;

    /** Converts the 'number' to a string and stores its in
        the internal storage of the object.
     */
    virtual IString& operator=( unsigned int num ) noexcept = 0;

    /** Converts the 'number' to a string and stores its in
        the internal storage of the object.
     */
    virtual IString& operator=( long num ) noexcept = 0;

    /** Converts the 'number' to a string and stores its in
        the internal storage of the object.
     */
    virtual IString& operator=( long long num ) noexcept = 0;

    /** Converts the 'number' to a string and stores its in
        the internal storage of the object.
     */
    virtual IString& operator=( unsigned long num ) noexcept = 0;

    /** Converts the 'number' to a string and stores its in
        the internal storage of the object.
     */
    virtual IString& operator=( unsigned long long num ) noexcept = 0;

    /** This method copies the content of the source string,
        up to 'n' characters, into internal storage of the object.
        NOTE: The internal storage is always guaranteed to be NULL
        terminated.
     */
    virtual void copyIn( const char* string, int n ) noexcept = 0;

    /** This method inserts the specified string starting at
        the specified offset.  The original contents of the string
        beginning with 'insertOffset' is "shifted to the right" to
        make room for the string being inserted. If an attempt is
        made to insert the string past the current end-of-string, then
        the stringToInsert is simply appended.  If an attempt is made
        to insert the string at a negative offset, then the stringToInsert
        is prepended to the string.
     */
    virtual void insertAt( int insertOffset, const char* stringToInsert ) noexcept = 0;

    /** Clears the internal string (i.e. string[0] = '\0').
     */
    virtual void clear() noexcept = 0;


public:
    /** Append a string. The argument string is concatenated
        to the end of the string object.
     */
    virtual IString& operator+=( const IString& string ) noexcept = 0;

    /** Append a string. The argument string is concatenated
        to the end of the string object.
     */
    virtual IString& operator+=( const char* string ) noexcept = 0;

    /** Appends the character to the end of the string.
     */
    virtual IString& operator+=( char c ) noexcept = 0;

    /** Converts the 'number' to a string and appends its to the
        end of the string.
     */
    virtual IString& operator+=( int num ) noexcept = 0;

    /** Converts the 'number' to a string and appends its to the
        end of the string.
     */
    virtual IString& operator+=( unsigned int num ) noexcept = 0;

    /** Converts the 'number' to a string and appends its to the
        end of the string.
     */
    virtual IString& operator+=( long num ) noexcept = 0;

    /** Converts the 'number' to a string and appends its to the
        end of the string.
     */
    virtual IString& operator+=( long long num ) noexcept = 0;

    /** Converts the 'number' to a string and appends its to the
        end of the string.
     */
    virtual IString& operator+=( unsigned long num ) noexcept = 0;

    /** Converts the 'number' to a string and appends its to the
        end of the string.
     */
    virtual IString& operator+=( unsigned long long num ) noexcept = 0;

    /** This method appends the content of the source string,
        up to 'n' characters, into internal storage of the object.
        NOTE: The internal storage is always guaranteed to be NULL
        terminated.
     */
    virtual void appendTo( const char* string, int n ) noexcept = 0;

public:
    /// Cast to character -->i.e returns the first character in the string.
    virtual operator char() const noexcept = 0;

    /// Read-only Access to the "raw" string.
    virtual const char* getString() const noexcept = 0;

    /// Cast to read-only character string pointer.
    inline operator const char*() const noexcept { return getString(); }

    /// Returns a Read-only pointer to the "raw" (short-hand for getString())
    inline const char* operator()() const noexcept { return getString(); }

    /// Returns the specified character. If n is out-bounds, then '\0' is returned
    inline char operator[]( int i ) const noexcept { return i < 0 || i > length() ? '\0' : getString()[i]; }


public:
    /** Returns the length, in bytes, of the string.  The size returned does
        NOT include NULL terminator (i.e. if the returned size is 15, my
        internal storage must be at least 15+1)
     */
    virtual int length() const noexcept = 0;

    /** Returns the max allowed size of the string. The size returned does
        NOT include NULL terminator (i.e. if the returned size is 15, my
        internal storage must be at least 15+1)
     */
    virtual int maxLength() const noexcept = 0;

    /** Returns the available space left in the internal buffer. The size
        returned does NOT include NULL terminator (i.e. if the returned size
        is 15, my internal storage must be at least 15+1).
     */
    inline int availLength() noexcept { return maxLength() - length(); }

    /// Returns true if the string is empty (i.e. string[0] == '\0')
    virtual bool isEmpty() const noexcept = 0;

    /** Returns true if the last IString "write" operation caused the
        IString contents to be truncated.  This flag is reset after every
        string "write" operation.
     */
    virtual bool truncated() const noexcept = 0;


public:
    /** Compares two Strings and returns true if they are equal
        NOTE: if 'string' is a null pointer, then false is returned.
     */
    virtual bool operator==( const char* string ) const noexcept = 0;

    /// Compares two Strings and returns true if they are equal
    virtual bool operator==( const IString& string ) const noexcept = 0;

    /** Compares two Strings and returns true if they are NOT equal
        NOTE: if 'string' is a null pointer, then true is returned.
     */
    virtual bool operator!=( const char* string ) const noexcept = 0;

    /// Compares two Strings and returns true if they are NOT equal
    virtual bool operator!=( const IString& string ) const noexcept = 0;

    /** Compares two strings independently of case and return true if equal
        NOTE: if 'string' is a null pointer, then false is returned.
     */
    virtual bool isEqualIgnoreCase( const char* string ) const noexcept = 0;

    /// Compares the specified character to the first character in the string
    virtual bool operator==( char c ) const noexcept = 0;

    /** Compares two strings and returns <0, 0, >0 if this string is less than,
        equal, or greater than respectively to the specified string.
        NOTE: if 'string' is a null pointer, then -1 is returned.
     */
    virtual int compare( const char* string ) const noexcept = 0;

    /// Same as compare(), but case insensitive.
    virtual int compareIgnoreCase( const char* string ) const noexcept = 0;

    /** Compares the specified substring with the specified string.  Substring
        is inclusive of the endpoints.  If the endpoints are out-of-range, they
        are clamped by the actual bounds of the string.  Position parameters are
        zero-based indexes into this IString object.
        NOTE: if 'string' is a null pointer, then false is returned.
     */
    virtual bool isEqualSubstring( int startOffset, int endOffset, const char* string ) const noexcept = 0;

    /// Same as above, except case insensitive comparison.
    virtual bool isEqualSubstringIgnoreCase( int startOffset, int endOffset, const char* string ) const noexcept = 0;

    /** Returns true if the string starts with the specified sub-string.
        Note: if 'string' is a null pointer, then false is returned
     */
    virtual bool startsWith( const char* string ) const noexcept = 0;

    /** Sames as starsWith(), but begins the search at index 'startOffset'.
        Note: If 'string' is a null pointer or 'startOffset' is out-of-bound,
        then false is returned.
     */
    virtual bool startsWith( const char* string, int startOffset ) const noexcept = 0;

    /** Returns true if the string ends with the specified sub-string.
        Note: if 'string' is a null pointer, then false is returned
     */
    virtual bool endsWith( const char* string ) const noexcept = 0;

public:
    /** Returns the index of the first match for the specified character.  If
        no match is found, -1 is returned.
     */
    virtual int indexOf( char c ) const noexcept = 0;

    /** Same as indexOf(char), but starts at 'startOffset' instead of the beginning
        of the string.

        Note: If 'startOffset' is out-of-bounds than -1 is returned.
     */
    virtual int indexOf( char c, int startOffset ) const noexcept = 0;

    /** Returns the index (of the starting character) of the first match
        for the specified sub-string.  If no match is found, -1 is returned.

        Note: If 'string' is null, then -1 is returned
     */
    virtual int indexOf( const char* string ) const noexcept = 0;

    /** Same as indexOf(const char*), but starts at 'startOffset' instead of the
        beginning of the string.

        Note: If 'string' is null or 'startOffset' is out-of-bounds than -1
              is returned.
     */
    virtual int indexOf( const char* string, int startOffset ) const noexcept = 0;

    /** Returns the index of the last match for the specified character.  If
        no match is found, -1 is returned.
     */
    virtual int lastIndexOf( char c ) const noexcept = 0;

    /** Returns the number of times the specified character appears in
        the string.
     */
    virtual int count( char c ) const noexcept = 0;


public:
    /** Allows "printf" formatting of the string. NOTE: Since the
        amount of information being written to the string is variable,
        this method does NOT guarantee how much, if any, of the
        information is actually written to the string.  The method
        does guarantee that the string's internal storage will NOT
        be exceeded.

        NOTE: if 'format' is null, then nothing is done
     */
    KIT_SYSTEM_PRINTF_CHECKER( 2, 3 )
    virtual void format( const char* format, ... ) noexcept = 0;

    /// Same as format() - but appends "formatting" to the end of the string
    KIT_SYSTEM_PRINTF_CHECKER( 2, 3 )
    virtual void formatAppend( const char* format, ... ) noexcept = 0;

    /** This method is the same as format(), except when 'appendFlag' is true
        then it behaves as formatAppend().
     */
    KIT_SYSTEM_PRINTF_CHECKER( 3, 4 )
    virtual void formatOpt( bool appendFlag, const char* format, ... ) noexcept = 0;


    /** Same as format(), except that it is called with a va_list
        instead of a variable number of arguments.
     */
    virtual void vformat( const char* format, va_list ap ) noexcept = 0;

    /** Same as formatAppend(), except that it is called with a va_list
        instead of a variable number of arguments.
     */
    virtual void vformatAppend( const char* format, va_list ap ) noexcept = 0;

    /** This method is the same as vformat(), except when 'appendFlag' is true
        then it behaves as vformatAppend().
     */
    virtual void vformatOpt( bool appendFlag, const char* format, va_list ap ) noexcept = 0;


public:
    /** Forces the entire string to upper case characters.  Also returns
        the newly modified string contents.
     */
    virtual const char* toUpper() noexcept = 0;

    /** Forces the entire string to lower case characters.  Also returns
        the newly modified string contents.
     */
    virtual const char* toLower() noexcept = 0;

    /** Removes any leading white-space from the string (white-space
        is defined by the standard isspace() function).
     */
    virtual void removeLeadingSpaces() noexcept = 0;

    /** Removes any trailing white-space from the string (white-space
        is defined by the standard isspace() function).
     */
    virtual void removeTrailingSpaces() noexcept = 0;

    /** Removes the specified leading characters from the string.

        NOTE: if 'charsSet' is null, nothing is done.
     */
    virtual void removeLeadingChars( const char* charsSet ) noexcept = 0;

    /** Removes the specified trailing characters from the string.

        NOTE: if 'charsSet' is null, nothing is done.
     */
    virtual void removeTrailingChars( const char* charsSet ) noexcept = 0;

    /** Removes the characters beginning with 'startOffset' and ending with
        'endOffset'.  The characters after 'endOffset' are shifted left
        to form a continuous string. If the position fields are invalid
        or out-of-bounds, nothing is done.
     */
    virtual void cut( int startOffset, int endOffset ) noexcept = 0;

    /** Removes 'n' characters from the start-of-string. If 'n' is greater
        than the length of the string or less than zero, nothing is done.
     */
    inline void trimLeft( int n ) noexcept { cut( 0, n - 1 ); }

    /** Removes 'n' characters from the end-of-string. If 'n' is greater
        than the length of the string or less than zero, nothing is done.
     */
    virtual void trimRight( int n ) noexcept = 0;

    /** Replaces the character at index 'atOffset' with 'newchar'.
        If 'atOffset' is out-of-bounds or greater than the length of the
        actual string, then nothing is done.
     */
    virtual void setChar( int atOffset, char newchar ) noexcept = 0;

    /** Replaces all instances of targetChar in the string with
        newChar.  Returns the number characters (if any) replaced.
        NOTE: If targetChar == newChar, nothing is done and zero
              is returned.
     */
    virtual int replace( char targetChar, char newChar ) noexcept = 0;


public:
    /** Returns a "writable" pointer to the string's internal storage.
        The caller is RESPONSIBLE for insuring that the string is
        left in a valid state - that is the string is NULL terminated
        and has NOT exceeded the 'maxAllowedLength'.  Also The pointer
        and length have a VERY LIMITED life span - the values become
        invalid when any of the another string methods are called!
        NOTE: This method always clears the 'truncated' flags/status.

        *** THIS METHOD SHOULD BE USED WITH EXTREME CAUTION AND AVOIDED WHENEVER POSSIBLE! ***
     */
    virtual char* getBuffer( int& maxAllowedLength ) noexcept = 0;

public:
    /// Virtual destructor!
    virtual ~IString() noexcept {}

public:
    /// Default constructor
    IString() noexcept = default;
    
    /// Delete move constructor to prevent moving
    IString(IString&&) = delete;
    
    /// Delete move assignment operator
    IString& operator=(IString&&) = delete;
};

}  // end namespaces
}

/* Note: I could only make the stream operators work if there were NOT declared
         within the Kit::Text namespace.  Fortunately, I don't actually have to
         make the stream operators 'friends' for the necessary implementation.

         If someone could explain/show me how to make it work using the
         'friend' approach - please contact me.
*/
namespace std {

/// Support stream output operator directly
ostream& operator<<( ostream& out, const Kit::Text::IString& outstring );

/// Support stream input operator directly
istream& operator>>( istream& in, Kit::Text::IString& instring );

}  // end std namespace
#endif  // end header latch
