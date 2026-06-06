#ifndef KIT_TEXT_TOKENIZER_BASIC_H_
#define KIT_TEXT_TOKENIZER_BASIC_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

///
namespace Kit {
///
namespace Text {
///
namespace Tokenizer {


/** This concrete class performs basic tokenizing/parse functions on a
    string. A token is a one or more characters separated by one or
    more delimiter characters. The parsing is a destructive parsing in
    that it modifies the original string.  The tokenizing is done
    one token at time i.e. when the next() method is called.
 */
class Basic
{
private:
    /// Definition of whitespace
    static constexpr const char* whiteSpace() { return " \t\n\r"; };

public:
    /// Constructor. Uses whitespace for token delimiters
    Basic( char* stringToParse )
        : m_ptr( nullptr)
        , m_delimiters( whiteSpace() )
        , m_base( nullptr )
        , m_count( 0 )
    {
        m_base = m_ptr = stringToParse;
    }

    /// Constructor. Uses the specified character set for token delimiters
    Basic( char* stringToParse, const char* delimiterSet )
        : m_ptr( nullptr )
        , m_delimiters( delimiterSet )
        , m_base( nullptr )
        , m_count( 0 )
    {
        m_base = m_ptr = stringToParse;
    }

public:
    /** Returns a pointer to the next token.  Returns nullptr if no tokens exist,
        or the string has already been completed tokenized. NOTE: This
        method is also used to get the 'first' token.
     */
    const char* next() noexcept;

    /** Returns a pointer to the portion of the string that has not
        been tokenized, i.e. the first character AFTER the delimiter character
        that marked the end of the current token to End-of-String.
     */
    inline const char* remaining() const noexcept { return m_ptr; }


public:
    /** This method returns the Nth token.  Where N is the zero-based token
        index, i.e. 0:=first token, 1:=second token, etc. If N is out-of-bound
        then nullptr is returned.  Typically this method is called once the entire
        string has been tokenized.
     */
    const char* getToken( unsigned n ) const noexcept;

    /// Returns the number of tokens parsed to-date.
    inline unsigned numTokens() const noexcept { return m_count; }

protected:
    /// Pointer to the current token
    char* m_ptr;

    /// Pointer to the token delimiters
    const char* m_delimiters;

    /// Pointer to the start of the original string
    char* m_base;

    /// Number of tokens parsed to date
    unsigned m_count;
};


}  // end namespaces
}
}
#endif  // end header latch
