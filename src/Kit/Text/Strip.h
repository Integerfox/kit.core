#ifndef Cpl_Text_strip_h_
#define Cpl_Text_strip_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

 #include <string.h>

///
namespace Kit {
///
namespace Text {

/** This all static class contains a set of methods that provide basic building
    blocks for parsing 'tokens' with in a null terminated string. It also
    provide functions for removing leading/trailing whitespace.
*/
class Strip
{
public:
    /** This method returns a pointer to the FIRST non-whitespace character in
        the specified null-terminated string.

        NOTES:
            o This method does NOT modify the original string in any way!
            o If 's' is null, then null is returned.
     */
    static const char* space( const char* s );


    /** This method returns a pointer to the FIRST whitespace character in the
        specified null-terminated string.

        NOTES:
            o This method does NOT modify the original string in any way!
            o If 's' is null, then null is returned.

        This method is useful in finding the 'next' token in a string, for example:
        @code

            // Returns a pointer to the first token in 'input'
            const char* token = Strip::space(input)

            // Returns a pointer to the second token in 'input'
            token = Strip::space(Strip::notSpace(token))

        @endcode
     */
    static const char* notSpace( const char* s );


    /** This method returns a pointer to the LAST non-whitespace character in
        the specified null-terminated string.

        NOTES:

            o This method does NOT modify the original string in any way!
            o If 's' is null, then null is returned.
            o If the entire string is whitespace, a pointer to the start of
              the string is returned (i.e. 's' returned)
     */
    static const char* trailingSpace( const char* s );


    /** This method TRUNCATES the specified null-terminated string by eliminating
        any trailing white space.

        NOTE: If 's' is null, then nothing is done.
     */
    static void removeTrailingSpace( char* s );


    /** This method is the same as space(), except the specified character set
        is used to terminate the search instead of the standard isspace()
        characters.

        NOTES:
            o If 's' is null, then null is returned
            o If 'charsSet' is null, then 's' is returned and nothing is done.
     */
    static const char* chars( const char* s, const char* charsSet );


    /** This method is the same as notSpace(), except the specified character
        set is used to terminate the search instead of the standard isspace()
        characters.

        NOTES:
            o If 's' is null, then null is returned
            o If 'charsSet' is null, then 's' is returned and nothing is done.
     */
    static const char* notChars( const char* s, const char* charsSet );

    /** This method is the same as trailingSpaces(), except the specified
        character set is used to identify the last "non-whitespace" character.

        NOTES:
            o If 's' is null, then null is returned
            o If 'charsSet' is null, then 's' is returned and nothing is done.
            o If the entire string is made up of 'charsSet', a pointer to the start
              of the string is returned (i.e. 's' returned)
     */
    static const char* trailingChars( const char* s, const char* charsSet );

    /** This method is the same as removeTrailingSpaces(), except the specified
        characters set is used for "whitespace" any trailing white space.

        NOTE: If 's' OR 'charsSet' is null, then nothing is done.
     */
    static void removeTrailingChars( char* s, const char* charsSet );


    /** This method returns true if the character 'c' is one of character(s) 
        contained in 'charsSet'; else false is returned.
     */
    static inline bool isCharInString( const char* charsSet, const char c )
    {
        return charsSet && strchr( charsSet, c ) != nullptr;
    }
};


}       // end namespaces
}
#endif  // end header latch
