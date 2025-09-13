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
    the data to multiple (up to 4) output streams . The write() methods will
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
    /// Maximum number of output streams supported
    static constexpr int MAX_STREAMS = 4;

public:
    /// Constructor. Starts off with NO output streams
    TeeOutput() noexcept;

    /// Constructor. Starts off with ONE output stream
    TeeOutput( IOutput& streamA ) noexcept;

    /// Constructor. Starts off with TWO output streams
    TeeOutput( IOutput& streamA, IOutput& streamB ) noexcept;


public:
    /** Adds a stream.  Return true when successful; else false is returned
        if no more streams can be added
     */
    inline bool add( IOutput& stream ) noexcept { return add( m_streams, stream ); }

    /** Removes a stream.  Returns true if the stream was actually
        removed (i.e. that it was in the list to start with).

        NOTE: If the stream exists it will be removed regardless if it is
              considered good or failed.
     */
    bool remove( IOutput& stream ) noexcept;


public:
    /** Returns the first failed stream.  If no failed streams exist nullptr
        will be returned.
      */
    inline IOutput* firstFailed() noexcept { return first( m_failed ); }

    /** Returns the next failed stream in the list.  If there are no more
        failed streams nullptr will be returned.
     */
    inline IOutput* nextFailed( IOutput& currentFailedStream ) noexcept { return next( m_failed, currentFailedStream ); }

public:
    /// Pull in overloaded methods from base class
    using Kit::Io::IOutput::write;

    /// See Kit::Io::IOutput
    bool write( const void* buffer, ByteCount_T maxBytes, ByteCount_T& bytesWritten ) noexcept override;

    /// See Kit::Io::IOutput
    void flush() noexcept override;

    /// See Kit::Io::IEos
    bool isEos() noexcept override;

    /// See Kit::Io::IClose
    void close() noexcept override;

protected:
    /// Helper method
    IOutput* first( IOutput** streamList ) const noexcept;

    /// Helper method
    IOutput* next( IOutput** streamList, IOutput& currentStream ) const noexcept;

    /// Helper method
    bool remove( IOutput** streamList, IOutput& stream ) noexcept;

    /// Helper method
    bool add( IOutput** streamList, IOutput& stream ) noexcept;

protected:
    /// List of active writers
    mutable IOutput* m_streams[MAX_STREAMS];

    /// List of failed writers
    mutable IOutput* m_failed[MAX_STREAMS];

    /// my open/close state
    bool m_opened;
};

}  // end namespaces
}
#endif  // end header latch
