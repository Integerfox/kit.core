#ifndef KIT_TEXT_STRING_BASE_H_
#define KIT_TEXT_STRING_BASE_H_
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


///
namespace Kit {
///
namespace Text {


/** This partially concrete class implements the portions of the String Api
    that is independent of the internal storage mechanisms.
 */

class StringBase : public IString
{
protected:
    /// Constructor
    StringBase( char* internalStringPtr ) noexcept;

    /// Delete move constructor to prevent moving
    StringBase( IString&& ) noexcept = delete;

public:
    /// See Kit::Text::IString
    void clear() noexcept override;

    /// See Kit::Text::IString
    operator char() const noexcept override;

    /// See Kit::Text::IString
    int length() const noexcept override;

    /// See Kit::Text::IString
    bool isEmpty() const noexcept override;

    /// See Kit::Text::IString
    bool truncated() const noexcept override;

    /// See Kit::Text::IString
    bool operator==( const char* string ) const noexcept override;

    /// See Kit::Text::IString
    bool operator==( const IString& string ) const noexcept override;

    /// See Kit::Text::IString
    bool operator!=( const char* string ) const noexcept override;

    /// See Kit::Text::IString
    bool operator!=( const IString& string ) const noexcept override;

    /// See Kit::Text::IString
    bool isEqualIgnoreCase( const char* string ) const noexcept override;

    /// See Kit::Text::IString
    int compare( const char* string ) const noexcept override;

    /// See Kit::Text::IString
    int compareIgnoreCase( const char* string ) const noexcept override;

    /// See Kit::Text::IString
    bool operator==( char c ) const noexcept override;

    /// See Kit::Text::IString
    bool isEqualSubstring( int startpos, int endpos, const char* string ) const noexcept override;

    /// See Kit::Text::IString
    bool isEqualSubstringIgnoreCase( int startpos, int endpos, const char* string ) const noexcept override;

    /// See Kit::Text::IString
    bool startsWith( const char* string ) const noexcept override;

    /// See Kit::Text::IString
    bool startsWith( const char* string, int startOffset ) const noexcept override;

    /// See Kit::Text::IString
    bool endsWith( const char* string ) const noexcept override;

    /// See Kit::Text::IString
    KIT_SYSTEM_PRINTF_CHECKER( 2, 3 )
    void format( const char* format, ... ) noexcept override;

    /// See Kit::Text::IString
    KIT_SYSTEM_PRINTF_CHECKER( 2, 3 )
    void formatAppend( const char* format, ... ) noexcept override;

    /// See Kit::Text::IString
    KIT_SYSTEM_PRINTF_CHECKER( 3, 4 )
    void formatOpt( bool appendFlag, const char* format, ... ) noexcept override;

    /// See Kit::Text::IString
    void vformat( const char* format, va_list ap ) noexcept override;

    /// See Kit::Text::IString
    void vformatAppend( const char* format, va_list ap ) noexcept override;

    /// See Kit::Text::IString
    void vformatOpt( bool appendFlag, const char* format, va_list ap ) noexcept override;

    /// See Kit::Text::IString
    int indexOf( char c ) const noexcept override;

    /// See Kit::Text::IString
    int indexOf( char c, int startpos ) const noexcept override;

    /// See Kit::Text::IString
    int indexOf( const char* str ) const noexcept override;

    /// See Kit::Text::IString
    int indexOf( const char* str, int startpos ) const noexcept override;

    /// See Kit::Text::IString
    int lastIndexOf( char c ) const noexcept override;

    /// See Kit::Text::IString
    int count( char c ) const noexcept override;

    /// See Kit::Text::IString
    const char* toUpper() noexcept override;

    /// See Kit::Text::IString
    const char* toLower() noexcept override;

    /// See Kit::Text::IString
    void removeLeadingSpaces() noexcept override;

    /// See Kit::Text::IString
    void removeTrailingSpaces() noexcept override;

    /// See Kit::Text::IString
    void removeLeadingChars( const char* charsSet ) noexcept override;

    /// See Kit::Text::IString
    void removeTrailingChars( const char* charsSet ) noexcept override;

    /// See Kit::Text::IString
    void cut( int startpos, int endpos ) noexcept override;

    /// See Kit::Text::IString
    void trimRight( int n ) noexcept override;

    /// See Kit::Text::IString
    void setChar( int atPosition, char newchar ) noexcept override;

    /// See Kit::Text::IString
    int replace( char targetChar, char newChar ) noexcept override;

    /// See Kit::Text::IString
    const char* getString() const noexcept override;

    /// See Kit::Text::IString
    char* getBuffer( int& maxAllowedLength ) noexcept override;

    /// Make parent method visible
    using IString::operator=;

    /// Assignment
    IString& operator=( const IString& string ) noexcept override;

    /// Assignment
    IString& operator=( const StringBase& string ) noexcept;

    /// Assignment
    IString& operator=( const char* string ) noexcept override;

    /// Assignment
    IString& operator=( char c ) noexcept override;

    /// Make parent method visible
    using IString::operator+=;

    /// Append
    IString& operator+=( const IString& string ) noexcept override;

    /// Append
    IString& operator+=( const char* string ) noexcept override;

    /// Append
    IString& operator+=( char c ) noexcept override;

public:
    /// See Cpl::Container::Key
    int compareKey( const Key& key ) const noexcept override;

    /// See Cpl::Container::Key
    const void* getRawKey( unsigned* returnRawKeyLenPtr = 0 ) const noexcept override;

protected:  // Helpers
    /// Sets the truncated flag if needed and insures string is null terminated
    void validateSizeAfterFormat( int availableLen, int formattedLen, int maxlen ) noexcept;

protected:
    /// Pointer to the actual internal string
    char* m_strPtr;

    /// Flag to track when the internal string get truncated
    bool m_truncated;
};

};  // end namespaces
};
#endif  // end header latch
