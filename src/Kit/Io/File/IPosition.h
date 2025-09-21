
#ifndef KIT_IO_FILE_POSITION_API_H_
#define KIT_IO_FILE_POSITION_API_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/Types.h"

///
namespace Kit {
///
namespace Io {
///
namespace File {


/** This abstract class defines random-access semantics for file-like objects.
 */
class IPosition
{
public:
    /** After a read/write operation this method returns true if the file
        pointer is at EOF.

        NOTES:
        - This method is ONLY VALID immediately following a read/write
          operation!
        - If the File object has been closed, this method will return true.
     */
    virtual bool isEof() noexcept = 0;

    /** Returns the length, in bytes, of the file. If there is an error than
        false is returned.
     */
    virtual bool length( ByteCount_T& length ) noexcept = 0;


public:
    /** Returns the current file pointer offset, in bytes, from the top of the
        file.  If there is an error than false is returned.
     */
    virtual bool currentPos( ByteCount_T& currentPos ) noexcept = 0;

    /** Adjusts the current pointer offset by the specified delta (in bytes).
        Returns true if successful, else false (i.e. setting the pointer
        past/before the file boundaries).
     */
    virtual bool setRelativePos( ByteCount_T deltaOffset ) noexcept = 0;

    /** Sets the file pointer to the absolute specified offset (in bytes).
        Returns true if successful, else false (i.e. setting the
        pointer past the end of the file).
     */
    virtual bool setAbsolutePos( ByteCount_T newoffset ) noexcept = 0;

    /** Sets the file pointer to End-Of-File.  Returns true  if successful, else
        false if an error occurred.
     */
    virtual bool setToEof() noexcept = 0;


public:
    /// Virtual destructor
    virtual ~IPosition() = default;
};


}  // end namespaces
}
}
#endif  // end header latch
