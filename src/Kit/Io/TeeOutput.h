#ifndef Cpl_Io_TeeOutput_h_
#define Cpl_Io_TeeOutput_h_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/IOutput.h"
#include "Kit/Container/SList.h"


///
namespace Kit {
///
namespace Io {


/** This concrete class implements a IOutput stream that outputs
    the data to MANY (or none) output streams. The write() methods will
    return false when at least of the IOutput stream(s) had an error.  There
    are methods provided (firstFailed(), nextFailed()) which returns a list of
    all currently failed IOutput streams.  Once a stream has been marked as failed
    it will not be added back to the list of "good/active" streams.  The user
    must explicitly delete the stream and then re-add it. NOTE: There is no
    guaranty of order or 'atomicity' with respect to other streams when
    writing to multiple streams.

    NOTE: The implementation is NOT thread safe.  The application must provide
          its own locking/critical section logic if calling a TeeOutput instance
          from multiple threads.
 */
class TeeOutput : public IOutput
{
public:
    /// Constructor. Starts off with NO output streams
    TeeOutput() noexcept;

    /// Constructor. Starts off with ONE output stream
    TeeOutput( IOutput& streamA ) noexcept;

    /// Constructor. Starts off with TWO output streams
    TeeOutput( IOutput& streamA, IOutput& streamB ) noexcept;


public:
    /** Adds a stream */
    void add( IOutput& stream ) noexcept;

    /** Removes a stream.  Returns true if the stream was actually
        removed (i.e. that it was in the list to start with).

        NOTE: If the stream exists it will be removed regardless if it is
              considered good or failed.
     */
    bool remove( IOutput& stream ) noexcept;


public:
    /** Returns the first failed stream.  If no failed streams exist 0 will
        be returned.
      */
    IOutput* firstFailed() noexcept;

    /** Returns the next failed stream in the list.  If there are no more
        failed streams 0 will be returned.
     */
    IOutput* nextFailed( IOutput& currentFailedStream ) noexcept;

    /** Removes the specified failed stream AND returns the next failed stream
        in the list.  If there are no more failed streams (or the specified stream
        is not 'failed') 0 will be returned.  This method allows the user to remove
        the failed streams as he/she walks the failed list. NOTE: Do NOT call remove()
        while walking the failed list, remove() can invalidate the link pointers of the
        current failed stream object!!!
     */
    IOutput* removeAndGetNextFailed( IOutput& currentFailedStream ) noexcept;


public:
    /// Pull in overloaded methods from base class
    using Kit::Io::IOutput::write;


    /// See Kit::Io::IOutput
    bool write( const void* buffer, int maxBytes, int& bytesWritten ) noexcept override;

    /// See Kit::Io::IOutput
    void flush() noexcept override;

    /// See Kit::Io::IEos
    bool isEos() const noexcept override;

    /// See Kit::Io::IClose
    void close() noexcept override;

protected:
    /// List of active writers
    mutable Kit::Container::SList<IOutput> m_streams;

    /// List of failed writers
    mutable Kit::Container::SList<IOutput> m_failed;

    /// my open/close state
    bool m_opened;
};

}  // end namespaces
}
#endif  // end header latch
