#ifndef KIT_IO_RAM_INPUTOUTPUT_H_
#define KIT_IO_RAM_INPUTOUTPUT_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Io/IInputOutput.h"
#include "Kit/Container/RingBuffer.h"
#include "Kit/System/Semaphore.h"
#include "Kit/System/Mutex.h"

///
namespace Kit {
///
namespace Io {
///
namespace Ram {

/** This concrete class implements an InputOutput stream using a Ring
    Buffer.

    NOTES:
    - Thread safe and read and write operations have blocking semantics when
      the RingBuffer is empty or full, respectively.
    - For concurrent access, there can only be at a single ACTIVE reader and a
      single ACTIVE writer of the stream. In addition, the concurrent active
      reader/writer must be in different threads.  For serialized access there
      is no threading restrictions on the readers and writers.
    - This is NOT shared memory across processes, i.e. assumes a SINGLE
      address space.
    - Implementation is not necessarily efficient - it is more focused on
      clarity and robustness.
 */

class InputOutput : public Kit::Io::IInputOutput
{
public:
    /** Constructor. 'N' is the number of bytes in 'memoryBuffer'. The actual
        number of bytes that can be stored in the Ring Buffer is N-1.
     */
    InputOutput( uint8_t* memoryBuffer, unsigned N, bool initializeMemory = true  ) noexcept
        : m_ringBuffer( memoryBuffer, N, initializeMemory )
        , m_readerWaiting( false )
        , m_writerWaiting( false )
        , m_started( true )
    {
    }

public:
    /// Pull in overloaded methods from base class
    using Kit::Io::IInputOutput::read;

    /// See Kit::Io::IInput
    bool read( void* buffer, Kit::Type::SSize_T numBytes, Kit::Type::SSize_T& bytesRead ) noexcept override;

    /// See Kit::Io::IInput
    bool available() noexcept override;

public:
    /// Pull in overloaded methods from base class
    using Kit::Io::IInputOutput::write;

    /// See Kit::Io::IOutput
    bool write( const void* buffer, Kit::Type::SSize_T maxBytes, Kit::Type::SSize_T& bytesWritten ) noexcept override;

    /// See Kit::Io::IOutput
    void flush() noexcept override;

    /// See Kit::Io::IEos. Note: This method always returns false
    bool isEos() noexcept override;

    /// See Kit::Io::IOutput
    void close() noexcept override;

protected:
    /// Thread safe Ring buffer
    Kit::Container::RingBuffer<uint8_t> m_ringBuffer;

    /// Blocking read semantics when the RingBuffer is empty
    Kit::System::Semaphore m_semaReader;

    /// Blocking write semantics when the RingBuffer is full
    Kit::System::Semaphore m_semaWriter;

    /// Mutex to protect concurrent access to the RingBuffer
    Kit::System::Mutex m_lock;

    /// Track if there is blocked reader (NOTE: Not Protected by m_lock)
    volatile bool m_readerWaiting;

    /// Track if there is blocked writer (NOTE: Not Protected by m_lock)
    volatile bool m_writerWaiting;

    /// My open/close state (NOTE: Not Protected by m_lock)
    volatile bool m_started;
};

}  // end namespaces
}
}
#endif  // end header latch
