#ifndef KIT_LOGGING_FRAMEWORK_MOCKED4TEST_KIT_ONLY_H_
#define KIT_LOGGING_FRAMEWORK_MOCKED4TEST_KIT_ONLY_H_
/*------------------------------------------------------------------------------
 * Copyright Integer Fox Authors
 *
 * Distributed under the BSD 3 Clause License. See the license agreement at:
 * https://github.com/Integerfox/kit.core/blob/main/LICENSE
 *
 * Redistributions of the source code must retain the above copyright notice.
 *----------------------------------------------------------------------------*/
/** @file */

#include "Kit/Logging/Framework/IApplication.h"
#include "Kit/Logging/Framework/EntryData.h"
#include "Kit/Logging/Framework/NullPackage.h"
#include "Kit/Container/RingBuffer.h"  // TODO: Needs to be Kit::Container::RingBufferMP
#include "Kit/Logging/Framework/_0test/_3pkgs/PkgZ/Logging/Pkg/Package.h"
#include "Kit/Logging/Framework/_0test/_3pkgs/Foo/Logging/Pkg/Package.h"
#include "Kit/Logging/Pkg/Package.h"


///
namespace Foo {
///
namespace Logging {
///
namespace App {

/** This concrete class implements the ILog interface, provides a concrete
    IApplication instance, and the Log entry FIFO. The IApplication instance
    supports:
        - App Package
        - Kit Package
        - PkgZ Package
     */
class Foo : public Kit::Logging::Framework::IApplication
{
public:
    /// Constructor. TODO: Needs to be Kit::Container::RingBufferMP
    Foo( Kit::Container::RingBuffer<Kit::Logging::Framework::EntryData_T>& logFifo ) noexcept;

public:
    /// See Kit::Logging::Framework::IApplication
    const char* classificationIdToString( uint8_t classificationId ) noexcept override;

    /// See Kit::Logging::Framework::IApplication
    Kit::Logging::Framework::IPackage& getPackage( uint8_t packageId ) noexcept override;

protected:
    /// Internal Log entry FIFO storage
    Kit::Logging::Framework::EntryData_T* m_logFifoStorage;

protected:
    /// Foo Package instance, aka the Application's specific Package instance
    Pkg::Package m_fooPackage;

    /// The KIT Package instance
    Kit::Logging::Pkg::Package m_kitPackage;

    /// The PKGZ Package instance
    PkgZ::Logging::Pkg::Package m_pkgzPackage;

    /// Null Package instance to handle unknown Package IDs
    Kit::Logging::Framework::NullPackage m_nullPackage;
};

}  // end namespaces
}
}
#endif  // end header latch